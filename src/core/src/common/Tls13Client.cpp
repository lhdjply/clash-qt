#include "common/Tls13Client.h"
#include <QCoreApplication>

#include "base/Common.h"
#include "common/JsonUtil.h"

#include <QDateTime>
#include <QEventLoop>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QTimer>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/params.h>
#include <openssl/rand.h>

// ML-KEM-768（FIPS 203，itzmeanjan/ml-kem，BSD-3-Clause），
// 供 X25519MLKEM768 混合分享使用
#include "ml_kem/ml_kem_768.hpp"

#include <array>
#include <span>

#include <algorithm>
#include <cstring>

namespace core {

namespace {

// ---------------------------------------------------------------------------
//  OpenSSL 基础封装
// ---------------------------------------------------------------------------

QByteArray secureRandom(int len)
{
    QByteArray out(len, Qt::Uninitialized);
    RAND_bytes(reinterpret_cast<unsigned char *>(out.data()), len);
    return out;
}

// HKDF-Extract(salt, ikm) -> PRK；摘要在 TLS 1.3 密钥调度中按协商套件选择
QByteArray hkdfExtract(const QByteArray &salt, const QByteArray &ikm, const char *digest)
{
    QByteArray  prk(64, 0);
    size_t      prkLen = 64;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr);
    if (!ctx) return {};
    EVP_PKEY_derive_init(ctx);
    OSSL_PARAM params[5];
    size_t     mode = EVP_PKEY_HKDEF_MODE_EXTRACT_ONLY;
    QByteArray digestStr(digest);
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST,
                                                 digestStr.data(), 0);
    params[1] = OSSL_PARAM_construct_size_t(OSSL_KDF_PARAM_MODE, &mode);
    params[2] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT,
                                                  const_cast<char *>(salt.constData()),
                                                  size_t(salt.size()));
    params[3] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY,
                                                  const_cast<char *>(ikm.constData()),
                                                  size_t(ikm.size()));
    params[4] = OSSL_PARAM_construct_end();
    EVP_PKEY_CTX_set_params(ctx, params);
    if (EVP_PKEY_derive(ctx, reinterpret_cast<unsigned char *>(prk.data()), &prkLen) != 1) {
        EVP_PKEY_CTX_free(ctx);
        return {};
    }
    prk.resize(int(prkLen));
    EVP_PKEY_CTX_free(ctx);
    return prk;
}

// HKDF-Expand(prk, info, len)；摘要按 PRK 长度选择（32B->SHA256，48B->SHA384）
QByteArray hkdfExpand(const QByteArray &prk, const QByteArray &info, int len)
{
    const char *digestName = prk.size() >= 48 ? "SHA384" : "SHA256";
    QByteArray  out(len, 0);
    size_t      outLen = size_t(len);
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr);
    if (!ctx) return {};
    EVP_PKEY_derive_init(ctx);
    OSSL_PARAM params[4];
    size_t     mode = EVP_PKEY_HKDEF_MODE_EXPAND_ONLY;
    QByteArray digestStr(digestName);
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST,
                                                 digestStr.data(), 0);
    params[1] = OSSL_PARAM_construct_size_t(OSSL_KDF_PARAM_MODE, &mode);
    params[2] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY,
                                                  const_cast<char *>(prk.constData()),
                                                  size_t(prk.size()));
    params[3] = OSSL_PARAM_construct_end();
    EVP_PKEY_CTX_set_params(ctx, params);
    EVP_PKEY_CTX_add1_hkdf_info(
        ctx, reinterpret_cast<const unsigned char *>(info.constData()), info.size());
    if (EVP_PKEY_derive(ctx, reinterpret_cast<unsigned char *>(out.data()), &outLen) != 1) {
        EVP_PKEY_CTX_free(ctx);
        return {};
    }
    EVP_PKEY_CTX_free(ctx);
    return out;
}

QByteArray digestOf(const char *algo, const QByteArray &data)
{
    size_t     len = 0;
    QByteArray out(EVP_MAX_MD_SIZE, 0);
    EVP_Q_digest(nullptr, algo, nullptr, data.constData(), size_t(data.size()),
                 reinterpret_cast<unsigned char *>(out.data()), &len);
    out.resize(int(len));
    return out;
}

// RFC 8446 §7.1：HKDF-ExpandLabel(secret, label, context, length)
QByteArray expandLabel(const QByteArray &secret, const char *label,
                       const QByteArray &context, int length)
{
    QByteArray info;
    info.append(char((length >> 8) & 0xFF));
    info.append(char(length & 0xFF));
    const QByteArray fullLabel = QByteArray("tls13 ") + label;
    info.append(char(fullLabel.size()));
    info.append(fullLabel);
    // RFC 8446 §7.1：context 为 opaque<0..255>，长度前缀 1 字节
    // （已用 RFC 8448 官方测试向量核对）
    info.append(char(context.size() & 0xFF));
    info.append(context);
    return hkdfExpand(secret, info, length);
}

QByteArray hmacOneShot(const char *algo, const QByteArray &key, const QByteArray &data)
{
    EVP_MAC     *mac = EVP_MAC_fetch(nullptr, "HMAC", nullptr);
    EVP_MAC_CTX *ctx = EVP_MAC_CTX_new(mac);
    QByteArray   out(64, 0);
    size_t       outLen = 0;
    OSSL_PARAM   params[2];
    QByteArray   algoStr(algo);
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_MAC_PARAM_DIGEST, algoStr.data(), 0);
    params[1] = OSSL_PARAM_construct_end();
    EVP_MAC_init(ctx, reinterpret_cast<const unsigned char *>(key.constData()),
                 size_t(key.size()), params);
    EVP_MAC_update(ctx, reinterpret_cast<const unsigned char *>(data.constData()),
                   size_t(data.size()));
    EVP_MAC_final(ctx, reinterpret_cast<unsigned char *>(out.data()), &outLen, out.size());
    out.resize(int(outLen));
    EVP_MAC_CTX_free(ctx);
    EVP_MAC_free(mac);
    return out;
}

bool x25519Keygen(QByteArray *priv, QByteArray *pub)
{
    EVP_PKEY *pkey = EVP_PKEY_Q_keygen(nullptr, nullptr, "X25519");
    if (!pkey) return false;
    size_t len = 32;
    priv->resize(32);
    pub->resize(32);
    EVP_PKEY_get_raw_private_key(pkey, reinterpret_cast<unsigned char *>(priv->data()), &len);
    len = 32;
    EVP_PKEY_get_raw_public_key(pkey, reinterpret_cast<unsigned char *>(pub->data()), &len);
    EVP_PKEY_free(pkey);
    return true;
}

