#include "common/Hysteria2Client.h"
#include <QCoreApplication>

#include "base/Common.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/quic.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#include <QDateTime>
#include <QHostAddress>
#include <QHostInfo>
#include <QRandomGenerator>
#include <QSocketNotifier>
#include <QTimer>

#include <algorithm>

namespace core {
namespace {

// ---------------------------------------------------------------------------
//  QUIC 变长整数（RFC 9000 §16）
// ---------------------------------------------------------------------------
void putVarint(QByteArray &out, quint64 value)
{
    if (value <= 63) {
        out.append(char(value));
    } else if (value <= 16383) {
        out.append(char((value >> 8) | 0x40));
        out.append(char(value));
    } else if (value <= 0x3FFFFFFFULL) {
        out.append(char((value >> 24) | 0x80));
        out.append(char(value >> 16));
        out.append(char(value >> 8));
        out.append(char(value));
    } else {
        out.append(char((value >> 56) | 0xC0));
        for (int shift = 48; shift >= 0; shift -= 8) out.append(char(value >> shift));
    }
}

// 返回 false 表示数据不足（需要更多字节）
bool readVarint(const QByteArray &data, int &pos, quint64 *out)
{
    if (pos >= data.size()) return false;
    const uchar first = uchar(data.at(pos));
    const int   bytes = 1 << (first >> 6);
    if (pos + bytes > data.size()) return false;
    quint64 value = first & 0x3F;
    for (int i = 1; i < bytes; ++i) value = (value << 8) | uchar(data.at(pos + i));
    pos += bytes;
    *out = value;
    return true;
}

// 带 N 位前缀的整数（QPACK 用），数据不足返回 false
bool readPrefixedInt(const QByteArray &data, int &pos, int prefixBits, quint64 *out)
{
    if (pos >= data.size()) return false;
    const quint64 mask = (quint64(1) << prefixBits) - 1;
    quint64       value = uchar(data.at(pos)) & mask;
    ++pos;
    if (value < mask) {
        *out = value;
        return true;
    }
    int shift = 0;
    for (;;) {
        if (pos >= data.size() || shift > 56) return false;
        const uchar byte = uchar(data.at(pos++));
        value += quint64(byte & 0x7F) << shift;
        shift += 7;
        if (!(byte & 0x80)) break;
    }
    *out = value;
    return true;
}

void putPrefixedInt(QByteArray &out, uchar highBits, int prefixBits, quint64 value)
{
    const quint64 mask = (quint64(1) << prefixBits) - 1;
    if (value < mask) {
        out.append(char(highBits | uchar(value)));
        return;
    }
    out.append(char(highBits | uchar(mask)));
    value -= mask;
    while (value >= 128) {
        out.append(char(uchar(value & 0x7F) | 0x80));
        value >>= 7;
    }
    out.append(char(uchar(value)));
}

// QPACK 字符串（H 位 + 7 位长度前缀；本实现对字符串一律不压缩）
void putQpackString(QByteArray &out, const QByteArray &text)
{
    putPrefixedInt(out, 0x00, 7, quint64(text.size()));
    out.append(text);
}

// ---------------------------------------------------------------------------
//  QPACK 静态表（RFC 9204 附录 A），解码应答时按索引取名字
// ---------------------------------------------------------------------------
struct StaticEntry {
    const char *name;
};

const StaticEntry kQpackStatic[] = {
    {":authority"}, {":path"}, {"age"}, {"content-disposition"}, {"content-length"},
    {"cookie"}, {"date"}, {"etag"}, {"if-modified-since"}, {"if-none-match"},
    {"last-modified"}, {"link"}, {"location"}, {"referer"}, {"set-cookie"},
    {":method"}, {":method"}, {":method"}, {":method"}, {":method"},
    {":method"}, {":method"}, {":scheme"}, {":scheme"}, {":status"},
    {":status"}, {":status"}, {":status"}, {":status"}, {"accept"},
    {"accept"}, {"accept-encoding"}, {"accept-ranges"}, {"access-control-allow-headers"},
    {"access-control-allow-headers"}, {"access-control-allow-origin"}, {"cache-control"},
    {"cache-control"}, {"cache-control"}, {"cache-control"}, {"cache-control"},
    {"cache-control"}, {"content-encoding"}, {"content-encoding"}, {"content-type"},
    {"content-type"}, {"content-type"}, {"content-type"}, {"content-type"},
    {"content-type"}, {"content-type"}, {"content-type"}, {"content-type"},
    {"content-type"}, {"range"}, {"strict-transport-security"},
    {"strict-transport-security"}, {"strict-transport-security"}, {"vary"}, {"vary"},
    {"x-content-type-options"}, {"x-xss-protection"}, {":status"}, {":status"},
    {":status"}, {":status"}, {":status"}, {":status"}, {":status"},
    {":status"}, {":status"}, {"accept-language"},
    {"access-control-allow-credentials"}, {"access-control-allow-credentials"},
    {"access-control-allow-headers"}, {"access-control-allow-methods"},
    {"access-control-allow-methods"}, {"access-control-allow-methods"},
    {"access-control-expose-headers"}, {"access-control-request-headers"},
    {"access-control-request-method"}, {"access-control-request-method"}, {"alt-svc"},
    {"authorization"}, {"content-security-policy"}, {"early-data"}, {"expect-ct"},
    {"forwarded"}, {"if-range"}, {"origin"}, {"purpose"}, {"server"},
    {"timing-allow-origin"}, {"upgrade-insecure-requests"}, {"user-agent"},
    {"x-forwarded-for"}, {"x-frame-options"},
};

const char *qpackStaticName(quint64 index)
{
    if (index < sizeof(kQpackStatic) / sizeof(kQpackStatic[0])) {
        return kQpackStatic[index].name;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
//  HPACK/QPACK Huffman 解码（RFC 7541 附录 B，服务端字符串压缩）
// ---------------------------------------------------------------------------
// RFC 7541 附录 B：HPACK/QPACK Huffman 码表（symbol -> code, bits）
static const uint32_t kHuffCode[257] = {
    0x00001ff8u, 0x007fffd8u, 0x0fffffe2u, 0x0fffffe3u, 0x0fffffe4u, 0x0fffffe5u,
    0x0fffffe6u, 0x0fffffe7u, 0x0fffffe8u, 0x00ffffeau, 0x3ffffffcu, 0x0fffffe9u,
    0x0fffffeau, 0x3ffffffdu, 0x0fffffebu, 0x0fffffecu, 0x0fffffedu, 0x0fffffeeu,
    0x0fffffefu, 0x0ffffff0u, 0x0ffffff1u, 0x0ffffff2u, 0x3ffffffeu, 0x0ffffff3u,
    0x0ffffff4u, 0x0ffffff5u, 0x0ffffff6u, 0x0ffffff7u, 0x0ffffff8u, 0x0ffffff9u,
    0x0ffffffau, 0x0ffffffbu, 0x00000014u, 0x000003f8u, 0x000003f9u, 0x00000ffau,
    0x00001ff9u, 0x00000015u, 0x000000f8u, 0x000007fau, 0x000003fau, 0x000003fbu,
    0x000000f9u, 0x000007fbu, 0x000000fau, 0x00000016u, 0x00000017u, 0x00000018u,
    0x00000000u, 0x00000001u, 0x00000002u, 0x00000019u, 0x0000001au, 0x0000001bu,
    0x0000001cu, 0x0000001du, 0x0000001eu, 0x0000001fu, 0x0000005cu, 0x000000fbu,
    0x00007ffcu, 0x00000020u, 0x00000ffbu, 0x000003fcu, 0x00001ffau, 0x00000021u,
    0x0000005du, 0x0000005eu, 0x0000005fu, 0x00000060u, 0x00000061u, 0x00000062u,
    0x00000063u, 0x00000064u, 0x00000065u, 0x00000066u, 0x00000067u, 0x00000068u,
    0x00000069u, 0x0000006au, 0x0000006bu, 0x0000006cu, 0x0000006du, 0x0000006eu,
    0x0000006fu, 0x00000070u, 0x00000071u, 0x00000072u, 0x000000fcu, 0x00000073u,
    0x000000fdu, 0x00001ffbu, 0x0007fff0u, 0x00001ffcu, 0x00003ffcu, 0x00000022u,
    0x00007ffdu, 0x00000003u, 0x00000023u, 0x00000004u, 0x00000024u, 0x00000005u,
    0x00000025u, 0x00000026u, 0x00000027u, 0x00000006u, 0x00000074u, 0x00000075u,
    0x00000028u, 0x00000029u, 0x0000002au, 0x00000007u, 0x0000002bu, 0x00000076u,
    0x0000002cu, 0x00000008u, 0x00000009u, 0x0000002du, 0x00000077u, 0x00000078u,
    0x00000079u, 0x0000007au, 0x0000007bu, 0x00007ffeu, 0x000007fcu, 0x00003ffdu,
    0x00001ffdu, 0x0ffffffcu, 0x000fffe6u, 0x003fffd2u, 0x000fffe7u, 0x000fffe8u,
    0x003fffd3u, 0x003fffd4u, 0x003fffd5u, 0x007fffd9u, 0x003fffd6u, 0x007fffdau,
    0x007fffdbu, 0x007fffdcu, 0x007fffddu, 0x007fffdeu, 0x00ffffebu, 0x007fffdfu,
    0x00ffffecu, 0x00ffffedu, 0x003fffd7u, 0x007fffe0u, 0x00ffffeeu, 0x007fffe1u,
    0x007fffe2u, 0x007fffe3u, 0x007fffe4u, 0x001fffdcu, 0x003fffd8u, 0x007fffe5u,
    0x003fffd9u, 0x007fffe6u, 0x007fffe7u, 0x00ffffefu, 0x003fffdau, 0x001fffddu,
    0x000fffe9u, 0x003fffdbu, 0x003fffdcu, 0x007fffe8u, 0x007fffe9u, 0x001fffdeu,
    0x007fffeau, 0x003fffddu, 0x003fffdeu, 0x00fffff0u, 0x001fffdfu, 0x003fffdfu,
    0x007fffebu, 0x007fffecu, 0x001fffe0u, 0x001fffe1u, 0x003fffe0u, 0x001fffe2u,
    0x007fffedu, 0x003fffe1u, 0x007fffeeu, 0x007fffefu, 0x000fffeau, 0x003fffe2u,
    0x003fffe3u, 0x003fffe4u, 0x007ffff0u, 0x003fffe5u, 0x003fffe6u, 0x007ffff1u,
    0x03ffffe0u, 0x03ffffe1u, 0x000fffebu, 0x0007fff1u, 0x003fffe7u, 0x007ffff2u,
    0x003fffe8u, 0x01ffffecu, 0x03ffffe2u, 0x03ffffe3u, 0x03ffffe4u, 0x07ffffdeu,
    0x07ffffdfu, 0x03ffffe5u, 0x00fffff1u, 0x01ffffedu, 0x0007fff2u, 0x001fffe3u,
    0x03ffffe6u, 0x07ffffe0u, 0x07ffffe1u, 0x03ffffe7u, 0x07ffffe2u, 0x00fffff2u,
    0x001fffe4u, 0x001fffe5u, 0x03ffffe8u, 0x03ffffe9u, 0x0ffffffdu, 0x07ffffe3u,
    0x07ffffe4u, 0x07ffffe5u, 0x000fffecu, 0x00fffff3u, 0x000fffedu, 0x001fffe6u,
    0x003fffe9u, 0x001fffe7u, 0x001fffe8u, 0x007ffff3u, 0x003fffeau, 0x003fffebu,
    0x01ffffeeu, 0x01ffffefu, 0x00fffff4u, 0x00fffff5u, 0x03ffffeau, 0x007ffff4u,
    0x03ffffebu, 0x07ffffe6u, 0x03ffffecu, 0x03ffffedu, 0x07ffffe7u, 0x07ffffe8u,
    0x07ffffe9u, 0x07ffffeau, 0x07ffffebu, 0x0ffffffeu, 0x07ffffecu, 0x07ffffedu,
    0x07ffffeeu, 0x07ffffefu, 0x07fffff0u, 0x03ffffeeu, 0x3fffffffu,
};
static const uint8_t kHuffBits[257] = {
    13, 23, 28, 28, 28, 28, 28, 28, 28, 24, 30, 28,
    28, 30, 28, 28, 28, 28, 28, 28, 28, 28, 30, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 6, 10, 10, 12,
    13, 6, 8, 11, 10, 10, 8, 11, 8, 6, 6, 6,
    5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 8,
    15, 6, 12, 10, 13, 6, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 8, 7, 8, 13, 19, 13, 14, 6,
    15, 5, 6, 5, 6, 5, 6, 6, 6, 5, 7, 7,
    6, 6, 6, 5, 6, 7, 6, 5, 5, 6, 7, 7,
    7, 7, 7, 15, 11, 14, 13, 28, 20, 22, 20, 20,
    22, 22, 22, 23, 22, 23, 23, 23, 23, 23, 24, 23,
    24, 24, 22, 23, 24, 23, 23, 23, 23, 21, 22, 23,
    22, 23, 23, 24, 22, 21, 20, 22, 22, 23, 23, 21,
    23, 22, 22, 24, 21, 22, 23, 23, 21, 21, 22, 21,
    23, 22, 23, 23, 20, 22, 22, 22, 23, 22, 22, 23,
    26, 26, 20, 19, 22, 23, 22, 25, 26, 26, 26, 27,
    27, 26, 24, 25, 19, 21, 26, 27, 27, 26, 27, 24,
    21, 21, 26, 26, 28, 27, 27, 27, 20, 24, 20, 21,
    22, 21, 21, 23, 22, 22, 25, 25, 24, 24, 26, 23,
    26, 27, 26, 26, 27, 27, 27, 27, 27, 28, 27, 27,
    27, 27, 27, 26, 30,
};

// 解码 Huffman 编码的字符串；数据不足或编码非法返回 false
bool huffmanDecode(const QByteArray &in, int &pos, int length, QByteArray *out)
{
    const int end = pos + length;
    if (end > in.size()) return false;
    quint64 current = 0;
    int     bits    = 0;
    for (int i = pos; i < end; ++i) {
        for (int b = 7; b >= 0; --b) {
            current = (current << 1) | quint64((uchar(in.at(i)) >> b) & 1);
            ++bits;
            if (bits > 30) return false;
            for (int symbol = 0; symbol < 257; ++symbol) {
                if (kHuffBits[symbol] == bits && kHuffCode[symbol] == current) {
                    if (symbol == 256) return false; // 中途出现 EOS：非法
                    out->append(char(symbol));
                    current = 0;
                    bits    = 0;
                    break;
                }
            }
        }
    }
    if (bits > 7) return false; // 尾部填充不得超过 7 位
    pos = end;
    return true;
}

// QPACK 字符串解码（数据不足返回 false）
bool readQpackString(const QByteArray &data, int &pos, QByteArray *out)
{
    if (pos >= data.size()) return false;
    const bool huffman = (uchar(data.at(pos)) & 0x80) != 0;
    quint64    length  = 0;
    if (!readPrefixedInt(data, pos, 7, &length)) return false;
    if (quint64(pos) + length > quint64(data.size())) return false;
    if (huffman) return huffmanDecode(data, pos, int(length), out);
    out->append(data.mid(pos, int(length)));
    pos += int(length);
    return true;
}

// ---------------------------------------------------------------------------
//  Hysteria2 协议常量（与上游 core/internal/protocol 一致）
// ---------------------------------------------------------------------------
const char  kAuthHost[]   = "hysteria";
const char  kAuthPath[]   = "/auth";
const char  kHeaderAuth[] = "hysteria-auth";
const char  kHeaderCcRx[] = "hysteria-cc-rx";
const char  kHeaderPad[]  = "hysteria-padding";
const char  kHeaderUdp[]  = "hysteria-udp";
const int   kStatusAuthOk = 233;
const quint64 kFrameTypeTcpRequest = 0x401;
const int   kMaxAddressLength  = 2048;

// 随机填充字符集，与上游 padding.go 相同
const char kPaddingChars[] = "abcdefghijklmnopqrstuvwxyz"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

QByteArray randomPadding(int minLength, int maxLength)
{
    const int length = minLength
                       + int(QRandomGenerator::global()->bounded(quint32(
                             std::max(1, maxLength - minLength))));
    QByteArray out;
    out.reserve(length);
    for (int i = 0; i < length; ++i) {
        out.append(kPaddingChars[QRandomGenerator::global()->bounded(
            quint32(sizeof(kPaddingChars) - 1))]);
    }
    return out;
}

qint64 nowMs()
{
    return QDateTime::currentMSecsSinceEpoch();
}

QString sslErrorText()
{
    const unsigned long code = ERR_peek_last_error();
    if (code == 0) return QCoreApplication::translate("clashqt::core", "Unknown error");
    char buffer[256] = {0};
    ERR_error_string_n(code, buffer, sizeof(buffer));
    return QString::fromLatin1(buffer);
}

// 流是否已被对端正常关闭（FIN）——OpenSSL 用 SSL_ERROR_SSL + 该原因码表示
bool isStreamFinished(SSL *stream, int sslError)
{
    if (sslError == SSL_ERROR_ZERO_RETURN) return true;
    if (sslError != SSL_ERROR_SSL) return false;
#ifdef SSL_R_STREAM_FINISHED
    if (ERR_GET_REASON(ERR_peek_last_error()) == SSL_R_STREAM_FINISHED) return true;
#endif
    return false;
}

// ---------------------------------------------------------------------------
//  HTTP/3 帧与请求构造
// ---------------------------------------------------------------------------
const quint64 kHttp3FrameHeaders  = 0x01;
const quint64 kHttp3FrameSettings = 0x04;
const quint64 kHttp3StreamControl = 0x00;

// 控制流：流类型(0x00) + SETTINGS 帧。QPACK 动态表容量声明为 0，
// 因此后续字段段只会使用静态表与字面量（无需编码器/解码器流）
QByteArray buildControlStream()
{
    QByteArray settings;
    putVarint(settings, 0x01); // SETTINGS_QPACK_MAX_TABLE_CAPACITY
    putVarint(settings, 0);
    putVarint(settings, 0x06); // SETTINGS_MAX_FIELD_SECTION_SIZE
    putVarint(settings, 65536);
    putVarint(settings, 0x07); // SETTINGS_QPACK_BLOCKED_STREAMS
    putVarint(settings, 0);

    QByteArray out;
    putVarint(out, kHttp3StreamControl);
    putVarint(out, kHttp3FrameSettings);
    putVarint(out, quint64(settings.size()));
    out.append(settings);
    return out;
}

// 鉴权请求：HTTP/3 HEADERS 帧，QPACK 字段段
QByteArray buildAuthRequest(const QString &password)
{
    QByteArray section;
    // 字段段前缀：Required Insert Count = 0，Delta Base = 0
    section.append(char(0x00));
    section.append(char(0x00));
    section.append(char(0xC0 | 20)); // :method: POST（静态索引 20，完全匹配）
    section.append(char(0xC0 | 23)); // :scheme: https（静态索引 23）
    putPrefixedInt(section, 0x50, 4, 0); // :authority（静态索引 0 为名）
    putQpackString(section, QByteArray(kAuthHost));
    putPrefixedInt(section, 0x50, 4, 1); // :path（静态索引 1）
    putQpackString(section, QByteArray(kAuthPath));

    // Hysteria-Auth：节点密码
    putPrefixedInt(section, 0x20, 3, quint64(sizeof(kHeaderAuth) - 1));
    section.append(kHeaderAuth);
    putQpackString(section, password.toUtf8());

    // Hysteria-CC-RX：0 表示不声明本机带宽，交由服务端默认拥塞控制
    putPrefixedInt(section, 0x20, 3, quint64(sizeof(kHeaderCcRx) - 1));
    section.append(kHeaderCcRx);
    putQpackString(section, QByteArray("0"));

    // Hysteria-Padding：随机填充
    putPrefixedInt(section, 0x20, 3, quint64(sizeof(kHeaderPad) - 1));
    section.append(kHeaderPad);
    putQpackString(section, randomPadding(256, 2048));

    QByteArray out;
    putVarint(out, kHttp3FrameHeaders);
    putVarint(out, quint64(section.size()));
    out.append(section);
    return out;
}

// 解析应答的 QPACK 字段段，提取 :status 与 hysteria-udp
// 返回 true 表示已解析出 :status
bool parseAuthResponseSection(const QByteArray &section, int *status, bool *udpEnabled)
{
    int pos = 0;
    // 字段段前缀
    quint64 insertCount = 0;
    quint64 deltaBase   = 0;
    if (!readPrefixedInt(section, pos, 8, &insertCount)) return false;
    if (!readPrefixedInt(section, pos, 7, &deltaBase)) return false;

    while (pos < section.size()) {
        const uchar first = uchar(section.at(pos));
        QByteArray  name;
        QByteArray  value;

        if (first & 0x80) { // 索引字段行：名与值都来自表
            quint64 index = 0;
            if (!readPrefixedInt(section, pos, 6, &index)) return false;
            const bool isStatic = (first & 0x40) != 0;
            if (!isStatic) return false; // 未使用动态表
            const char *entry = qpackStaticName(index);
            if (!entry) return false;
            name = entry;
            // 静态表中 :status 的值随索引变化，本实现只关心名，值另行判断
            if (name == ":status") {
                static const struct {
                    quint64 index;
                    int     status;
                } kStatusTable[] = {
                    {24, 103}, {25, 200}, {26, 304}, {27, 404}, {28, 503},
                    {63, 100}, {64, 204}, {65, 206}, {66, 302}, {67, 400},
                    {68, 403}, {69, 421}, {70, 425}, {71, 500},
                };
                for (const auto &item : kStatusTable) {
                    if (item.index == index) *status = item.status;
                }
            }
        } else if (first & 0x40) { // 名引用 + 字面量值
            quint64 index = 0;
            if (!readPrefixedInt(section, pos, 4, &index)) return false;
            const bool isStatic = (first & 0x10) != 0;
            if (!readQpackString(section, pos, &value)) return false;
            if (isStatic) {
                const char *entry = qpackStaticName(index);
                if (entry) name = entry;
            }
            if (name == ":status") *status = value.toInt();
        } else if (first & 0x20) { // 字面量名 + 字面量值
            const bool huffmanName = (first & 0x08) != 0;
            quint64    nameLength  = 0;
            if (!readPrefixedInt(section, pos, 3, &nameLength)) return false;
            if (quint64(pos) + nameLength > quint64(section.size())) return false;
            if (huffmanName) {
                if (!huffmanDecode(section, pos, int(nameLength), &name)) return false;
            } else {
                name.append(section.mid(pos, int(nameLength)));
                pos += int(nameLength);
            }
            if (!readQpackString(section, pos, &value)) return false;
            if (name == ":status") *status = value.toInt();
            if (name == kHeaderUdp) *udpEnabled = (value == "true" || value == "1");
        } else {
            return false; // 后基索引等形态未使用
        }

        if (!name.isEmpty() && name == kHeaderUdp && !value.isEmpty()) {
            *udpEnabled = (value == "true" || value == "1");
        }
    }
    return true;
}

} // namespace

// ===========================================================================
//  Hysteria2Stream
// ===========================================================================

Hysteria2Stream::Hysteria2Stream(std::shared_ptr<Hysteria2Session> session, SSL *stream,
                                 QObject *parent)
    : OutboundStream(parent), m_session(std::move(session)), m_stream(stream)
{
    if (m_session) m_session->m_streams.append(this);
}

Hysteria2Stream::~Hysteria2Stream()
{
    if (m_stream) {
        SSL_free(m_stream);
        m_stream = nullptr;
    }
    if (m_session) m_session->detachStream(this);
}

void Hysteria2Stream::prependPlaintext(const QByteArray &data)
{
    if (data.isEmpty()) return;
    m_in.prepend(data);
}

void Hysteria2Stream::markBroken(const QString &reason)
{
    if (m_closed) return;
    m_error = reason;
    m_eof   = true;
    emit streamClosed();
}

QByteArray Hysteria2Stream::readAll()
{
    // 只返回已缓存的数据：驱动网络由会话的 notifier/定时器负责。
    // 若在此调用 pump()，pullFromQuic() 会在同一次调用栈里重新 emit
    // readyToRead()，造成 readAll -> pump -> readyToRead -> readAll 的递归。
    const QByteArray out = m_in;
    m_in.clear();
    return out;
}

void Hysteria2Stream::pullFromQuic()
{
    if (!m_stream || m_closed || m_eof) return;

    bool gotData = false;
    bool finished = false;
    for (;;) {
        char      buffer[16384];
        const int n = SSL_read(m_stream, buffer, sizeof(buffer));
        if (n > 0) {
            m_in.append(buffer, n);
            gotData = true;
            continue;
        }
        const int sslError = SSL_get_error(m_stream, n);
        if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE) break;
        if (isStreamFinished(m_stream, sslError)) {
            finished = true;
            break;
        }
        // QUIC 流被重置等：把原因带给上层
        m_error   = sslErrorText();
        finished  = true;
        break;
    }

    if (gotData) emit readyToRead();
    if (finished && !m_eof) {
        m_eof = true;
        // 先把已到达的数据交付，再报告关闭
        if (gotData) emit readyToRead();
        emit streamClosed();
    }
}

qint64 Hysteria2Stream::write(const QByteArray &data)
{
    if (!m_stream || m_closed || m_eof) {
        if (m_error.isEmpty()) m_error = QCoreApplication::translate("clashqt::core", "Stream closed");
        return -1;
    }
    if (data.isEmpty()) return 0;

    int          offset   = 0;
    const qint64 deadline = nowMs() + 15000;
    while (offset < data.size()) {
        const int n = SSL_write(m_stream, data.constData() + offset, data.size() - offset);
        if (n > 0) {
            offset += n;
            continue;
        }
        const int sslError = SSL_get_error(m_stream, n);
        if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE) {
            // 流控窗口已满：驱动网络把窗口腾出来
            if (nowMs() > deadline) {
                m_error = QCoreApplication::translate("clashqt::core", "Write timed out (flow-control window stayed closed)");
                return -1;
            }
            if (!m_session) {
                m_error = QCoreApplication::translate("clashqt::core", "Session released");
                return -1;
            }
            m_session->pump();
            continue;
        }
        m_error = sslErrorText();
        emit streamClosed();
        return -1;
    }

