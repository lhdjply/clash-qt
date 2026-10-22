#pragma once

// ---------------------------------------------------------------------------
//  流量页
//  对应参照实现的 src/renderer/src/pages/traffic.tsx。
//  实时速度、累计流量与 Qt Charts 绘制的近 120 秒速度曲线。
// ---------------------------------------------------------------------------

#include <QWidget>

// 速率的显示单位（speedUnit），用于把单位写进轴标题与曲线名
#include "utils/Calc.h"

class QEvent;
class QLabel;
class QShowEvent;
class QValueAxis;

// Qt 6 起 Qt Charts 的类位于全局命名空间（Qt 5 才在 QtCharts 命名空间内）。
// 这里只做前置声明以减少头文件依赖，不要写进任何命名空间，
// 否则会生成同名但不存在的类型，链接时产生未定义符号。
class QLineSeries;
class QChart;

namespace clashqt {

class AppContext;

class TrafficPage : public QWidget {
    Q_OBJECT

public:
    explicit TrafficPage(AppContext *context, QWidget *parent = nullptr);

    // 记录一次速度采样（由主窗口转发 Hub::trafficUpdated）
    void onTrafficUpdated(qint64 uploadSpeed, qint64 downloadSpeed);

protected:
    // 主题（调色板 / 样式表）变化时重刷图表配色：QChart 不会跟着调色板走，
    // 不处理就会在深色界面里留一块浅色白底
    void changeEvent(QEvent *event) override;
    // 切到本页时兜底对齐一次主题（避免隐藏期间漏掉主题变化）
    void showEvent(QShowEvent *event) override;

private:
    // 把当前主题的颜色应用到图表（背景交给卡片，坐标轴 / 图例 / 曲线显式着色）
    void applyChartTheme();
    // 依据流量峰值切换 Y 轴显示单位（B/s → KB/s → MB/s → GB/s），
    // 单位变化时把已有采样点整体换算，曲线形状不变
    void updateAxisUnit();
    // 把单位写进 Y 轴标题与曲线名（图例）
    void applyUnitLabels(const calc::SpeedUnit &unit);
    // 重算 Y 轴上界与刻度，使刻度落在整数位置上
    void updateAxisRange();

    AppContext  *m_context = nullptr;
    QLabel      *m_uploadLabel = nullptr;
    QLabel      *m_downloadLabel = nullptr;
    QLabel      *m_totalLabel = nullptr;
    QLabel      *m_connectionLabel = nullptr;

    QChart      *m_chart = nullptr;
    QLineSeries *m_uploadSeries = nullptr;
    QLineSeries *m_downloadSeries = nullptr;

    QValueAxis *m_axisX = nullptr;
    QValueAxis *m_axisY = nullptr;

    // 选中显示单位用的峰值（每秒衰减 2%，避免单位随单个采样抖动）
    double m_peakSpeed = 0.0;
    // 当前 Y 轴单位相对「字节/秒」的除数：曲线按它缩放后入图
    double m_yDivisor = 1.0;

    // 保留最近 N 个采样点
    qint64 m_totalUpload = 0;
    qint64 m_totalDownload = 0;
};

} // namespace clashqt
