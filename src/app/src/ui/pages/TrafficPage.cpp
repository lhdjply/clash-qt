#include "ui/pages/TrafficPage.h"

#include "AppContext.h"
#include "ui/UiFactory.h"
#include "utils/Calc.h"

#include <QChart>
#include <QChartView>
#include <QLabel>
#include <QLineSeries>
#include <QPainter>
#include <QValueAxis>
#include <QVBoxLayout>

#include "hub/Hub.h"

namespace clashqt {
namespace {

// 曲线保留的采样点数（每秒 1 个采样）
constexpr int kMaxSamples = 120;

} // namespace

TrafficPage::TrafficPage(AppContext *context, QWidget *parent) : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        QStringLiteral("流量"),
        QStringLiteral("实时上下行速度与累计流量统计，曲线为最近 2 分钟的每秒采样。")));

    // ---- 统计卡片 ---------------------------------------------------------
    auto *statsCard = ui::makeCard(QStringLiteral("当前状态"), this);
    auto *statsLayout = qobject_cast<QVBoxLayout *>(statsCard->layout());

    m_uploadLabel = new QLabel(QStringLiteral("↑ 0 B/s"), statsCard);
    m_downloadLabel = new QLabel(QStringLiteral("↓ 0 B/s"), statsCard);
    m_totalLabel = new QLabel(QStringLiteral("累计 0 B / 0 B"), statsCard);
    m_connectionLabel = new QLabel(QStringLiteral("活动连接 0"), statsCard);

    for (QLabel *label : {m_uploadLabel, m_downloadLabel, m_totalLabel, m_connectionLabel}) {
        label->setStyleSheet(QStringLiteral("font-size: 15px; padding: 2px 0;"));
        statsLayout->addWidget(label);
    }
    root->addWidget(statsCard);

    // ---- 速度曲线 ---------------------------------------------------------
    auto *chartCard = ui::makeCard(QStringLiteral("速度曲线"), this);
    auto *chartLayout = qobject_cast<QVBoxLayout *>(chartCard->layout());

    m_uploadSeries = new QLineSeries(chartCard);
    m_uploadSeries->setName(QStringLiteral("上传 (B/s)"));
    m_downloadSeries = new QLineSeries(chartCard);
    m_downloadSeries->setName(QStringLiteral("下载 (B/s)"));

    m_chart = new QChart();
    m_chart->addSeries(m_uploadSeries);
    m_chart->addSeries(m_downloadSeries);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->setTitle(QString());

    auto *axisX = new QValueAxis(m_chart);
    axisX->setRange(0, kMaxSamples);
    axisX->setTitleText(QStringLiteral("秒"));
    axisX->setLabelFormat(QStringLiteral("%d"));

    auto *axisY = new QValueAxis(m_chart);
    axisY->setTitleText(QStringLiteral("字节/秒"));
    axisY->setLabelFormat(QStringLiteral("%d"));

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_uploadSeries->attachAxis(axisX);
    m_uploadSeries->attachAxis(axisY);
    m_downloadSeries->attachAxis(axisX);
    m_downloadSeries->attachAxis(axisY);

    // QChartView 在 Qt Charts 模块；直接以 widget 形式嵌入
    auto *chartView = new QChartView(m_chart, chartCard);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(260);
    chartLayout->addWidget(chartView);
    root->addWidget(chartCard, 1);

    QObject::connect(m_context->hub(), &core::Hub::trafficUpdated, this,
                     &TrafficPage::onTrafficUpdated);
}

void TrafficPage::onTrafficUpdated(qint64 uploadSpeed, qint64 downloadSpeed)
{
    m_uploadLabel->setText(QStringLiteral("↑ %1").arg(calc::speed(uploadSpeed)));
    m_downloadLabel->setText(QStringLiteral("↓ %1").arg(calc::speed(downloadSpeed)));

    m_totalUpload += uploadSpeed;
    m_totalDownload += downloadSpeed;
    m_totalLabel->setText(
        QStringLiteral("窗口累计 ↑ %1 · ↓ %2")
            .arg(calc::traffic(m_totalUpload), calc::traffic(m_totalDownload)));

    const QJsonObject stats = m_context->hub()->statistics();
    m_connectionLabel->setText(
        QStringLiteral("活动连接 %1 · 内存 %2")
            .arg(stats.value(QStringLiteral("connectionCount")).toInt())
            .arg(calc::traffic(qint64(stats.value(QStringLiteral("memoryInUse")).toDouble()))));

    // 追加采样点，超出窗口时移除最旧的一个，保持曲线滚动
    const double x = m_uploadSeries->count() > 0
                         ? m_uploadSeries->at(m_uploadSeries->count() - 1).x() + 1.0
                         : 0.0;
    m_uploadSeries->append(x, double(uploadSpeed));
    m_downloadSeries->append(x, double(downloadSpeed));

    while (m_uploadSeries->count() > kMaxSamples) {
        m_uploadSeries->remove(0);
        m_downloadSeries->remove(0);
    }
    // 曲线左移后重新适配横轴范围
    if (m_uploadSeries->count() >= kMaxSamples) {
        const double minX = m_uploadSeries->at(0).x();
        static_cast<QValueAxis *>(m_chart->axes(Qt::Horizontal).first())->setRange(minX,
                                                                                   minX + kMaxSamples);
    }
}

} // namespace clashqt