    // 尽量把刚写入的数据刷到网络（QUIC 需要驱动 SSL_handle_events）
    if (m_session) m_session->flushOutgoing();
    return data.size();
}

void Hysteria2Stream::closeStream()
{
    if (m_closed) return;
    m_closed = true;

    if (m_stream) {
        // 发送 FIN 让服务端知道本次请求结束；随后冲刷一次
        SSL_stream_conclude(m_stream, 0);
        if (m_session) m_session->flushOutgoing();
        SSL_free(m_stream);
        m_stream = nullptr;
    }
    if (m_session) {
        m_session->detachStream(this);
        m_session.reset(); // 每条会话只服务一条流，随流一起释放
    }
}

QString Hysteria2Stream::errorText() const
{
    if (!m_error.isEmpty()) return m_error;
    if (m_session && !m_session->isOpen()) return m_session->lastError();
    return {};
}

bool Hysteria2Stream::isOpen() const
{
    // 仍有未取走的数据时视为可用，避免中继漏掉已到达的尾部数据
    if (!m_in.isEmpty() && !m_closed) return true;
    if (m_closed || m_eof || !m_stream) return false;
    return !m_session || m_session->isOpen();
}

// ===========================================================================
//  Hysteria2Session
// ===========================================================================

Hysteria2Session::Hysteria2Session(QObject *parent) : QObject(parent) {}

Hysteria2Session::~Hysteria2Session()
{
    closeNow(QCoreApplication::translate("clashqt::core", "Session release"));
}

bool Hysteria2Session::openSocket(QString *error)
{
    // 端口：ports 区间内随机取一个（服务端整段监听）；否则用主端口
    qint32 port = m_peerPort;
    if (!m_portRange.isEmpty()) {
        const QStringList parts = m_portRange.split(QLatin1Char('-'));
        if (parts.size() == 2) {
            const int low  = parts.at(0).trimmed().toInt();
            const int high = parts.at(1).trimmed().toInt();
            if (low > 0 && high >= low) {
                port = low + int(QRandomGenerator::global()->bounded(
                                   quint32(high - low + 1)));
            }
        } else if (parts.size() == 1) {
            const int single = parts.at(0).trimmed().toInt();
            if (single > 0) port = single;
        }
    }
    m_peerPort = quint16(port);

    // 解析服务器地址（可能是域名）
    QHostAddress address;
    if (!address.setAddress(m_server)) {
        const QHostInfo info = QHostInfo::fromName(m_server);
        if (info.error() != QHostInfo::NoError || info.addresses().isEmpty()) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "Could not parse server address %1 (%2)")
                             .arg(m_server, info.errorString());
            }
            return false;
        }
        for (const QHostAddress &candidate : info.addresses()) {
            if (candidate.protocol() == QAbstractSocket::IPv4Protocol) {
                address = candidate;
                break;
            }
        }
        if (address.isNull()) address = info.addresses().first();
    }

    const int family = address.protocol() == QAbstractSocket::IPv6Protocol ? AF_INET6
                                                                          : AF_INET;
    const int fd     = ::socket(family, SOCK_DGRAM, 0);
    if (fd < 0) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to create the UDP socket: %1").arg(strerror(errno));
        return false;
    }

    // 非阻塞：QUIC 事件驱动要求套接字不阻塞读取
    const int flags = ::fcntl(fd, F_GETFL, 0);
    ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    if (family == AF_INET) {
        sockaddr_in peer{};
        peer.sin_family = AF_INET;
        peer.sin_port   = htons(m_peerPort);
        peer.sin_addr.s_addr = htonl(address.toIPv4Address());
        if (::connect(fd, reinterpret_cast<sockaddr *>(&peer), sizeof(peer)) != 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "UDP connect failed: %1").arg(strerror(errno));
            ::close(fd);
            return false;
        }
    } else {
        sockaddr_in6 peer{};
        peer.sin6_family = AF_INET6;
        peer.sin6_port   = htons(m_peerPort);
        const Q_IPV6ADDR v6 = address.toIPv6Address();
        std::memcpy(&peer.sin6_addr, v6.c, 16);
        if (::connect(fd, reinterpret_cast<sockaddr *>(&peer), sizeof(peer)) != 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "UDP connect failed: %1").arg(strerror(errno));
            ::close(fd);
            return false;
        }
    }
    m_fd = fd;
    return true;
}