QByteArray x25519Ecdh(const QByteArray &ourPriv, const QByteArray &peerPub)
{
    EVP_PKEY *ours = EVP_PKEY_new_raw_private_key(
        EVP_PKEY_X25519, nullptr,
        reinterpret_cast<const unsigned char *>(ourPriv.constData()), 32);
    EVP_PKEY *peer = EVP_PKEY_new_raw_public_key(
        EVP_PKEY_X25519, nullptr,
        reinterpret_cast<const unsigned char *>(peerPub.constData()), 32);
    QByteArray    secret(32, 0);
    size_t        len = 32;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(ours, nullptr);
    EVP_PKEY_derive_init(ctx);
    EVP_PKEY_derive_set_peer(ctx, peer);
    EVP_PKEY_derive(ctx, reinterpret_cast<unsigned char *>(secret.data()), &len);
    secret.resize(int(len));
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(ours);
    EVP_PKEY_free(peer);
    return secret;
}

// AEAD 加密：输出 密文||tag（TLS 1.3 记录层格式）
QByteArray aeadSeal(const char *cipherNameStr, const QByteArray &key, const QByteArray &iv,
                    const QByteArray &aad, const QByteArray &plaintext)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    QByteArray      out(plaintext.size() + 16, 0);
    int             len = 0, total = 0;
    EVP_EncryptInit_ex(ctx, EVP_get_cipherbyname(cipherNameStr), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, iv.size(), nullptr);
    EVP_EncryptInit_ex(ctx, nullptr, nullptr,
                       reinterpret_cast<const unsigned char *>(key.constData()),
                       reinterpret_cast<const unsigned char *>(iv.constData()));
    if (!aad.isEmpty()) {
        EVP_EncryptUpdate(ctx, nullptr, &len,
                          reinterpret_cast<const unsigned char *>(aad.constData()),
                          aad.size());
    }
    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(out.data()), &len,
                      reinterpret_cast<const unsigned char *>(plaintext.constData()),
                      plaintext.size());
    total = len;
    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(out.data()) + total, &len);
    total += len;
    QByteArray tag(16, 0);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag.data());
    out.resize(total + 16);
    out.replace(total, 16, tag);
    EVP_CIPHER_CTX_free(ctx);
    return out;
}

bool aeadOpen(const char *cipherNameStr, const QByteArray &key, const QByteArray &iv,
              const QByteArray &aad, const QByteArray &ctAndTag, QByteArray *plaintext)
{
    if (ctAndTag.size() < 16) return false;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_get_cipherbyname(cipherNameStr), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, iv.size(), nullptr);
    EVP_DecryptInit_ex(ctx, nullptr, nullptr,
                       reinterpret_cast<const unsigned char *>(key.constData()),
                       reinterpret_cast<const unsigned char *>(iv.constData()));
    int len = 0;
    if (!aad.isEmpty()) {
        EVP_DecryptUpdate(ctx, nullptr, &len,
                          reinterpret_cast<const unsigned char *>(aad.constData()),
                          aad.size());
    }
    const int      ctLen = ctAndTag.size() - 16;
    const QByteArray tag = ctAndTag.right(16);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, 16,
                        const_cast<char *>(tag.constData()));
    plaintext->resize(ctLen);
    bool ok = EVP_DecryptUpdate(ctx,
                                reinterpret_cast<unsigned char *>(plaintext->data()), &len,
                                reinterpret_cast<const unsigned char *>(ctAndTag.constData()),
                                ctLen) == 1;
    if (ok) {
        int flen = 0;
        ok = EVP_DecryptFinal_ex(ctx,
                                 reinterpret_cast<unsigned char *>(plaintext->data()) + len,
                                 &flen) == 1;
        plaintext->resize(len + flen);
    }
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

// 记录层 nonce：static_iv XOR 大端 64 位序号（iv 12B，序号占末 8B）
QByteArray recordNonce(const QByteArray &staticIv, quint64 seq)
{
    QByteArray nonce = staticIv;
    for (int i = 0; i < 8; ++i) {
        nonce[4 + i] = char(nonce.at(4 + i) ^ char((seq >> (56 - 8 * i)) & 0xFF));
    }
    return nonce;
}

QByteArray be16(int v)
{
    QByteArray out(2, 0);
    out[0] = char((v >> 8) & 0xFF);
    out[1] = char(v & 0xFF);
    return out;
}

QByteArray be24(int v)
{
    QByteArray out(3, 0);
    out[0] = char((v >> 16) & 0xFF);
    out[1] = char((v >> 8) & 0xFF);
    out[2] = char(v & 0xFF);
    return out;
}

int readBe16(const QByteArray &d, int off)
{
    if (off + 2 > d.size()) return -1;
    return (uchar(d.at(off)) << 8) | uchar(d.at(off + 1));
}

int readBe24(const QByteArray &d, int off)
{
    if (off + 3 > d.size()) return -1;
    return (uchar(d.at(off)) << 16) | (uchar(d.at(off + 1)) << 8) | uchar(d.at(off + 2));
}

// ---------------------------------------------------------------------------
//  最小 X.509 解析：提取叶子证书的 ed25519 公钥与证书签名字段（REALITY 验证）
//  对应参照实现 realityVerifier：certs[0].PublicKey(ed25519) 与 .Signature
// ---------------------------------------------------------------------------

struct DerTlv {
    uchar      tag = 0;
    QByteArray content;
    int        next = 0;
};

bool derRead(const QByteArray &d, int offset, DerTlv *out)
{
    if (offset + 2 > d.size()) return false;
    const uchar tag = uchar(d.at(offset));
    int         len = uchar(d.at(offset + 1));
    int         pos = offset + 2;
    if (len & 0x80) {
        const int numBytes = len & 0x7F;
        if (numBytes > 3 || pos + numBytes > d.size()) return false;
        len = 0;
        for (int i = 0; i < numBytes; ++i) len = (len << 8) | uchar(d.at(pos + i));
        pos += numBytes;
    }
    if (pos + len > d.size()) return false;
    out->tag     = tag;
    out->content = d.mid(pos, len);
    out->next    = pos + len;
    return true;
}

struct RealityCert {
    QByteArray ed25519Pub;
    QByteArray signature;
};

