// ---------------------------------------------------------------------------
//  clash-qt 应用入口
//
//  对应 clash-party 的 src/main/index.ts（进程初始化部分）。
//
//  启动顺序（顺序敏感，不可调换）：
//    1. 解析命令行（--silent / --portable 数据目录已由 Paths 处理）
//    2. 单实例检查（已有实例则请求其唤醒窗口后直接退出，见下）
//    3. 初始化日志（否则此前的错误无处可查）
//    4. 初始化 AppContext（加载配置）
//    5. 创建主窗口（其中会按配置决定是否自动启动内核）
//
//  信号处理：SIGINT/SIGTERM 时先停内核再退出，避免留下监听端口。
// ---------------------------------------------------------------------------

#include "AppContext.h"
#include "config/AppConfig.h"
#include "sys/SingleInstance.h"
#include "ui/MainWindow.h"
#include "ui/UiFactory.h"
#include "utils/AppLogger.h"
#include "utils/Paths.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTimer>

#include <csignal>

namespace {

clashqt::AppContext *g_context = nullptr;

void handleSignal(int)
{
    if (g_context) g_context->stopKernel();
    QTimer::singleShot(0, qApp, &QApplication::quit);
}

} // namespace

int main(int argc, char *argv[])
{
    // Qt 图形应用必须先构造 QApplication 再创建任何窗口部件
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("clash-qt"));
    QApplication::setApplicationDisplayName(QStringLiteral("Clash Qt"));
    QApplication::setOrganizationName(QStringLiteral("clash-qt"));
    QApplication::setApplicationVersion(QStringLiteral(CLASHQT_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Clash Qt —— Clash Party 的 Qt6/C++ 实现"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption silentOption({QStringLiteral("s"), QStringLiteral("silent")},
                                    QStringLiteral("启动后不显示主窗口（配合自启动使用）"));
    QCommandLineOption portableOption(QStringLiteral("portable"),
                                      QStringLiteral("使用可执行文件旁的 data 目录作为数据目录"));
    parser.addOption(silentOption);
    parser.addOption(portableOption);
    parser.process(app);

    // ---- 单实例 -----------------------------------------------------------
    // 必须早于 AppContext 初始化：内核运行在界面进程内，第二个实例若继续
    // 初始化会重复启动内核、争抢混合端口并写入同一份配置。
    clashqt::SingleInstance instance(QStringLiteral("clash-qt"), &app);
    if (!instance.acquire()) {
        clashqt::AppLogger::instance().install();
        clashqt::AppLogger::instance().info(
            QStringLiteral("检测到已有实例在运行：已请求其显示窗口，本进程退出"));
        return 0;
    }

    // ---- 日志 -------------------------------------------------------------
    clashqt::AppLogger::instance().install();
    clashqt::AppLogger::instance().info(
        QStringLiteral("启动 clash-qt %1（%2，silent=%3）")
            .arg(QApplication::applicationVersion())
            .arg(clashqt::paths::dataDir())
            .arg(parser.isSet(silentOption) ? QStringLiteral("true")
                                            : QStringLiteral("false")));

    // ---- 应用上下文 -------------------------------------------------------
    auto *context = new clashqt::AppContext(&app);
    g_context = context;

    QString initError;
    if (!context->initialize(&initError)) {
        clashqt::AppLogger::instance().error(
            QStringLiteral("初始化失败：%1").arg(initError));
        return 1;
    }

    // 信号处理：内核监听端口必须释放后再退出
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    // 内核常驻运行，且与窗口无关：静默启动（--silent / 开机自启动）同样需要
    // 代理可用，因此在创建窗口之前就拉起，避免「没有窗口就没有代理」。
    {
        QString startError;
        if (!context->startKernel(&startError)) {
            // 首次运行没有订阅属正常情况，不弹窗打扰，只记录日志
            clashqt::AppLogger::instance().info(
                QStringLiteral("启动失败：%1").arg(startError));
        }
    }

    // 窗口按需创建：静默启动时不创建；之后若用户再次
    // 启动本程序（单实例守卫发出 activationRequested），再创建并显示。
    clashqt::MainWindow *window = nullptr;
    auto showWindow = [&]() {
        if (!window) {
            clashqt::ui::applyTheme(
                context->appConfig()->appTheme() == QLatin1String("dark"));
            window = new clashqt::MainWindow(context);
            QObject::connect(&app, &QApplication::aboutToQuit, window,
                             &QObject::deleteLater);
        }
        // 窗口可能处于「关闭到托盘」的隐藏或最小化状态，一并恢复
        window->setWindowState((window->windowState() & ~Qt::WindowMinimized)
                               | Qt::WindowActive);
        window->show();
        window->raise();
        window->activateWindow();
    };

    QObject::connect(&instance, &clashqt::SingleInstance::activationRequested, &app,
                     [&showWindow]() { showWindow(); });

    if (!parser.isSet(silentOption)) {
        showWindow();
    } else {
        clashqt::AppLogger::instance().info(QStringLiteral("静默启动：不显示主窗口"));
    }

    const int code = app.exec();

    clashqt::AppLogger::instance().info(QStringLiteral("clash-qt 已退出，退出码 %1").arg(code));
    return code;
}