bool Hysteria2Session::handshake(const Hysteria2Config &config, int timeoutMs,
                                 QString *error)
{
    SSL_CTX *ctx = SSL_CTX_new(OSSL_QUIC_client_method());
    if (!ctx) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to create the QUIC context");
        return false;
    }
    if (config.skipVerify) {
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
    } else {
        // 标准证书链校验；配置了 fingerprint 时额外做指纹固定（见下）
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);
        SSL_CTX_set_default_verify_paths(ctx);
    }

    SSL *ssl = SSL_new(ctx);
    SSL_CTX_free(ctx); // SSL 已持有引用
    if (!ssl) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to create the QUIC connection");
        return false;
    }

    const QByteArray sni = config.serverName.toUtf8();
    if (!sni.isEmpty()) SSL_set_tlsext_host_name(ssl, sni.constData());
    static const unsigned char kAlpn[] = {2, 'h', '3'};
    SSL_set_alpn_protos(ssl, kAlpn, sizeof(kAlpn));

    BIO *bio = BIO_new_dgram(m_fd, BIO_NOCLOSE);
    if (!bio) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to bind the UDP socket");
        SSL_free(ssl);
        return false;
    }
    SSL_set_bio(ssl, bio, bio);
    SSL_set_blocking_mode(ssl, 0);
    SSL_set_default_stream_mode(ssl, SSL_DEFAULT_STREAM_MODE_NONE);
    // 服务端会自行开启控制流 / QPACK 流，必须允许接收，否则会被判为拒绝流
    SSL_set_incoming_stream_policy(ssl, SSL_INCOMING_STREAM_POLICY_ACCEPT, 0);

    sockaddr_storage storage{};
    socklen_t        storageLen = 0;
    if (::getpeername(m_fd, reinterpret_cast<sockaddr *>(&storage), &storageLen) != 0) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to read the peer address: %1").arg(strerror(errno));
        SSL_free(ssl);
        return false;
    }
    BIO_ADDR *peerAddress = BIO_ADDR_new();
    bool      addrOk      = false;
    if (storage.ss_family == AF_INET) {
        const auto *v4 = reinterpret_cast<const sockaddr_in *>(&storage);
        addrOk = BIO_ADDR_rawmake(peerAddress, AF_INET, &v4->sin_addr,
                                  sizeof(v4->sin_addr), ntohs(v4->sin_port))
                 == 1;
    } else {
        const auto *v6 = reinterpret_cast<const sockaddr_in6 *>(&storage);
        addrOk = BIO_ADDR_rawmake(peerAddress, AF_INET6, &v6->sin6_addr,
                                  sizeof(v6->sin6_addr), ntohs(v6->sin6_port))
                 == 1;
    }
    if (addrOk) SSL_set1_initial_peer_addr(ssl, peerAddress);
    BIO_ADDR_free(peerAddress);
    if (!addrOk) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to build the initial QUIC peer address");
        SSL_free(ssl);
        return false;
    }

    m_ssl = ssl;

    const qint64 deadline = nowMs() + timeoutMs;
    for (;;) {
        const int rv = SSL_connect(m_ssl);
        if (rv == 1) break;

        const int sslError = SSL_get_error(m_ssl, rv);
        if (sslError != SSL_ERROR_WANT_READ && sslError != SSL_ERROR_WANT_WRITE) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "QUIC handshake failed: %1").arg(sslErrorText());
            }
            return false;
        }

        int waitMs = 200;
        struct timeval tv;
        int            infinite = 0;
        if (SSL_get_event_timeout(m_ssl, &tv, &infinite) == 1 && !infinite) {
            const int suggested = int(tv.tv_sec) * 1000 + int(tv.tv_usec) / 1000;
            if (suggested >= 0 && suggested < waitMs) waitMs = suggested;
        }
        struct pollfd pfd{m_fd, POLLIN, 0};
        ::poll(&pfd, 1, waitMs);
        if (SSL_handle_events(m_ssl) != 1) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "QUIC event handling failed: %1").arg(sslErrorText());
            return false;
        }
        if (nowMs() > deadline) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "QUIC handshake timed out (port %1)").arg(m_peerPort);
            }
            return false;
        }
    }

    // 证书指纹固定（clash 的 fingerprint 字段为证书 SHA-256 十六进制）
    if (!config.fingerprint.isEmpty()) {
        X509 *cert = SSL_get1_peer_certificate(m_ssl);
        if (!cert) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "The server provided no certificate");
            return false;
        }
        unsigned char digest[EVP_MAX_MD_SIZE] = {0};
        unsigned int  digestLen               = 0;
        const bool    digested = X509_digest(cert, EVP_sha256(), digest, &digestLen) == 1;
        X509_free(cert);
        if (!digested || digestLen != 32
            || QByteArray(reinterpret_cast<const char *>(digest), 32)
                   != config.fingerprint) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "Server certificate fingerprint does not match the config (fingerprint check failed)");
            }
            return false;
        }
    }

    m_open = true;
    return true;
}

