// ---------------------------------------------------------------------------
//  clash-qt 界面快照工具（开发期用，不属于应用源码）
//
//  为什么不在应用里加一个 --snapshot 开关：出图只是维护文档时才需要，没必要
//  让正式二进制多一个只有维护者会用到的参数。
//
//  为什么不用桌面截图：依赖窗口管理器的截图既不稳定（窗口可能被遮挡、被重排），
//  也无法在没有显示服务的环境里跑。这里直接把窗口 render 到 QPixmap，
//  结果完全确定；配合 QT_QPA_PLATFORM=offscreen，连显示服务器都不需要。
//
//  不做布局重排：页面切换走真实控件（点导航瓦片 / 订阅卡），因此图里显示的
//  就是用户点开时看到的东西，包括侧栏选中态与徽标。
//
//  由 scripts/ui-snapshot.py 编译并运行：该脚本复用主工程已编译的对象文件，
//  只把这个文件换成 main()，所以不必为它再编一遍整个应用。
// ---------------------------------------------------------------------------

#include "AppContext.h"
#include "config/AppConfig.h"
#include "i18n/I18n.h"
#include "ui/MainWindow.h"
#include "ui/TileButton.h"
#include "ui/UiFactory.h"
#include "utils/AppLogger.h"

#include <QApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QNetworkProxyFactory>
#include <QPixmap>
#include <QPushButton>
#include <QThread>

namespace {

// 侧栏瓦片的文案由 Qt 从 .ts 里取，这里按同一套上下文翻译一次再比较，
// 出图是中文还是英文都能找到对应的瓦片
bool clickTile(QWidget *window, const char *text)
{
    const QString label = QCoreApplication::translate("clashqt::MainWindow", text);
    for (auto *tile : window->findChildren<clashqt::ui::TileButton *>()) {
        if (tile->text() == label) {
            tile->click();
            return true;
        }
    }
    return false;
}

// 页面取数是同步的，但列表控件、滚动区与徽标要等事件循环才排版到位；
// 立刻 render 会拍到上一页的残留布局
void settle(int milliseconds)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < milliseconds) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        QThread::msleep(10);
    }
}

// 2 倍像素密度出图：README 里缩到一半显示时，边框与文字才不会发虚
bool saveShot(QWidget *window, const QString &path)
{
    constexpr qreal scale = 2.0;
    QPixmap pixmap(window->size() * scale);
    pixmap.setDevicePixelRatio(scale);
    pixmap.fill(Qt::transparent);
    window->render(&pixmap);
    return pixmap.save(path);
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 本程序自己就是代理：读系统代理会绕回自己的入站端口形成自环
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    QApplication::setApplicationName(QStringLiteral(CLASHQT_APP_NAME));

    const QStringList args = QApplication::arguments();
    if (args.size() < 2) {
        qCritical("usage: ui-snapshot <output-dir>");
        return 2;
    }
    const QString outputDir = args.at(1);
    if (!QDir().mkpath(outputDir)) {
        qCritical("cannot create %s", qPrintable(outputDir));
        return 2;
    }

    clashqt::AppLogger::instance().install();

    // 语言必须先装：控件构造时就把 tr() 的结果取走了
    {
        clashqt::AppConfig preview;
        preview.load();
        clashqt::i18n::install(&app, preview.language());
    }

    clashqt::AppContext context;
    QString initError;
    if (!context.initialize(&initError)) {
        qCritical("initialize failed: %s", qPrintable(initError));
        return 1;
    }

    // 内核起不来也继续出图：徽标会隐藏、代理页会是空的，但界面本身照样能画
    QString startError;
    if (!context.startKernel(&startError)) {
        qWarning("kernel not started: %s", qPrintable(startError));
    }

    clashqt::ui::applyThemeFor(context.appConfig()->appTheme());

    clashqt::MainWindow window(&context);
    // 用窗口自己的默认尺寸出图：README 里的样子要和用户第一次打开时一致
    window.show();
    settle(400);

    struct Shot {
        const char *file;
        const char *tile; // 导航瓦片的翻译原文；为空表示点侧栏卡片
        const char *cardObjectName;
    };
    const Shot shots[] = {
        {"profiles", nullptr, "profileCard"},
        {"proxies", "Proxies", nullptr},
        {"rules", "Rules", nullptr},
        {"overrides", "Overrides", nullptr},
        {"settings", "Settings", nullptr},
    };

    for (const Shot &shot : shots) {
        if (shot.cardObjectName) {
            if (auto *card = window.findChild<QPushButton *>(
                    QLatin1String(shot.cardObjectName))) {
                card->click();
            }
        } else {
            clickTile(&window, shot.tile);
        }
        settle(600);

        const QString path =
            QStringLiteral("%1/%2.png").arg(outputDir, QLatin1String(shot.file));
        if (!saveShot(&window, path)) {
            qCritical("failed to write %s", qPrintable(path));
            context.stopKernel();
            return 1;
        }
        qInfo("wrote %s", qPrintable(path));
    }

    // 只停内核，不走 shutdown()：那条路径会去撤桌面上的系统代理，
    // 出图工具不该因为「桌面上恰好指向本机端口」而改动用户设置
    context.stopKernel();
    return 0;
}