bool parseLeafCertForReality(const QByteArray &der, RealityCert *out)
{
    // Certificate ::= SEQ { tbsCertificate SEQ, signatureAlgorithm SEQ,
    //                       signatureValue BIT STRING }
    DerTlv cert;
    if (!derRead(der, 0, &cert) || cert.tag != 0x30) return false;

    // 证书签名值（BIT STRING，首字节为 unused-bits，其后为 64 字节 ed25519 签名）
    DerTlv    tbsTlv;
    if (!derRead(cert.content, 0, &tbsTlv) || tbsTlv.tag != 0x30) return false;
    const QByteArray tbs = tbsTlv.content;

    QByteArray signature;
    {
        DerTlv item;
        int    pos = tbsTlv.next; // 相对 cert.content 的偏移
        while (derRead(cert.content, pos, &item)) {
            if (item.tag == 0x03) signature = item.content.mid(1);
            pos = item.next;
        }
    }
    if (signature.size() != 64) return false;

    // tbs 内第 5 个 SEQUENCE = SubjectPublicKeyInfo
    // （sigAlg / issuer / validity / subject / spki；version 为 context 标签、
    //   serial 为 INTEGER，均不计数）
    QByteArray spki;
    {
        int    count = 0;
        int    cur   = 0;
        DerTlv e;
        while (cur < tbs.size()) {
            if (!derRead(tbs, cur, &e)) break;
            cur = e.next;
            if (e.tag == 0x30 && ++count == 5) {
                spki = e.content;
                break;
            }
        }
    }
    if (spki.isEmpty()) return false;

    // spki ::= SEQ { AlgorithmIdentifier SEQ { OID, ... }, BIT STRING key }
    DerTlv alg, key;
    if (!derRead(spki, 0, &alg) || alg.tag != 0x30) return false;
    if (!derRead(spki, alg.next, &key) || key.tag != 0x03) return false;

    DerTlv oid;
    if (!derRead(alg.content, 0, &oid) || oid.tag != 0x06) return false;
    static const QByteArray kEd25519Oid = QByteArray::fromHex("2B6570"); // 1.3.101.112
    if (!oid.content.startsWith(kEd25519Oid)) return false;

    out->ed25519Pub = key.content.mid(1); // 去掉 unused-bits 字节
    out->signature  = signature;
    return out->ed25519Pub.size() == 32;
}

const char *aeadCipherName(RealityTlsStream::Cipher cipher)
{
    using C = RealityTlsStream::Cipher;
    switch (cipher) {
    case C::Aes128GcmSha256: return "AES-128-GCM";
    case C::Aes256GcmSha384: return "AES-256-GCM";
    case C::ChaCha20Poly1305Sha256: return "ChaCha20-Poly1305";
    }
    return "AES-128-GCM";
}

const char *hashName(int hashLen) { return hashLen == 48 ? "SHA384" : "SHA256"; }

} // namespace

// ---------------------------------------------------------------------------
//  生命周期
// ---------------------------------------------------------------------------

RealityTlsStream::RealityTlsStream(QTcpSocket *socket, QObject *parent)
    : OutboundStream(parent), m_socket(socket)
{
    QObject::connect(m_socket, &QAbstractSocket::readyRead, this, [this]() {
        if (m_rawMode || !m_established) {
            emit readyToRead();
            return;
        }
        decryptAvailable();
        if (!m_plaintext.isEmpty()) emit readyToRead();
    });
    QObject::connect(m_socket, &QAbstractSocket::disconnected, this, [this]() {
        m_closed = true;
        emit streamClosed();
    });
    QObject::connect(m_socket, &QAbstractSocket::errorOccurred, this,
                     [this](QAbstractSocket::SocketError) {
                         if (!m_established) return;
                         m_closed = true;
                         emit streamClosed();
                     });
}

RealityTlsStream::~RealityTlsStream()
{
    if (m_socket) {
        m_socket->abort();
        m_socket->deleteLater();
    }
}

std::shared_ptr<RealityTlsStream> RealityTlsStream::connect(const RealityConfig &config,
                                                            int timeoutMs, QString *error)
{
    // ---- TCP 建连 ---------------------------------------------------------
    auto socket = std::make_unique<QTcpSocket>();
    {
        QEventLoop loop;
        QTimer     timer;
        timer.setSingleShot(true);
        QObject::connect(socket.get(), &QAbstractSocket::connected, &loop,
                         &QEventLoop::quit);
        QObject::connect(socket.get(), &QAbstractSocket::errorOccurred, &loop,
                         &QEventLoop::quit);
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

        socket->connectToHost(config.server, config.port);
        timer.start(std::max(1000, timeoutMs));
        loop.exec();

        if (socket->state() != QAbstractSocket::ConnectedState) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "REALITY connection to %1:%2 failed: %3")
                             .arg(config.server)
                             .arg(config.port)
                             .arg(socket->errorString());
            }
            return nullptr;
        }
    }

    auto stream = std::shared_ptr<RealityTlsStream>(
        new RealityTlsStream(socket.release(), nullptr));
    if (!stream->handshake(config, timeoutMs, error)) return nullptr;
    return stream;
}

// ---------------------------------------------------------------------------
//  握手
// ---------------------------------------------------------------------------