SSL *Hysteria2Session::openQuicStream(bool uni, const QByteArray &payload, bool conclude,
                                      QString *error)
{
    SSL *stream = nullptr;
    const qint64 deadline = nowMs() + 10000;
    for (;;) {
        stream = SSL_new_stream(m_ssl, (uni ? SSL_STREAM_FLAG_UNI : 0)
                                           | SSL_STREAM_FLAG_NO_BLOCK);
        if (stream) break;
        const int sslError = SSL_get_error(m_ssl, 0);
        if (sslError != SSL_ERROR_WANT_READ && sslError != SSL_ERROR_WANT_WRITE) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to create a QUIC stream: %1").arg(sslErrorText());
            return nullptr;
        }
        if (nowMs() > deadline) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Timed out creating a QUIC stream");
            return nullptr;
        }
        struct pollfd pfd{m_fd, POLLIN, 0};
        ::poll(&pfd, 1, 100);
        if (SSL_handle_events(m_ssl) != 1) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "QUIC event handling failed: %1").arg(sslErrorText());
            return nullptr;
        }
    }

    int offset = 0;
    while (offset < payload.size()) {
        const int n = SSL_write(stream, payload.constData() + offset,
                                payload.size() - offset);
        if (n > 0) {
            offset += n;
            continue;
        }
        const int sslError = SSL_get_error(stream, n);
        if (sslError != SSL_ERROR_WANT_READ && sslError != SSL_ERROR_WANT_WRITE) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to write to a QUIC stream: %1").arg(sslErrorText());
            SSL_free(stream);
            return nullptr;
        }
        if (nowMs() > deadline) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Timed out writing to a QUIC stream");
            SSL_free(stream);
            return nullptr;
        }
        struct pollfd pfd{m_fd, POLLIN, 0};
        ::poll(&pfd, 1, 100);
        if (SSL_handle_events(m_ssl) != 1) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "QUIC event handling failed: %1").arg(sslErrorText());
            SSL_free(stream);
            return nullptr;
        }
    }
    if (conclude) SSL_stream_conclude(stream, 0);
    if (SSL_handle_events(m_ssl) != 1) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "QUIC event handling failed: %1").arg(sslErrorText());
        SSL_free(stream);
        return nullptr;
    }
    return stream;
}

