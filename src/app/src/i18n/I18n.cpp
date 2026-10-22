#include "i18n/I18n.h"


#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

namespace clashqt::i18n {
namespace {

QString g_current = QStringLiteral("en");

// 语言表。英文是源语言，qmName 留空：不装翻译器，tr() 原样返回英文。
const QVector<LanguageInfo> &languageTable()
{
    static const QVector<LanguageInfo> table = {
        {QStringLiteral("zh-CN"), QStringLiteral("zh"), QStringLiteral("clash-qt_zh_CN"),
         QStringLiteral("zh_CN"), QStringLiteral("简体中文")},
        {QStringLiteral("zh-TW"), QStringLiteral("zh_TW"), QStringLiteral("clash-qt_zh_TW"),
         QStringLiteral("zh_TW"), QStringLiteral("繁體中文")},
        {QStringLiteral("en-US"), QStringLiteral("en"), QString(), QStringLiteral("en"),
         QStringLiteral("English")},
        {QStringLiteral("ja-JP"), QStringLiteral("ja"), QStringLiteral("clash-qt_ja_JP"),
         QStringLiteral("ja"), QStringLiteral("日本語")},
        {QStringLiteral("ko-KR"), QStringLiteral("ko"), QStringLiteral("clash-qt_ko_KR"),
         QStringLiteral("ko"), QStringLiteral("한국어")},
        {QStringLiteral("de-DE"), QStringLiteral("de"), QStringLiteral("clash-qt_de_DE"),
         QStringLiteral("de"), QStringLiteral("Deutsch")},
        {QStringLiteral("fr-FR"), QStringLiteral("fr"), QStringLiteral("clash-qt_fr_FR"),
         QStringLiteral("fr"), QStringLiteral("Français")},
        {QStringLiteral("es-ES"), QStringLiteral("es"), QStringLiteral("clash-qt_es_ES"),
         QStringLiteral("es"), QStringLiteral("Español")},
        {QStringLiteral("ru-RU"), QStringLiteral("ru"), QStringLiteral("clash-qt_ru_RU"),
         QStringLiteral("ru"), QStringLiteral("Русский")},
    };
    return table;
}

// QLocale 与配置里都可能出现 zh-Hans / zh_CN / zh-TW 这类写法，统一成
// 小写下划线形式再比较。
QString normalize(const QString &value)
{
    QString normalized = value.trimmed().toLower();
    normalized.replace(QLatin1Char('-'), QLatin1Char('_'));
    return normalized;
}

// 配置值或系统语言 → 语言代码；认不出来返回空。
QString matchSetting(const QString &value)
{
    const QString normalized = normalize(value);
    if (normalized.isEmpty()) return QString();

    // 先按完整写法比：简体与繁体同属中文，靠语言代码前缀区分不开
    for (const LanguageInfo &language : languageTable()) {
        if (normalized == normalize(language.setting)) return language.code;
    }

    // 中文再按字形与地区兜一层：zh-Hant、zh-HK、zh-MO 都算繁体
    if (normalized.startsWith(QLatin1String("zh"))) {
        if (normalized.contains(QLatin1String("hant"))
            || normalized.contains(QLatin1String("_tw"))
            || normalized.contains(QLatin1String("_hk"))
            || normalized.contains(QLatin1String("_mo"))) {
            return QStringLiteral("zh_TW");
        }
        return QStringLiteral("zh");
    }

    // 其余语言按语言代码前缀：ja-JP / ja_JP 都落到 ja
    for (const LanguageInfo &language : languageTable()) {
        const QString code = language.code.toLower();
        if (normalized == code || normalized.startsWith(code + QLatin1Char('_')))
            return language.code;
    }
    return QString();
}

const LanguageInfo *findByCode(const QString &code)
{
    for (const LanguageInfo &language : languageTable())
        if (language.code == code) return &language;
    return nullptr;
}

// Qt 自带的标准控件翻译：标准按钮、输入框右键菜单、文件对话框等。
// 部署位置因平台而异：Linux 在 Qt 安装的 translations/ 下，Windows 由
// windeployqt 放到 exe 同级的 translations/ 下。缺失时只是这些控件显示
// 英文，不影响本程序的文案。
void installQtTranslator(QCoreApplication *app, const QString &localeName)
{
    const QString fileName = QStringLiteral("qtbase_%1").arg(localeName);
    for (const QString &dir : {QLibraryInfo::path(QLibraryInfo::TranslationsPath),
                               QCoreApplication::applicationDirPath() + QStringLiteral("/translations")}) {
        auto *translator = new QTranslator(app);
        if (translator->load(fileName, dir)) {
            app->installTranslator(translator);
            return;
        }
        delete translator;
    }
}

} // namespace

const QVector<LanguageInfo> &languages() { return languageTable(); }

QString resolve(const QString &setting)
{
    const QString value = setting.trimmed();

    // 未配置或显式选择「跟随系统」时按桌面语言走：从系统的首选语言列表里
    // 找第一个支持的语言，一个都不支持就落到英文 —— 英文是源语言，没有
    // .qm 也能完整显示。
    if (value.isEmpty() || value.compare(QLatin1String("system"), Qt::CaseInsensitive) == 0) {
        for (const QString &ui : QLocale::system().uiLanguages()) {
            const QString code = matchSetting(ui);
            if (!code.isEmpty()) return code;
        }
        return QStringLiteral("en");
    }

    const QString code = matchSetting(value);
    return code.isEmpty() ? QStringLiteral("en") : code;
}

QString install(QCoreApplication *app, const QString &setting)
{
    g_current = resolve(setting);

    const LanguageInfo *language = findByCode(g_current);
    // 英文就是源语言：不装翻译器，tr() 原样返回英文
    if (!language || language->qmName.isEmpty()) {
        g_current = QStringLiteral("en");
        QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
        return g_current;
    }

    // 翻译器挂到 app 上，生命周期与应用一致（QTranslator 必须活过所有 tr() 调用）。
    auto *translator = new QTranslator(app);
    if (translator->load(QStringLiteral(":/i18n/%1.qm").arg(language->qmName))) {
        // Qt 的目录先装，本程序的目录后装：同名条目以本程序为准。
        // 只在有 .qm 的分支装 —— 资源缺失时不留下「界面英文、菜单中文」。
        installQtTranslator(app, language->qtLocale);
        app->installTranslator(translator);
        QLocale::setDefault(QLocale(language->qtLocale));
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
