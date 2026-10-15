#pragma once

// ---------------------------------------------------------------------------
//  流量数值格式化
//
//  对应 clash-party 的 src/main/utils/calc.ts：
//    * calcTraffic(byte)         字节数 -> "1.23 MB" 形式
//    * calcSpeed(bytePerSecond)  速率 -> "1.23 MB/s" 形式
//    * calcTimeLeft(used,total,rate) 预计可用时长
//
//  格式规则与原实现一致：保留 2 位小数；超过 5 个字符时降为 1 位小数；
//  仍超长则取整，避免在窄列中被截断。
// ---------------------------------------------------------------------------

#include <QString>

namespace clashqt {

namespace calc {

QString traffic(qint64 bytes);
QString speed(qint64 bytesPerSecond);
// 剩余流量按当前速率还能用多久；rate 为 0 时返回 "--"
QString timeLeft(double usedBytes, double totalBytes, double bytesPerSecond);
// 字节数保留两位小数的可读形式（不带单位换算），用于表格
QString number(double value);

} // namespace calc

} // namespace clashqt
