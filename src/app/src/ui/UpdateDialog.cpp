#include "ui/UpdateDialog.h"

#include "ui/UiFactory.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace clashqt {

UpdateDialog::UpdateDialog(const ReleaseInfo &release, QWidget *parent) : QDialog(parent)
{
    setObjectName(QStringLiteral("updateDialog"));
    setWindowTitle(QStringLiteral("发现新版本"));
    setModal(true);
    setMinimumSize(620, 520);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(20, 18, 20, 16);
    root->setSpacing(12);

    root->addWidget(ui::makeDialogHeader(
        QStringLiteral("clouddown"),
        QStringLiteral("发现新版本 %1").arg(release.tagName),
        QStringLiteral("当前版本 v%1。更新日志来自 GitHub Release；"
                       "下载与安装由你自己决定，本程序不会自动替换自身。")
            .arg(QLatin1String(CLASHQT_VERSION)),
        this));

    // 正文：Release 的 body 就是作者写的更新日志。标题在部分发布里为空、也有
    // 与版本号重复的，只在非空且未出现在正文开头时补一行二级标题。
    QString markdown = release.notes.trimmed();
    // 标题常常就是版本号本身（GitHub 发布页默认这么填），而版本号已经在
    // 对话框标题里了，重复一遍只会多出一行无意义的「## 0.2.0」。
    const QString title = release.title.trimmed();
    const bool titleIsVersion =
        QRegularExpression(QStringLiteral("^[vV]?\\d+(\\.\\d+)*([-+].*)?$"))
            .match(title)
            .hasMatch();
    if (!title.isEmpty() && !titleIsVersion && !markdown.startsWith(title)) {
        markdown = QStringLiteral("## %1\n\n%2").arg(title, markdown);
    }
    if (markdown.isEmpty()) {
        markdown = QStringLiteral("（这次发布没有填写更新说明，可直接到发布页查看。）");
    }

    auto *notes = new QTextBrowser(this);
    notes->setObjectName(QStringLiteral("releaseNotes"));
    notes->setOpenExternalLinks(false); // 日志里的链接不自动打开，避免误点
    notes->setMarkdown(markdown);

    const ui::Theme &theme = ui::theme();
    notes->setStyleSheet(
        QStringLiteral("QTextBrowser { background:%1; color:%2;"
                       "border:1px solid %3; border-radius:8px; padding:10px; }")
            .arg(theme.base.name(), theme.text.name(), theme.cardBorder.name()));
    root->addWidget(notes, 1);

    const QDateTime published =
        QDateTime::fromString(release.publishedAt, Qt::ISODate).toLocalTime();
    root->addWidget(ui::makeHintLabel(
        published.isValid()
            ? QStringLiteral("发布于 %1。安装包请到发布页下载。")
                  .arg(published.toString(QStringLiteral("yyyy-MM-dd")))
            : QStringLiteral("安装包请到发布页下载。"),
        this));

    auto *actions = new QHBoxLayout();
    actions->setContentsMargins(0, 0, 0, 0);
    actions->setSpacing(8);
    actions->addStretch(1);

    auto *later = new QPushButton(QStringLiteral("稍后再说"), this);
    auto *open  = ui::makePrimaryButton(QStringLiteral("打开下载页"), this);
    ui::decorate(open, QStringLiteral("external"));
    open->setDefault(true);
    actions->addWidget(later);
    actions->addWidget(open);
    root->addLayout(actions);

    QObject::connect(later, &QPushButton::clicked, this, &QDialog::reject);
    QObject::connect(open, &QPushButton::clicked, this, [this, release]() {
        QDesktopServices::openUrl(release.pageUrl);
        accept();
    });
}

} // namespace clashqt