bool Hysteria2Session::authenticate(int timeoutMs, QString *error)
{
    // 1) 控制流（HTTP/3 要求客户端发送 SETTINGS）
    if (!openQuicStream(true, buildControlStream(), false, error)) return false;

    // 2) 鉴权请求流
    SSL *request = openQuicStream(false, buildAuthRequest(m_password), true, error);
    if (!request) return false;

    // 3) 读应答：HTTP/3 帧，取 HEADERS 里的 :status
    QByteArray buffer;
    int        status  = 0;
    bool       udp     = false;
    bool       parsed  = false;
    const qint64 deadline = nowMs() + timeoutMs;

    while (!parsed && nowMs() < deadline) {
        // 先把已有字节解析一遍
        int pos = 0;
        while (!parsed) {
            const int frameStart = pos;
            quint64   frameType  = 0;
            quint64   frameLen   = 0;
            if (!readVarint(buffer, pos, &frameType)) {
                pos = frameStart;
                break;
            }
            if (!readVarint(buffer, pos, &frameLen)) {
                pos = frameStart;
                break;
            }
            if (quint64(pos) + frameLen > quint64(buffer.size())) {
                pos = frameStart;
                break;
            }
            if (frameType == kHttp3FrameHeaders) {
                const QByteArray section = buffer.mid(pos, int(frameLen));
                if (parseAuthResponseSection(section, &status, &udp)) parsed = true;
            }
            pos += int(frameLen);
        }
        if (parsed) break;

        // 数据不足：继续驱动网络
        struct pollfd pfd{m_fd, POLLIN, 0};
        ::poll(&pfd, 1, 200);
        if (SSL_handle_events(m_ssl) != 1) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "QUIC event handling failed: %1").arg(sslErrorText());
            SSL_free(request);
            return false;
        }

        for (;;) {
            char      chunk[8192];
            const int n = SSL_read(request, chunk, sizeof(chunk));
            if (n > 0) {
                buffer.append(chunk, n);
                continue;
            }
            const int sslError = SSL_get_error(request, n);
            if (isStreamFinished(request, sslError)) break;
            if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE) break;
            break;
        }
    }

    SSL_free(request);
    m_streams.clear();

    if (!parsed) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::core", "No response to authentication (timeout or connection reset, port %1)")
                         .arg(m_peerPort);
        }
        return false;
    }
    if (status != kStatusAuthOk) {
        m_authRejected = true; // 密码错误等确定性问题，重试无意义
        if (error) {
            *error = QCoreApplication::translate("clashqt::core", "Authentication rejected (HTTP status %1; check the password)").arg(status);
        }
        return false;
    }
    m_serverUdp = udp;
    return true;
}