bool RealityTlsStream::handshake(const RealityConfig &config, int timeoutMs, QString *error)
{
    // ---- 1. 临时 X25519 密钥对 + REALITY 静态 ECDH -------------------------
    // authKey = HKDF-SHA256(IKM=ECDH(临时私钥, 服务器静态公钥),
    //                       salt=ClientHello.random[:20], info="REALITY")
    // 对应参照实现：ecdheKey.ECDH(realityConfig.PublicKey) + hkdf(...)
    if (!x25519Keygen(&m_km.ourPriv, &m_km.ourPub)) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "X25519 key generation failed");
        return false;
    }

    // ML-KEM-768 密钥对 + 混合分享尾部的独立 x25519 密钥对。
    // 新版 REALITY 服务器要求 ClientHello 携带 X25519MLKEM768 分享
    // （见 XTLS/reality handshake_server_tls13.go：无 MLKEM 分享直接回退）。
    {
        // keygen 由种子 (d, z) 确定性派生，随机性由本端控制
        std::array<uint8_t, ml_kem_768::SEED_D_BYTE_LEN> d{};
        std::array<uint8_t, ml_kem_768::SEED_Z_BYTE_LEN> z{};
        RAND_bytes(d.data(), int(d.size()));
        RAND_bytes(z.data(), int(z.size()));

        std::array<uint8_t, ml_kem_768::PKEY_BYTE_LEN> pubkey{};
        std::array<uint8_t, ml_kem_768::SKEY_BYTE_LEN> seckey{};
        ml_kem_768::keygen(d, z, pubkey, seckey);
        m_mlkemEk.assign(pubkey.begin(), pubkey.end());
        m_mlkemSk.assign(seckey.begin(), seckey.end());
    }
    if (!x25519Keygen(&m_mlkemXPriv, &m_mlkemXPub)) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "X25519 key generation failed");
        return false;
    }

    const QByteArray ecdhStatic = x25519Ecdh(m_km.ourPriv, config.publicKey);
    if (ecdhStatic.size() != 32) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "REALITY static ECDH failed");
        return false;
    }

    // ---- 2. ClientHello（session_id 区域先全零） ---------------------------
    m_km.clientRandom = secureRandom(32);
    if (!buildClientHello(config, error)) {
        if (error && error->isEmpty()) *error = QCoreApplication::translate("clashqt::core", "Failed to build ClientHello");
        return false;
    }

    if (config.reality) {
        m_km.authKey = hkdfExtract(m_km.clientRandom.left(20), ecdhStatic, "SHA256");
        m_km.authKey = hkdfExpand(m_km.authKey, QByteArray("REALITY"), 32);
    }

    // session_id 明文（16B）：[1,8,2] + 时间戳低 5 字节(大端) + shortId(8B)
    const quint64 now  = quint64(QDateTime::currentSecsSinceEpoch());
    QByteArray    sid(16, 0);
    sid[0] = char(0x01);
    sid[1] = char(0x08);
    sid[2] = char(0x02);
    for (int i = 0; i < 5; ++i) {
        sid[3 + i] = char((now >> (32 - 8 * i)) & 0xFF);
    }
    // 参照实现：copy(hello.SessionId[8:], ShortID[:]) —— shortId 从偏移 8 起
    // 原样填入，不足 8 字节的部分保持零（右补零，而非左对齐）
    const QByteArray shortId = config.shortId.left(8).leftJustified(8, '\0');
    sid.replace(8, 8, shortId);

    if (config.reality) {
        // AES-256-GCM(key=authKey, nonce=random[20:32], pt=sid[:16], aad=rawHello)
        // 结果 32B（密文+tag）写回 session_id —— 对应参照实现的
        // aeadCipher.Seal(hello.SessionId[:0], hello.Random[20:], hello.SessionId[:16], hello.Raw)
        const QByteArray sealed = aeadSeal("AES-256-GCM", m_km.authKey,
                                           m_km.clientRandom.mid(20), m_km.rawHello,
                                           sid.left(16));
        if (sealed.size() != 32) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to generate REALITY authentication data");
            return false;
        }
        m_km.rawHello.replace(39, 32, sealed);
    }
    m_transcript = m_km.rawHello;

    // ---- 3. 发送 ClientHello ----------------------------------------------
    if (!sendRecord(0x16, m_km.rawHello, error)) return false;

    // ---- 4. ServerHello ----------------------------------------------------
    uchar        recordType = 0;
    QByteArray   recordPayload;
    QByteArray   recordHeader;
    if (!recvRecord(&recordType, &recordPayload, &recordHeader, timeoutMs, error)) {
        return false;
    }
    if (recordType != 0x16 || recordPayload.isEmpty() || uchar(recordPayload.at(0)) != 0x02) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::core", "REALITY received no ServerHello (record type=%1 length=%2 content=%3)")
                         .arg(recordType)
                         .arg(recordPayload.size())
                         .arg(QString::fromLatin1(recordPayload.left(24).toHex()));
        }
        return false;
    }
    m_transcript.append(recordPayload);

    const QByteArray &sh    = recordPayload;
    if (sh.size() < 43) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Truncated ServerHello");
        return false;
    }
    const int sidLen = uchar(sh.at(38));
    int       pos    = 39 + sidLen;
    const quint16 suite = quint16(readBe16(sh, pos));
    pos += 2 + 1;
    const int extTotal = readBe16(sh, pos);
    pos += 2;
    const int extEnd = pos + extTotal;

    // 服务端 key_share：X25519MLKEM768 时为 密文(1088) || x25519 公钥(32)；
    // 纯 x25519 时为 32 字节公钥
    enum class ServerGroup { X25519, X25519Mlkem768 };
    ServerGroup serverGroup = ServerGroup::X25519;
    QByteArray  serverX25519Pub;
    while (pos + 4 <= extEnd && pos + 4 <= sh.size()) {
        const quint16 extType = quint16(readBe16(sh, pos));
        const int     extLen  = readBe16(sh, pos + 2);
        pos += 4;
        if (extType == 0x0033 && extLen >= 4) { // key_share
            const quint16 group  = quint16(readBe16(sh, pos));
            const int     keyLen = readBe16(sh, pos + 2);
            const QByteArray data = sh.mid(pos + 4, keyLen);
            if (group == 0x001D && keyLen == 32) {
                serverGroup     = ServerGroup::X25519;
                serverX25519Pub = data;
            } else if (group == 0x11EC && keyLen == 1088 + 32) {
                serverGroup     = ServerGroup::X25519Mlkem768;
                m_mlkemCt       = data.left(1088);
                serverX25519Pub = data.right(32);
            }
        }
        pos += extLen;
    }
    if (serverX25519Pub.size() != 32) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "ServerHello contains no usable key_share");
        return false;
    }

    Logger::instance().debug(
        LogCategory::Adapter,
        QCoreApplication::translate("clashqt::core", "TLS negotiated: cipher=0x%1 key-exchange group=%2")
            .arg(suite, 4, 16, QLatin1Char('0'))
            .arg(serverGroup == ServerGroup::X25519Mlkem768
                     ? QStringLiteral("X25519MLKEM768")
                     : QStringLiteral("x25519")));

    switch (suite) {
    case 0x1301: m_cipher = Cipher::Aes128GcmSha256; m_hashLen = 32; m_keyLen = 16; break;
    case 0x1302: m_cipher = Cipher::Aes256GcmSha384; m_hashLen = 48; m_keyLen = 32; break;
    case 0x1303: m_cipher = Cipher::ChaCha20Poly1305Sha256; m_hashLen = 32; m_keyLen = 32; break;
    default:
        if (error) {
            *error = QCoreApplication::translate("clashqt::core", "The REALITY server negotiated an unsupported cipher 0x%1")
                         .arg(suite, 4, 16, QLatin1Char('0'));
        }
        return false;
    }

    // ---- 5. 密钥调度（RFC 8446 §7.1） --------------------------------------
    const char *hash  = hashName(m_hashLen);
    const QByteArray zeros(m_hashLen, 0);
    const QByteArray hashEmpty = digestOf(hash, QByteArray());

    const QByteArray early        = hkdfExtract(zeros, zeros, hash);
    const QByteArray earlyDerived = expandLabel(early, "derived", hashEmpty, m_hashLen);

    // TLS 共享密钥：MLKEM768 组 = ML-KEM 解封装(32B) || x25519(32B)；纯 x25519 组 = 32B
    QByteArray shared;
    if (serverGroup == ServerGroup::X25519Mlkem768) {
        std::array<uint8_t, ml_kem_768::SHARED_SECRET_BYTE_LEN> mlkemShared{};
        const auto sk = std::span<const uint8_t, ml_kem_768::SKEY_BYTE_LEN>(
            reinterpret_cast<const uint8_t *>(m_mlkemSk.constData()),
            ml_kem_768::SKEY_BYTE_LEN);
        const auto ct = std::span<const uint8_t, ml_kem_768::CIPHER_TEXT_BYTE_LEN>(
            reinterpret_cast<const uint8_t *>(m_mlkemCt.constData()),
            ml_kem_768::CIPHER_TEXT_BYTE_LEN);
        ml_kem_768::decapsulate(sk, ct, mlkemShared);
        // shared = ML-KEM 解封装(32B) || x25519(32B)。
        // x25519 半段必须用「混合分享尾部」的 x25519 密钥（服务端按此计算），
        // 而不是独立 x25519 分享的密钥。
        shared = QByteArray(reinterpret_cast<const char *>(mlkemShared.data()),
                            int(mlkemShared.size()))
                 + x25519Ecdh(m_mlkemXPriv, serverX25519Pub);
    } else {
        shared = x25519Ecdh(m_km.ourPriv, serverX25519Pub);
    }
    const QByteArray hsSecret = hkdfExtract(earlyDerived, shared, hash);

    const QByteArray thSh = digestOf(hash, m_transcript);
    // RFC 8446 §7.3：key/iv 从各自的 traffic secret 再派生
    const QByteArray cHsSecret = expandLabel(hsSecret, "c hs traffic", thSh, m_hashLen);
    const QByteArray sHsSecret = expandLabel(hsSecret, "s hs traffic", thSh, m_hashLen);
    m_clientHsSecret = cHsSecret;
    m_serverHsSecret = sHsSecret;
    m_clientHsKey = expandLabel(cHsSecret, "key", QByteArray(), m_keyLen);
    m_clientHsIv  = expandLabel(cHsSecret, "iv", QByteArray(), 12);
    m_serverHsKey = expandLabel(sHsSecret, "key", QByteArray(), m_keyLen);
    m_serverHsIv  = expandLabel(sHsSecret, "iv", QByteArray(), 12);
    m_clientSeq   = 0;
    m_serverSeq   = 0;

    // ---- 6. 解密握手消息：EE / Certificate / CertificateVerify / Finished --
    const char *aead = aeadCipherName(m_cipher);
    bool        sawFinished = false;
    QByteArray  serverFinishedData;   // 完整 Finished 握手消息
    QByteArray  serverFinishedVerify; // 其中的 verify_data
    RealityCert realityCert;
    bool        gotCert       = false; // 是否收到 Certificate 消息
    bool        realityCertOk = false; // 证书是否解析为 REALITY 的 ed25519 形态

    while (!sawFinished) {
        if (!recvRecord(&recordType, &recordPayload, &recordHeader, timeoutMs, error)) {
            return false;
        }
        if (recordType == 0x14) {
            // TLS 1.3 中间件兼容用 change_cipher_spec（内容恒为 0x01），跳过
            continue;
        }
        if (recordType != 0x16 && recordType != 0x17) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "Unexpected record type %1 during the REALITY handshake").arg(recordType);
            }
            return false;
        }
        QByteArray inner;
        const QByteArray nonce = recordNonce(m_serverHsIv, m_serverSeq++);
        if (!aeadOpen(aead, m_serverHsKey, nonce, recordHeader, recordPayload, &inner)) {
            Logger::instance().warn(
                LogCategory::Adapter,
                QCoreApplication::translate("clashqt::core", "Failed to decrypt a REALITY handshake record (type=%1 length=%2 seq=%3)")
                    .arg(recordType)
                    .arg(recordPayload.size())
                    .arg(m_serverSeq - 1));
            if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to decrypt a REALITY handshake record");
            return false;
        }
        // 去尾部零填充与内容类型字节
        int end = inner.size() - 1;
        while (end >= 0 && inner.at(end) == 0) --end;
        if (end < 0) continue;
        m_hsPending.append(inner.left(end));

        while (m_hsPending.size() >= 4) {
            const uchar msgType = uchar(m_hsPending.at(0));
            const int    msgLen = readBe24(m_hsPending, 1);
            if (msgLen < 0 || m_hsPending.size() < 4 + msgLen) break;
            const QByteArray message = m_hsPending.left(4 + msgLen);
            m_hsPending.remove(0, 4 + msgLen);

            if (msgType == 0x0B) { // Certificate
                // body = context(1) + listLen(3) + [certLen(3) + der + extLen(2)]*
                const QByteArray &body = message.mid(4);
                if (body.size() >= 7) {
                    const int listLen = readBe24(body, 1);
                    int p2 = 4;
                    const int listEnd = int(std::min<qsizetype>(4 + listLen, body.size()));
                    m_peerCerts.clear();
                    while (p2 + 3 <= listEnd) {
                        const int certLen = readBe24(body, p2);
                        p2 += 3;
                        if (certLen <= 0 || p2 + certLen > listEnd) break;
                        m_peerCerts.append(body.mid(p2, certLen));
                        p2 += certLen + 2; // 跳过每个条目的 extensions 长度
                    }
                    if (!m_peerCerts.isEmpty()) {
                        gotCert       = true; // 证书消息已收到（普通 TLS 校验用）
                        realityCertOk = parseLeafCertForReality(m_peerCerts.first(),
                                                                &realityCert);
                    }
                }
            } else if (msgType == 0x14) { // Finished
                // 保留完整握手消息（type + 长度 + verify_data）用于转录
                serverFinishedData = message;
                serverFinishedVerify = message.mid(4);
                sawFinished        = true;
            }
            if (!sawFinished) m_transcript.append(message); // Finished 之后单独追加
        }
    }

    // ---- REALITY 认证：HMAC-SHA512(authKey, ed25519公钥) == 证书签名字段 ----
    if (!config.reality) {
        // 普通 TLS：标准证书链校验（系统根证书）+ 主机名匹配
        if (!gotCert) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "The server returned no certificate");
            return false;
        }
        if (!config.skipVerify) {
            const unsigned char *p = nullptr;
            X509 *leaf = nullptr;
            {
                const char *der = m_peerCerts.first().constData();
                p = reinterpret_cast<const unsigned char *>(der);
                leaf = d2i_X509(nullptr, &p, long(m_peerCerts.first().size()));
            }
            X509_STORE *store = X509_STORE_new();
            X509_STORE_set_default_paths(store);
            STACK_OF(X509) *untrusted = sk_X509_new_null();
            for (int i = 1; i < m_peerCerts.size(); ++i) {
                const char *der2 = m_peerCerts.at(i).constData();
                const unsigned char *q = reinterpret_cast<const unsigned char *>(der2);
                X509 *mid = d2i_X509(nullptr, &q, long(m_peerCerts.at(i).size()));
                if (mid) { sk_X509_push(untrusted, mid); }
            }
            X509_STORE_CTX *ctx = X509_STORE_CTX_new();
            X509_STORE_CTX_init(ctx, store, leaf, untrusted);
            const int ok = X509_verify_cert(ctx);
            const int  hostOk = X509_check_host(leaf, config.serverName.toUtf8().constData(),
                                                config.serverName.toUtf8().size(), 0, nullptr);
            X509_STORE_CTX_free(ctx);
            sk_X509_pop_free(untrusted, X509_free);
            X509_STORE_free(store);
            X509_free(leaf);
            if (ok != 1 || hostOk != 1) {
                if (error) {
                    *error = QCoreApplication::translate("clashqt::core", "TLS certificate verification failed (chain=%1 hostname=%2)")
                                 .arg(ok == 1 ? QCoreApplication::translate("clashqt::core", "Pass")
                                              : QCoreApplication::translate("clashqt::core", "Fail"),
                                      hostOk == 1 ? QCoreApplication::translate("clashqt::core", "Match")
                                                  : QCoreApplication::translate("clashqt::core", "No match"));
                }
                return false;
            }
        } else {
            Logger::instance().warn(LogCategory::Adapter,
                QCoreApplication::translate("clashqt::core", "Node %1 skips TLS certificate verification")
                    .arg(config.serverName));
        }
    } else {
        // REALITY 模式：服务端证书的 ed25519 公钥经 HMAC-SHA512 认证
        const QByteArray expect =
            hmacOneShot("SHA512", m_km.authKey, realityCert.ed25519Pub);
        if (!gotCert || !realityCertOk || expect != realityCert.signature) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "REALITY authentication failed (the peer is the borrowed site itself rather than a REALITY server, or public-key / short-id is misconfigured)");
            }
            return false;
        }
    }

    // 校验服务器 Finished：verify = HMAC(fk_s_hs, TH(CH..CertVerify))
    {
        const QByteArray thCv = digestOf(hash, m_transcript);
        const QByteArray fk =
            expandLabel(m_serverHsSecret, "finished", QByteArray(), m_hashLen);
        if (hmacOneShot(hash, fk, thCv) != serverFinishedVerify) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Server Finished verification failed");
            return false;
        }
    }

    // ---- 7. 客户端 Finished（握手密钥加密，外层记录类型 22） ----------------
    QByteArray thSf; // TH(CH..server Finished)
    {
        m_transcript.append(serverFinishedData);
        // TH(ClientHello..server Finished)：客户端 Finished 与应用流量密钥共用
        thSf = digestOf(hash, m_transcript);
        const QByteArray fk =
            expandLabel(m_clientHsSecret, "finished", QByteArray(), m_hashLen);
        const QByteArray cVerify = hmacOneShot(hash, fk, thSf);

        QByteArray finishedMsg;
        finishedMsg.append(char(0x14));
        finishedMsg.append(be24(cVerify.size()));
        finishedMsg.append(cVerify);
        m_transcript.append(finishedMsg);

        QByteArray inner = finishedMsg;
        inner.append(char(0x16));
        // RFC 8446：所有加密记录外层类型恒为 23（application_data）
        const QByteArray nonce  = recordNonce(m_clientHsIv, m_clientSeq++);
        const QByteArray header = QByteArray(1, char(0x17)) + QByteArray("\x03\x03") +
                                  be16(inner.size() + 16);
        if (!sendEncryptedRecord(0x17, aeadSeal(aead, m_clientHsKey, nonce, header, inner),
                                 error)) {
            return false;
        }
    }

    // ---- 8. 应用流量密钥 ----------------------------------------------------
    {
        const QByteArray hsDerived =
            expandLabel(hsSecret, "derived", hashEmpty, m_hashLen);
        const QByteArray master = hkdfExtract(hsDerived, zeros, hash);
        // RFC 8446 §7.1：应用流量密钥用 TH(CH..server Finished)
        const QByteArray &thCf = thSf;
        const QByteArray cApSecret = expandLabel(master, "c ap traffic", thCf, m_hashLen);
        const QByteArray sApSecret = expandLabel(master, "s ap traffic", thCf, m_hashLen);
        m_clientApKey = expandLabel(cApSecret, "key", QByteArray(), m_keyLen);
        m_clientApIv  = expandLabel(cApSecret, "iv", QByteArray(), 12);
        m_serverApKey = expandLabel(sApSecret, "key", QByteArray(), m_keyLen);
        m_serverApIv  = expandLabel(sApSecret, "iv", QByteArray(), 12);
        m_serverSeq   = 0; // 服务端应用阶段序号重新计数
        m_clientSeq   = 0;
    }

    m_established = true;
    return true;
}

