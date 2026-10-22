#pragma once

// ---------------------------------------------------------------------------
//  流量数值格式化
//
//  对应参照实现的 src/main/utils/calc.ts：
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

// 速率的显示单位：把「字节/秒」按 1024 进制折算成便于阅读的单位，
// 例如 4194304 -> { 1048576.0, "MB/s" }。曲线图的 Y 轴用它决定
// 「数据除以多少」以及轴标题写什么，避免轴上出现 5699531 这种七位数。
struct SpeedUnit {
    double  divisor; // 显示值 = 字节/秒 ÷ divisor
    QString name;    // 单位名，如 "MB/s"
};
SpeedUnit speedUnit(double bytesPerSecond);

QString traffic(qint64 bytes);
QString speed(qint64 bytesPerSecond);
// 剩余流量按当前速率还能用多久；rate 为 0 时返回 "--"
QString timeLeft(double usedBytes, double totalBytes, double bytesPerSecond);
// 字节数保留两位小数的可读形式（不带单位换算），用于表格
QString number(double value);

} // namespace calc

} // namespace clashqt