std::shared_ptr<Hysteria2Session> Hysteria2Session::create(const Hysteria2Config &config,
                                                           int timeoutMs, QString *error,
                                                           bool *retryable)
{
    if (retryable) *retryable = false;
    auto session = std::shared_ptr<Hysteria2Session>(new Hysteria2Session());

    QString failure;
    auto fail = [&](const QString &stage) -> std::shared_ptr<Hysteria2Session> {
        session->closeNow(QStringLiteral("%1：%2").arg(stage, failure));
        if (error) *error = session->lastError();
        return nullptr;
    };

    // 端口区间在 openSocket 内解析（需要对 m_server 先赋值）
    session->m_server    = config.server;
    session->m_peerPort  = config.port;
    session->m_portRange = config.portRange;
    session->m_password  = config.password;

    if (config.server.isEmpty()) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Node is missing server");
        return nullptr;
    }
    if (config.password.isEmpty()) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Node is missing password");
        return nullptr;
    }

    // 从这里开始都是网络环节，失败多半是瞬时丢包（节点/链路抖动），值得换端口重试
    if (retryable) *retryable = true;

    if (!session->openSocket(&failure)) return fail(QCoreApplication::translate("clashqt::core", "Socket setup"));
    if (!session->handshake(config, timeoutMs, &failure)) return fail(QCoreApplication::translate("clashqt::core", "Handshake"));
    if (!session->authenticate(timeoutMs, &failure)) {
        if (retryable) *retryable = !session->m_authRejected;
        return fail(QCoreApplication::translate("clashqt::core", "Authentication"));
    }

    session->m_lastError.clear();
    return session;
}

