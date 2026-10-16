#include "ui/Icons.h"

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
                QStringLiteral("图标资源缺失：%1（检查 resources/clash-qt.qrc）")
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

// 标题 → 图标名。卡片与导航项的标题在页面里是稳定的中文短语，因此直接按标题
// 映射：既让既有 makeCard(标题) 调用自动获得图标，也免去在每个页面重复写图标名。
struct TitleIcon {
    const char *title;
    const char *icon;
};

const TitleIcon kCardTitles[] = {
    {"快捷设置", "quick"}, {"统计", "stats"},   {"当前状态", "activity"},
    {"速度曲线", "trend"}, {"节点", "server"},  {"日志", "logs"},
    {"配置覆写", "code"},  {"入站端口", "port"}, {"系统集成", "system"},
};

const TitleIcon kNavTitles[] = {
    {"代理", "proxy"},   {"订阅", "profiles"}, {"连接", "connections"},
    {"规则", "rules"},   {"日志", "logs"},     {"流量", "traffic"},
    {"设置", "settings"},
};

QString lookup(const TitleIcon *table, size_t count, const QString &title)
{
    for (size_t i = 0; i < count; ++i) {
        if (title == QString::fromUtf8(table[i].title)) {
            return QString::fromUtf8(table[i].icon);
        }
    }
    return {};
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

QPixmap appPixmap(int size)
{
    return appIcon(size).pixmap(size, size);
}

QIcon appIcon(int size)
{
    const QString key = QStringLiteral("__app__") + QString::number(size);
    auto cached = iconCache().constFind(key);
    if (cached != iconCache().constEnd()) return *cached;

    // 应用图标自带渐变色（icons/app.svg），不参与主题替换
    const QIcon icon = renderSvg(svgSource(QStringLiteral("app")), size);
    iconCache().insert(key, icon);
    return icon;
}

QString cardIconName(const QString &title)
{
    return lookup(kCardTitles, std::size(kCardTitles), title);
}

QString navIconName(const QString &title)
{
    return lookup(kNavTitles, std::size(kNavTitles), title);
}

QIcon forCardTitle(const QString &title, const QColor &color, int size)
{
    const QString name = cardIconName(title);
    return name.isEmpty() ? QIcon() : get(name, color, size);
}

QIcon forNavTitle(const QString &title, const QColor &color, int size)
{
    const QString name = navIconName(title);
    return name.isEmpty() ? QIcon() : get(name, color, size);
}

} // namespace icons
} // namespace ui
} // namespace clashqt