bool RealityTlsStream::buildClientHello(const RealityConfig &config, QString *error)
{
    Q_UNUSED(error);
    // Safari 风格的 TLS 1.3 ClientHello；指纹仅用于伪装，REALITY 服务端
    // 只解析 SNI / session_id / key_share
    QByteArray ext;

    { // server_name
        const QByteArray sni = config.serverName.toUtf8();
        QByteArray inner;
        inner.append(be16(sni.size() + 3));
        inner.append(char(0x00));
        inner.append(be16(sni.size()));
        inner.append(sni);
        ext.append(be16(0x0000)).append(be16(inner.size())).append(inner);
    }
    ext.append(be16(0x0017)).append(be16(0));  // extended_master_secret
    ext.append(be16(0x0016)).append(be16(0));  // encrypt_then_mac
    ext.append(be16(0x0023)).append(be16(0));  // session_ticket
    { // supported_groups：X25519MLKEM768 在前（新版 REALITY 服务器强制要求），
      // x25519 在后（旧服务器回退用）
        QByteArray inner = be16(4) + be16(0x11EC) + be16(0x001D);
        ext.append(be16(0x000A)).append(be16(inner.size())).append(inner);
    }
    { // ec_point_formats: uncompressed
        QByteArray inner(2, 0);
        inner[0] = char(1);
        inner[1] = char(0);
        ext.append(be16(0x000B)).append(be16(inner.size())).append(inner);
    }
    { // signature_algorithms
        static const quint16 algs[] = {0x0403, 0x0804, 0x0401, 0x0501, 0x0805,
                                       0x0503, 0x0806, 0x0402, 0x0807};
        QByteArray list;
        for (quint16 a : algs) list.append(be16(a));
        QByteArray inner = be16(list.size()) + list;
        ext.append(be16(0x000D)).append(be16(inner.size())).append(inner);
    }
    { // ALPN: h2, http/1.1
        QByteArray list;
        // REALITY 模式：与真实浏览器一致（h2, http/1.1）
        // 普通 TLS 模式（ws 等）：仅声明 http/1.1 —— 否则 CDN 会协商出 h2，
        // 后续按 HTTP/1.1 编写的 WebSocket 升级请求会被当作非法请求而关闭
        if (config.reality) {
            list.append(char(2)).append("h2");
        }
        list.append(char(8)).append("http/1.1");
        QByteArray inner = be16(list.size()) + list;
        ext.append(be16(0x0010)).append(be16(inner.size())).append(inner);
    }
    { // supported_versions: TLS 1.3
        QByteArray inner(3, 0);
        inner[0] = char(2);
        inner[1] = char(0x03);
        inner[2] = char(0x04);
        ext.append(be16(0x002B)).append(be16(inner.size())).append(inner);
    }
    { // psk_key_exchange_modes: psk_dhe_ke
        QByteArray inner(2, 0);
        inner[0] = char(1);
        inner[1] = char(0x01);
        ext.append(be16(0x002D)).append(be16(inner.size())).append(inner);
    }
    { // key_share：client_shares 向量（RFC 8446 KeyShareClientHello）——
      // 结构为 [向量总长(2B)] + 条目*，每条目 = [组(2B)][数据长(2B)][数据]。
      // X25519MLKEM768（ek 1184B || x25519 公钥 32B）在前，独立 x25519 在后，
      // 与真实 Chrome/Xray 布局一致（已与真实 Xray 26.3.27 的握手字节比对）
        QByteArray mlkemShare = m_mlkemEk + m_mlkemXPub;
        QByteArray mlkemEntry = be16(0x11EC) + be16(mlkemShare.size()) + mlkemShare;
        QByteArray xEntry     = be16(0x001D) + be16(32) + m_km.ourPub;
        QByteArray shares     = mlkemEntry + xEntry;
        // 扩展数据 = [client_shares 向量总长(2B)] + 条目
        ext.append(be16(0x0033))
           .append(be16(shares.size() + 2))
           .append(be16(shares.size()))
           .append(shares);
    }

    QByteArray body;
    body.append("\x03\x03");  // legacy_version
    body.append(m_km.clientRandom);
    body.append(char(32));    // session_id 长度 32
    body.append(QByteArray(32, '\0'));
    const QByteArray suites = QByteArray::fromHex(
        "130113021303c02bc02fc02cc030cca9cca8c013c014009c009d002f0035");
    body.append(be16(suites.size())).append(suites);
    body.append(char(1)).append(char(0)); // compression: null
    body.append(be16(ext.size())).append(ext);

    m_km.rawHello.clear();
    m_km.rawHello.append(char(0x01)); // handshake type: ClientHello
    m_km.rawHello.append(be24(body.size()));
    m_km.rawHello.append(body);
    // session_id 位于偏移 39（4 hdr + 2 ver + 32 random + 1 len）
    return m_km.rawHello.size() > 71;
}