std::shared_ptr<Hysteria2Stream> Hysteria2Session::openTcp(const QString &addr,
                                                           int timeoutMs, QString *error)
{
    if (!m_open) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Session unavailable");
        return nullptr;
    }
    if (addr.isEmpty() || addr.size() > kMaxAddressLength) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Invalid target address");
        return nullptr;
    }

    // 请求帧：0x401 | 地址长度 | 地址 | 填充长度 | 填充
    QByteArray frame;
    putVarint(frame, kFrameTypeTcpRequest);
    putVarint(frame, quint64(addr.size()));
    frame.append(addr.toUtf8());
    const QByteArray padding = randomPadding(64, 512);
    putVarint(frame, quint64(padding.size()));
    frame.append(padding);

    QString  streamError;
    SSL     *stream = openQuicStream(false, frame, false, &streamError);
    if (!stream) {
        if (error) *error = streamError;
        return nullptr;
    }

    // 应答：状态(1) | 消息长度(varint) | 消息 | 填充长度(varint) | 填充
    QByteArray buffer;
    int        cursor = 0;
    bool       ok     = false;
    QString    message;
    const qint64 deadline = nowMs() + timeoutMs;

    auto tryParse = [&]() -> bool {
        int pos = 0;
        if (buffer.size() < 1) return false;
        const uchar status = uchar(buffer.at(0));
        ++pos;
        quint64 messageLength = 0;
        if (!readVarint(buffer, pos, &messageLength)) return false;
        if (quint64(pos) + messageLength > quint64(buffer.size())) return false;
        message = QString::fromUtf8(buffer.mid(pos, int(messageLength)));
        pos += int(messageLength);
        quint64 paddingLength = 0;
        if (!readVarint(buffer, pos, &paddingLength)) return false;
        if (quint64(pos) + paddingLength > quint64(buffer.size())) return false;
        pos += int(paddingLength);

        ok     = (status == 0);
        cursor = pos;
        return true;
    };

    while (!tryParse() && nowMs() < deadline) {
        struct pollfd pfd{m_fd, POLLIN, 0};
        ::poll(&pfd, 1, 200);
        if (SSL_handle_events(m_ssl) != 1) {
            SSL_free(stream);
            if (error) *error = QCoreApplication::translate("clashqt::core", "QUIC event handling failed: %1").arg(sslErrorText());
            return nullptr;
        }
        for (;;) {
            char      chunk[8192];
            const int n = SSL_read(stream, chunk, sizeof(chunk));
            if (n > 0) {
                buffer.append(chunk, n);
                continue;
            }
            break;
        }
    }

    if (buffer.isEmpty()) {
        SSL_free(stream);
        if (error) {
            *error = QCoreApplication::translate("clashqt::core", "Target %1 did not respond (timeout)").arg(addr);
        }
        return nullptr;
    }
    if (!ok) {
        SSL_free(stream);
        if (error) {
            *error = message.isEmpty()
                         ? QCoreApplication::translate("clashqt::core", "Connection to target %1 refused").arg(addr)
                         : QCoreApplication::translate("clashqt::core", "Connection to target %1 refused: %2").arg(addr, message);
        }
        return nullptr;
    }

    auto result = std::make_shared<Hysteria2Stream>(shared_from_this(), stream);
    // 应答之后的字节已属于隧道载荷（例如服务端与目标建连后立刻回送的数据）
    if (buffer.size() > cursor) result->prependPlaintext(buffer.mid(cursor));

    startEventLoop();
    return result;
}

