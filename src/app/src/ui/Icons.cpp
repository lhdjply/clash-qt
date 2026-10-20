#include "ui/Icons.h"
#include <QCoreApplication>

#include "utils/AppLogger.h"

#include <QApplication>
#include <QByteArray>
#include <QColor>
#include <QFile>
#include <QHash>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QSet>
#include <QSvgRenderer>

namespace clashqt {
namespace ui {
namespace icons {
namespace {

// 资源前缀：图标是项目根 resources/icons/ 下的真实 .svg 文件，
// 由 resources/resources.qrc 编译进可执行文件（见该文件的说明注释）。
const char kResourcePrefix[] = ":/icons/";

QHash<QString, QByteArray> &sourceCache()
{
    static QHash<QString, QByteArray> instance;
    return instance;
}

QHash<QString, QIcon> &iconCache()
{
    static QHash<QString, QIcon> instance;
    return instance;
}

// 读取 SVG 源码；缺失时只告警一次（避免反复刷屏），并返回空
const QByteArray &svgSource(const QString &name)
{
    auto cached = sourceCache().constFind(name);
    if (cached != sourceCache().constEnd()) return *cached;

    static QSet<QString> reported;
    QFile                file(QString::fromLatin1(kResourcePrefix) + name
                             + QStringLiteral(".svg"));
    QByteArray           data;
    if (!file.open(QIODevice::ReadOnly)) {
        if (!reported.contains(name)) {
            reported.insert(name);
            AppLogger::instance().warn(
                QCoreApplication::translate("clashqt::app", "Icon resource missing: %1 (check resources/clash-qt.qrc)")
                    .arg(file.fileName()));
        }
    } else {
        data = file.readAll();
    }
    return *sourceCache().insert(name, data);
}

qreal deviceRatio()
{
    return qApp ? qApp->devicePixelRatio() : 1.0;
}

// 把 SVG 渲染成 QIcon（按设备像素比放大，高分屏下依然锐利）
QIcon renderSvg(const QByteArray &svg, int size)
{
    if (svg.isEmpty()) return {};

    QSvgRenderer renderer(svg);
    if (!renderer.isValid()) return {};

    const qreal ratio = deviceRatio();
    if (ratio <= 0) return {};

    QImage image(QSize(size, size) * ratio, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    renderer.render(&painter);
    painter.end();

    image.setDevicePixelRatio(ratio);
    return QIcon(QPixmap::fromImage(image));
}

} // namespace

QIcon get(const QString &name, const QColor &color, int size)
{
    const QString key = name + QLatin1Char('|') + color.name() + QLatin1Char('|')
                        + QString::number(size);
    auto cached = iconCache().constFind(key);
    if (cached != iconCache().constEnd()) return *cached;

    // 描边色在 SVG 里以 currentColor 占位，这里替换为当前主题色；
    // 因此同一份文件可复用于任意状态，无须为每种颜色各存一份
    QByteArray svg = svgSource(name);
    if (!svg.isEmpty()) svg.replace("currentColor", color.name().toUtf8());

    const QIcon icon = renderSvg(svg, size);
    iconCache().insert(key, icon);
    return icon;
}

QPixmap pixmap(const QString &name, const QColor &color, int size)
{
    return get(name, color, size).pixmap(size, size);
}

QPixmap appPixmap(int size, AppIconState state)
{
    return appIcon(size, state).pixmap(size, size);
}

QIcon appIcon(int size, AppIconState state)
{
    QString stateKey;
    switch (state) {
    case AppIconState::Active: stateKey = QStringLiteral("|proxy"); break;
    case AppIconState::Tun:    stateKey = QStringLiteral("|tun"); break;
    case AppIconState::Both:   stateKey = QStringLiteral("|both"); break;
    case AppIconState::Normal: break;
    }
    const QString key = QStringLiteral("__app__") + QString::number(size) + stateKey;
    auto cached = iconCache().constFind(key);
    if (cached != iconCache().constEnd()) return *cached;

    // 应用图标自带渐变色（icons/app.svg），不参与主题替换。
    // 接管状态只替换底板的三个渐变停靠点，其余（猫头、挖空、高光、描边）
    // 全部沿用同一份 SVG —— 状态与图形各管各的，不必为每种颜色维护一份文件。
    QByteArray svg = svgSource(QStringLiteral("app"));

    struct Recolor {
        const char *from;
        const char *to;
    };
    // 红：系统代理；绿：TUN；紫：两者同时开启。
    // 三组都按「亮 → 主 → 深」排列，与原始渐变同一结构，白猫头对比度一致。
    static const Recolor kProxy[] = {
        {"#4F8CFF", "#FF8778"},
        {"#3B6EF5", "#E23B36"},
        {"#7C5CF2", "#A81C17"},
    };
    static const Recolor kTun[] = {
        {"#4F8CFF", "#6BE0A8"},
        {"#3B6EF5", "#17A06B"},
        {"#7C5CF2", "#0B5A3C"},
    };
    static const Recolor kBoth[] = {
        {"#4F8CFF", "#C08BFF"},
        {"#3B6EF5", "#8B5CF6"},
        {"#7C5CF2", "#4C1D95"},
    };

    const Recolor *table = nullptr;
    int            count = 0;
    if (state == AppIconState::Active) {
        table = kProxy;
        count = int(std::size(kProxy));
    } else if (state == AppIconState::Tun) {
        table = kTun;
        count = int(std::size(kTun));
    } else if (state == AppIconState::Both) {
        table = kBoth;
        count = int(std::size(kBoth));
    }
    for (int i = 0; i < count; ++i) {
        svg.replace(table[i].from, table[i].to);
    }

    const QIcon icon = renderSvg(svg, size);
    iconCache().insert(key, icon);
    return icon;
}

} // namespace icons
} // namespace ui
} // namespace clashqt
