#pragma once

// ---------------------------------------------------------------------------
//  流量页
//  对应 clash-party 的 src/renderer/src/pages/traffic.tsx。
//  实时速度、累计流量与 Qt Charts 绘制的近 120 秒速度曲线。
// ---------------------------------------------------------------------------

#include <QWidget>

class QLabel;

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

private:
    AppContext  *m_context = nullptr;
    QLabel      *m_uploadLabel = nullptr;
    QLabel      *m_downloadLabel = nullptr;
    QLabel      *m_totalLabel = nullptr;
    QLabel      *m_connectionLabel = nullptr;

    QChart      *m_chart = nullptr;
    QLineSeries *m_uploadSeries = nullptr;
    QLineSeries *m_downloadSeries = nullptr;

    // 保留最近 N 个采样点
    qint64 m_totalUpload = 0;
    qint64 m_totalDownload = 0;
};

} // namespace clashqt