// ---------------------------------------------------------------------------
//  记录层
// ---------------------------------------------------------------------------

bool RealityTlsStream::sendRecord(uchar recordType, const QByteArray &payload, QString *error)
{
    QByteArray header;
    header.append(char(recordType));
    header.append("\x03\x01"); // ClientHello 外层 legacy 版本
    header.append(be16(payload.size()));
    m_socket->write(header + payload);
    if (!m_socket->flush()) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "REALITY send failed: %1").arg(m_socket->errorString());
        return false;
    }
    return true;
}

bool RealityTlsStream::sendEncryptedRecord(uchar recordType, const QByteArray &encryptedBody,
                                           QString *error)
{
    QByteArray header;
    header.append(char(recordType));
    header.append("\x03\x03");
    header.append(be16(encryptedBody.size()));
    m_socket->write(header + encryptedBody);
    if (!m_socket->flush()) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "REALITY send failed: %1").arg(m_socket->errorString());
        return false;
    }
    return true;
}

bool RealityTlsStream::recvRecord(uchar *recordType, QByteArray *payload,
                                  QByteArray *header, int timeoutMs, QString *error)
{
    const qint64 deadline =
        QDateTime::currentMSecsSinceEpoch() + std::max(1000, timeoutMs);
    auto pump = [&]() -> bool {
        const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
        if (remaining <= 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Timed out reading a REALITY record");
            return false;
        }
        if (!m_socket->waitForReadyRead(int(remaining))) {
            if (m_socket->bytesAvailable() == 0) {
                if (error) {
                    *error = QCoreApplication::translate("clashqt::core", "REALITY failed to read a record: %1")
                                 .arg(m_socket->errorString());
                }
                return false;
            }
        }
        m_rawInput.append(m_socket->readAll());
        return true;
    };

    while (m_rawInput.size() < 5) {
        if (!pump()) return false;
    }
    const uchar type  = uchar(m_rawInput.at(0));
    const int    len  = readBe16(m_rawInput, 3);
    if (len <= 0 || len > 17 * 1024) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Invalid REALITY record length %1").arg(len);
        return false;
    }
    while (m_rawInput.size() < 5 + len) {
        if (!pump()) return false;
    }
    *recordType = type;
    *header     = m_rawInput.left(5);
    *payload    = m_rawInput.mid(5, len);
    m_rawInput.remove(0, 5 + len);
    return true;
}

