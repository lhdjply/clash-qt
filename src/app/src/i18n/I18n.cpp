#include "i18n/I18n.h"


#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>

namespace clashqt::i18n {
namespace {

QString g_current = QStringLiteral("en");

} // namespace

QString resolve(const QString &setting)
{
    const QString value = setting.trimmed();

    // 未配置或显式选择「跟随系统」时按桌面语言走：只有中文桌面用中文，
    // 其余一律落到英文 —— 英文是源语言，没有 .qm 也能完整显示。
    if (value.isEmpty() || value.compare(QLatin1String("system"), Qt::CaseInsensitive) == 0) {
        return QLocale::system().language() == QLocale::Chinese ? QStringLiteral("zh")
                                                                : QStringLiteral("en");
    }
    if (value.startsWith(QLatin1String("zh"), Qt::CaseInsensitive)) return QStringLiteral("zh");
    return QStringLiteral("en");
}

QString install(QCoreApplication *app, const QString &setting)
{
    g_current = resolve(setting);

    // 英文就是源语言：不装翻译器，tr() 原样返回英文
    if (g_current == QLatin1String("en")) {
        QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
        return g_current;
    }

    // 翻译器挂到 app 上，生命周期与应用一致（QTranslator 必须活过所有 tr() 调用）。
    // 资源名用完整 locale（zh_CN），与 translations/ 下的 .ts 文件名一致；
    // 运行期只关心 zh / en 两个分支，这里做一次映射而不是把 locale 拼进配置。
    const QString qmName = g_current == QLatin1String("zh")
                               ? QStringLiteral("clash-qt_zh_CN")
                               : QStringLiteral("clash-qt_") + g_current;
    auto *translator = new QTranslator(app);
    if (translator->load(QStringLiteral(":/i18n/%1.qm").arg(qmName))) {
        app->installTranslator(translator);
        QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));
    } else {
        // 资源缺失（打包漏了 .qm）时退回英文源文案：宁可全英文，也不要把
        // 未翻译的占位符或空白摆到用户面前。
        delete translator;
        g_current = QStringLiteral("en");
    }
    return g_current;
}

QString current() { return g_current; }

} // namespace clashqt::i18n