void Hysteria2Session::startEventLoop()
{
    if (!m_open || m_notifier) return;

    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, [this]() {
        // 关掉通知，处理完再恢复，避免水平触发导致的忙循环
        m_notifier->setEnabled(false);
        pump();
        if (m_open && m_notifier) m_notifier->setEnabled(true);
    });

    // QUIC 的定时器（重传 / ACK 延迟）不依赖套接字可读，需要周期性推进
    m_timer = new QTimer(this);
    m_timer->setInterval(200);
    connect(m_timer, &QTimer::timeout, this, [this]() { pump(); });
    m_timer->start();
}

void Hysteria2Session::flushOutgoing()
{
    if (!m_ssl || !m_open) return;
    for (int i = 0; i < 16; ++i) {
        if (SSL_handle_events(m_ssl) != 1) {
            closeNow(QCoreApplication::translate("clashqt::core", "QUIC event handling failed: %1").arg(sslErrorText()));
            return;
        }
        struct pollfd pfd{m_fd, POLLIN, 0};
        if (::poll(&pfd, 1, 0) <= 0) break;
    }
    const QList<Hysteria2Stream *> streams = m_streams;
    for (Hysteria2Stream *stream : streams) {
        if (stream) stream->pullFromQuic();
    }
}

void Hysteria2Session::pump()
{
    if (!m_ssl || !m_open) return;
    drainIncomingStreams();
    if (SSL_handle_events(m_ssl) != 1) {
        closeNow(QCoreApplication::translate("clashqt::core", "QUIC event handling failed: %1").arg(sslErrorText()));
        return;
    }
    drainIncomingStreams();

    // 逐个流取出已到达的数据（回调中可能关闭并注销流，故先复制列表）
    const QList<Hysteria2Stream *> streams = m_streams;
    for (Hysteria2Stream *stream : streams) {
        if (stream) stream->pullFromQuic();
    }
}

void Hysteria2Session::drainIncomingStreams()
{
    // 服务端会自行开启控制流 / QPACK 流；只要它们不被拒绝即可。
    // 这里接收后立刻读空，避免接收窗口堆积。
    for (;;) {
        SSL *incoming = SSL_accept_stream(m_ssl, SSL_ACCEPT_STREAM_NO_BLOCK);
        if (!incoming) break;
        char      chunk[4096];
        for (;;) {
            const int n = SSL_read(incoming, chunk, sizeof(chunk));
            if (n > 0) continue;
            break;
        }
        SSL_free(incoming);
    }
}

void Hysteria2Session::detachStream(Hysteria2Stream *stream)
{
    m_streams.removeAll(stream);
}

void Hysteria2Session::closeNow(const QString &reason)
{
    if (!m_ssl && m_fd < 0) {
        m_open = false;
        return;
    }
    m_open = false;

    if (m_notifier) {
        m_notifier->setEnabled(false);
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }
    if (m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = nullptr;
    }

    if (m_ssl) {
        SSL_free(m_ssl);
        m_ssl = nullptr;
    }
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }

    if (!reason.isEmpty()) m_lastError = reason;

    const QList<Hysteria2Stream *> streams = m_streams;
    m_streams.clear();
    for (Hysteria2Stream *stream : streams) {
        if (stream) stream->markBroken(m_lastError);
    }
}

} // namespace core