bool RealityTlsStream::decryptAvailable()
{
    // 已经判定「后面不是记录」：不再重试，等 Vision 切成直通后由 takeRaw 取走
    if (m_decryptStalled) return false;

    m_rawInput.append(m_socket->readAll());
    const char *aead = aeadCipherName(m_cipher);

    // Vision 切换点：服务端在最后一帧之后直接发内层 TLS 密文。这些字节的
    // 前 5 个字节很可能恰好长得像记录头（0x17 0x03 0x03 + 长度），一旦按记录
    // 解密就会失败。此时不能判死——把字节留在缓冲里，等 Vision 处理完
    // PaddingDirect 后 takeRaw() 原样交出去。
    const auto stallForRawTail = [this]() {
        m_decryptStalled = true;
        return false;
    };

    while (m_rawInput.size() >= 5) {
        const int len = readBe16(m_rawInput, 3);
        if (len <= 0 || len > 17 * 1024) {
            if (m_allowRawTail) return stallForRawTail();
            m_closed = true;
            m_error  = QCoreApplication::translate("clashqt::core", "Invalid REALITY application-record length");
            emit streamClosed();
            return false;
        }
        if (m_rawInput.size() < 5 + len) break;

        const uchar      type   = uchar(m_rawInput.at(0));
        const QByteArray header = m_rawInput.left(5);
        const QByteArray body   = m_rawInput.mid(5, len);
        m_rawInput.remove(0, 5 + len);

        const QByteArray nonce = recordNonce(m_serverApIv, m_serverSeq++);
        QByteArray       inner;
        if (!aeadOpen(aead, m_serverApKey, nonce, header, body, &inner)) {
            if (m_allowRawTail) {
                // 放回去（含尚未消费的尾巴），保证 takeRaw() 拿到完整字节流
                m_rawInput.prepend(header + body);
                return stallForRawTail();
            }
            m_closed = true;
            m_error  = QCoreApplication::translate("clashqt::core", "Failed to decrypt a REALITY record");
            emit streamClosed();
            return false;
        }
        int end = inner.size() - 1;
        while (end >= 0 && inner.at(end) == 0) --end;
        if (end < 0) continue;
        const uchar contentType = uchar(inner.at(end));
        if (contentType == 0x17) {
            m_plaintext.append(inner.left(end));
        } else if (contentType == 0x15) { // close_notify
            m_closed = true;
            emit streamClosed();
            return false;
        }
        // 0x16：应用阶段的 NewSessionTicket / KeyUpdate 等握手消息，忽略
        Q_UNUSED(type);
    }
    return true;
}

