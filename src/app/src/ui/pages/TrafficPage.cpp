#include "ui/pages/TrafficPage.h"

#include "AppContext.h"
#include "ui/UiFactory.h"
#include "utils/Calc.h"

#include <QAbstractAxis>
#include <QBrush>
#include <QChart>
#include <QChartView>
#include <QEvent>
#include <QLabel>
#include <QLineSeries>
#include <QPainter>
#include <QPen>
#include <QValueAxis>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

#include "hub/Hub.h"

namespace clashqt {
namespace {

// 曲线保留的采样点数（每秒 1 个采样）
constexpr int kMaxSamples = 120;

// Y 轴刻度段数：5 段 → 6 个刻度标签，落在 0 起点上
constexpr int kYIntervals = 5;

// 上下行曲线色。与侧栏「上行 / 下行」的箭头图标同色，两处观感一致
const QColor kUploadColor(0xE0, 0x9B, 0x36);
const QColor kDownloadColor(0x2F, 0xA8, 0x7C);

// 取一个「好看」的刻度步长（1 / 2 / 2.5 / 5 / 10 × 10^n），
// 让轴标签落在 1、2.5、100 这类整数位置上
double niceStep(double raw)
{
    if (raw <= 0.0) return 1.0;

    const double magnitude  = std::pow(10.0, std::floor(std::log10(raw)));
    const double normalized = raw / magnitude;

    const double step = normalized <= 1.0   ? 1.0
                        : normalized <= 2.0 ? 2.0
                        : normalized <= 2.5 ? 2.5
                        : normalized <= 5.0 ? 5.0
                                            : 10.0;
    return step * magnitude;
}

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
    m_downloadSeries = new QLineSeries(chartCard);

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
    axisY->setLabelFormat(QStringLiteral("%.0f"));
    m_axisX = axisX;
    m_axisY = axisY;

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

    // 图表配色（含深浅两套）在这里一次性对齐当前主题
    applyChartTheme();
    // 初始单位：B/s（有流量后按峰值自动升到 KB/s、MB/s…）
    applyUnitLabels(calc::speedUnit(0.0));
    updateAxisRange();

    QObject::connect(m_context->hub(), &core::Hub::trafficUpdated, this,
                     &TrafficPage::onTrafficUpdated);
}

void TrafficPage::applyChartTheme()
{
    const ui::Theme &t = ui::theme();

    // 背景交给卡片：图表自带一块实底（默认是浅色），深色界面下会突兀地
    // 留一块白，索性让它透明，露出卡片的毛玻璃底
    m_chart->setBackgroundVisible(false);
    m_chart->setPlotAreaBackgroundVisible(false);
    m_chart->setBackgroundRoundness(0);
    m_chart->setMargins(QMargins(6, 6, 6, 2));

    for (QAbstractAxis *axis : m_chart->axes()) {
        axis->setLabelsColor(t.textMuted);
        axis->setTitleBrush(QBrush(t.textMuted));
        axis->setLinePenColor(t.scrollHandle);      // 轴线：略实一点
        axis->setGridLinePen(QPen(t.separator, 1)); // 网格：浅到不喧宾夺主
    }

    // 图例同样自带浅色底：关掉它，否则深色主题下会出现一块白底 + 浅色文字
    m_chart->legend()->setBackgroundVisible(false);
    m_chart->legend()->setLabelColor(t.text);
    // 曲线线宽 2：1px 在半透明卡片上偏细，看不出「流动」的曲线感
    m_uploadSeries->setPen(QPen(kUploadColor, 2));
    m_downloadSeries->setPen(QPen(kDownloadColor, 2));
}

void TrafficPage::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    // 应用主题时会重设调色板与样式表，图表不在 QSS 的管辖范围内，
    // 需要在这里重新上色（否则深色主题下曲线图仍是浅色）
    switch (event->type()) {
    case QEvent::PaletteChange:
    case QEvent::ApplicationPaletteChange:
    case QEvent::StyleChange:
    case QEvent::ApplicationFontChange:
        applyChartTheme();
        break;
    default:
        break;
    }
}

void TrafficPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // 兜底：本页隐藏期间主题若变过，切回来时对齐一次
    applyChartTheme();
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

    // 峰值每秒衰减 2%：用它挑显示单位，单位不会随单个采样抖动，
    // 流量停下来了也能把大单位多保持一会儿
    m_peakSpeed = std::max({double(uploadSpeed), double(downloadSpeed), m_peakSpeed * 0.98});
    updateAxisUnit();

    // 追加采样点（按当前显示单位缩放），超出窗口时移除最旧的一个，保持曲线滚动
    const double x = m_uploadSeries->count() > 0
                         ? m_uploadSeries->at(m_uploadSeries->count() - 1).x() + 1.0
                         : 0.0;
    m_uploadSeries->append(x, double(uploadSpeed) / m_yDivisor);
    m_downloadSeries->append(x, double(downloadSpeed) / m_yDivisor);

    while (m_uploadSeries->count() > kMaxSamples) {
        m_uploadSeries->remove(0);
        m_downloadSeries->remove(0);
    }
    // 曲线左移后重新适配横轴范围
    if (m_uploadSeries->count() >= kMaxSamples) {
        const double minX = m_uploadSeries->at(0).x();
        m_axisX->setRange(minX, minX + kMaxSamples);
    }

    updateAxisRange();
}

void TrafficPage::updateAxisUnit()
{
    const calc::SpeedUnit unit = calc::speedUnit(m_peakSpeed);
    if (qFuzzyCompare(unit.divisor, m_yDivisor)) return;

    // 单位变了：把已有采样点整体换算过去（形状不变），Y 轴标题跟着换
    const double factor = m_yDivisor / unit.divisor;
    for (QLineSeries *series : {m_uploadSeries, m_downloadSeries}) {
        QList<QPointF> points = series->points();
        for (QPointF &point : points) point.setY(point.y() * factor);
        series->replace(points);
    }

    m_yDivisor = unit.divisor;
    applyUnitLabels(unit);
}

void TrafficPage::applyUnitLabels(const calc::SpeedUnit &unit)
{
    m_axisY->setTitleText(unit.name);
    m_uploadSeries->setName(QStringLiteral("上传 (%1)").arg(unit.name));
    m_downloadSeries->setName(QStringLiteral("下载 (%1)").arg(unit.name));
}

void TrafficPage::updateAxisRange()
{
    // 上界取「好看」的档位而不是自动适配：自动适配会把下界顶到数据最小值
    // （速度图从 0 起更直观），刻度也容易落在 1302299 这种位置上
    const double peak     = std::max(1.0, m_peakSpeed / m_yDivisor);
    const double step     = niceStep(peak / kYIntervals);
    const double upper    = step * kYIntervals;

    m_axisY->setRange(0.0, upper);
    m_axisY->setTickCount(kYIntervals + 1);
    // 步长小于 1 时留一位小数，避免刻度全挤成同一个整数
    m_axisY->setLabelFormat(step >= 1.0   ? QStringLiteral("%.0f")
                            : step >= 0.1 ? QStringLiteral("%.1f")
                                          : QStringLiteral("%.2f"));
}

} // namespace clashqt