// ---------------------------------------------------------------------------
//  OutboundStream 接口
// ---------------------------------------------------------------------------

QByteArray RealityTlsStream::readAll()
{
    if (m_rawMode) return takeRaw();
    if (m_socket->bytesAvailable() > 0 && !m_decryptStalled) decryptAvailable();

    if (!m_expectVlessHeader) {
        const QByteArray out = m_plaintext;
        m_plaintext.clear();
        return out;
    }

    // 懒消费「裸 VLESS 应答头」：VER(1) + ADDONS_LEN(1) + ADDONS(n)。
    // 头可能跨多次读取到达，故用 m_hdrBuf 缓冲；消费完即恢复直通。
    QByteArray buf = m_hdrBuf + m_plaintext;
    m_plaintext.clear();
    m_hdrBuf.clear();

    if (m_vlessHeaderRemaining < 0) {
        if (buf.size() < 2) {
            m_hdrBuf = buf;
            return {};
        }
        m_vlessHeaderRemaining = 2 + uchar(buf.at(1));
    }
    if (buf.size() < m_vlessHeaderRemaining) {
        m_hdrBuf = buf;
        return {};
    }
    buf.remove(0, m_vlessHeaderRemaining);
    m_vlessHeaderRemaining = -1;
    m_expectVlessHeader    = false;
    return buf;
}

qint64 RealityTlsStream::write(const QByteArray &data)
{
    if (m_rawMode) return writeRaw(data);
    if (data.isEmpty()) return 0;

    const char *aead = aeadCipherName(m_cipher);
    qint64      total = 0;
    int         offset = 0;
    while (offset < data.size()) {
        const int chunk = std::min(16384, int(data.size() - offset));
        QByteArray inner = data.mid(offset, chunk);
        inner.append(char(0x17));
        const QByteArray nonce  = recordNonce(m_clientApIv, m_clientSeq++);
        const QByteArray header = QByteArray(1, char(0x17)) + QByteArray("\x03\x03") +
                                  be16(inner.size() + 16);
        const QByteArray ct = aeadSeal(aead, m_clientApKey, nonce, header, inner);

        QByteArray record;
        record.append(char(0x17));
        record.append("\x03\x03");
        record.append(be16(ct.size()));
        record.append(ct);
        m_socket->write(record);
        offset += chunk;
        total += chunk;
    }
    m_socket->flush();
    return total;
}

void RealityTlsStream::closeStream()
{
    if (m_closed) return;
    if (m_established && !m_rawMode) {
        // 尽力发送 close_notify，然后断开
        const char    *aead  = aeadCipherName(m_cipher);
        QByteArray     inner(1, char(0x15));
        const QByteArray nonce  = recordNonce(m_clientApIv, m_clientSeq++);
        const QByteArray header = QByteArray(1, char(0x17)) + QByteArray("\x03\x03") +
                                  be16(inner.size() + 16);
        QByteArray record;
        record.append(char(0x17));
        record.append("\x03\x03");
        record.append(be16(aeadSeal(aead, m_clientApKey, nonce, header, inner).size()));
        record.append(aeadSeal(aead, m_clientApKey, nonce, header, inner));
        m_socket->write(record);
        m_socket->flush();
        m_socket->disconnectFromHost();
    } else {
        m_socket->abort();
    }
    m_closed = true;
    emit streamClosed();
}

QString RealityTlsStream::errorText() const { return m_error; }

bool RealityTlsStream::isOpen() const
{
    return !m_closed && m_socket->state() == QAbstractSocket::ConnectedState;
}

bool RealityTlsStream::waitForData(int timeoutMs)
{
    if (m_rawMode) {
        if (!m_rawInput.isEmpty() || m_socket->bytesAvailable() > 0) return true;
        return m_socket->waitForReadyRead(timeoutMs);
    }
    if (!m_plaintext.isEmpty()) return true;
    if (m_decryptStalled) {
        // 尾部已判定为原始字节：不再等解密结果
        return m_socket->bytesAvailable() > 0 || !m_rawInput.isEmpty();
    }
    if (m_socket->bytesAvailable() > 0) {
        decryptAvailable();
        if (!m_plaintext.isEmpty()) return true;
        if (m_decryptStalled) return true; // 交给 Vision 走直通
    }
    if (!m_socket->waitForReadyRead(timeoutMs)) return false;
    decryptAvailable();
    return !m_plaintext.isEmpty();
}

bool RealityTlsStream::readExact(int n, QByteArray *out, int timeoutMs, QString *error)
{
    QByteArray acc;
    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + std::max(1000, timeoutMs);
    while (acc.size() < n) {
        if (m_plaintext.isEmpty()) {
            if (m_socket->bytesAvailable() > 0) decryptAvailable();
            if (m_plaintext.isEmpty()) {
                const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
                if (remaining <= 0) {
                    if (error) *error = QCoreApplication::translate("clashqt::core", "Timed out reading the response");
                    return false;
                }
                if (!m_socket->waitForReadyRead(int(remaining))) {
                    if (m_closed) {
                        if (error) *error = QCoreApplication::translate("clashqt::core", "Connection closed while reading");
                        return false;
                    }
                    continue;
                }
                decryptAvailable();
            }
        }
        acc += m_plaintext;
        m_plaintext.clear();
    }
    *out = acc.left(n);
    const QByteArray rest = acc.mid(n);
    m_plaintext = rest + m_plaintext;
    return true;
}

void RealityTlsStream::setRawMode() { m_rawMode = true; }

QByteArray RealityTlsStream::takeRaw()
{
    QByteArray out = m_rawInput; // 停止记录层解析后，未消费字节都是裸数据
    m_rawInput.clear();
    out.append(m_socket->readAll());
    return out;
}

qint64 RealityTlsStream::writeRaw(const QByteArray &data)
{
    m_socket->write(data);
    m_socket->flush();
    return data.size();
}

} // namespace core
