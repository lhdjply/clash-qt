#include "ui/UiFactory.h"

#include "utils/AppLogger.h"

#include <QApplication>
#include <QFont>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QStyle>
#include <QStyleFactory>
#include <QVBoxLayout>

namespace clashqt {
namespace ui {

QGroupBox *makeCard(const QString &title, QWidget *parent)
{
    auto *card = new QGroupBox(title, parent);
    card->setObjectName(QStringLiteral("card"));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);
    return card;
}

QWidget *makeFormRow(const QString &label, QWidget *field, const QString &hint, QWidget *parent)
{
    auto *row = new QWidget(parent);
    auto *layout = new QVBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    auto *caption = new QLabel(label, row);
    caption->setObjectName(QStringLiteral("formLabel"));
    layout->addWidget(caption);
    layout->addWidget(field);

    if (!hint.isEmpty()) {
        auto *hintLabel = new QLabel(hint, row);
        hintLabel->setObjectName(QStringLiteral("hintLabel"));
        hintLabel->setWordWrap(true);
        layout->addWidget(hintLabel);
    }
    return row;
}

QWidget *makePageHeader(const QString &title, const QString &description, QWidget *parent)
{
    auto *header = new QWidget(parent);
    auto *layout = new QVBoxLayout(header);
    layout->setContentsMargins(0, 0, 0, 8);
    layout->setSpacing(4);

    auto *titleLabel = new QLabel(title, header);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    layout->addWidget(titleLabel);

    if (!description.isEmpty()) {
        auto *descLabel = new QLabel(description, header);
        descLabel->setObjectName(QStringLiteral("pageDescription"));
        descLabel->setWordWrap(true);
        layout->addWidget(descLabel);
    }
    return header;
}

QPushButton *makePrimaryButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setObjectName(QStringLiteral("primaryButton"));
    return button;
}

QPushButton *makeDangerButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setObjectName(QStringLiteral("dangerButton"));
    return button;
}

QLabel *makeHintLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName(QStringLiteral("hintLabel"));
    label->setWordWrap(true);
    return label;
}

namespace {

// 统一使用 Qt 的 Fusion 风格：
//   · 不随桌面环境（GTK / Breeze / Windows11）变化，各平台外观一致；
//   · 按钮、输入框、下拉框、勾选框、滚动条、表头等一律交由 Fusion 绘制，
//     样式表只负责 Fusion 无法表达的强调色与卡片外观。
void ensureFusionStyle()
{
    if (!qApp) return;
    QStyle *current = QApplication::style();
    if (current && current->objectName() == QLatin1String("fusion")) return;
    if (QStyle *fusion = QStyleFactory::create(QStringLiteral("Fusion"))) {
        QApplication::setStyle(fusion); // 所有权移交 QApplication
    }
}

// 深色调色板：Fusion 依据调色板绘制控件，主题切换必须同步调色板，
// 否则会出现「浅色控件 + 深色背景」的错配。
QPalette darkPalette()
{
    QPalette p;
    p.setColor(QPalette::Window, QColor("#1e1f22"));
    p.setColor(QPalette::WindowText, QColor("#e6e6e6"));
    p.setColor(QPalette::Base, QColor("#26282c"));
    p.setColor(QPalette::AlternateBase, QColor("#2c2f33"));
    p.setColor(QPalette::ToolTipBase, QColor("#26282c"));
    p.setColor(QPalette::ToolTipText, QColor("#e6e6e6"));
    p.setColor(QPalette::Text, QColor("#e6e6e6"));
    p.setColor(QPalette::Button, QColor("#2c2f33"));
    p.setColor(QPalette::ButtonText, QColor("#e6e6e6"));
    p.setColor(QPalette::BrightText, QColor("#ff6b6b"));
    p.setColor(QPalette::Link, QColor("#4285f4"));
    p.setColor(QPalette::Highlight, QColor("#2f6feb"));
    p.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    p.setColor(QPalette::PlaceholderText, QColor("#9aa0a6"));
    // 禁用态整体压暗，与可用控件区分
    p.setColor(QPalette::Disabled, QPalette::Text, QColor("#6b7075"));
    p.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#6b7075"));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#6b7075"));
    return p;
}

} // namespace

void applyTheme(bool dark)
{
    ensureFusionStyle();

    // ---- 调色板：Fusion 的绘制依据 ----------------------------------------
    if (qApp) {
        if (dark) {
            qApp->setPalette(darkPalette());
        } else if (QStyle *style = qApp->style()) {
            // 浅色直接使用 Fusion 自带的标准调色板
            qApp->setPalette(style->standardPalette());
        }

        // Fusion 不定义字号，沿用原有 13px 正文观感
        QFont font = QApplication::font();
        font.setPixelSize(13);
        QApplication::setFont(font);
    }

    // ---- 样式表：只保留 Fusion 无法表达的部分 -----------------------------
    // 这里刻意不再覆盖 QPushButton / QLineEdit / QComboBox / QSpinBox /
    // QListWidget / QTableWidget / QHeaderView 等通用控件 —— 一旦覆盖，Fusion
    // 的绘制就被绕过，外观会退回「扁平自定义」而非 Fusion 风格。
    const QString sheet = dark ? QStringLiteral(R"(
            QGroupBox#card { border: 1px solid #3a3d42; border-radius: 8px;
                             margin-top: 14px; background-color: #26282c; }
            QGroupBox#card::title { subcontrol-origin: margin; left: 10px;
                                    padding: 0 4px; color: #9aa0a6; }
            QLabel#pageTitle { font-size: 20px; font-weight: 600; }
            QLabel#pageDescription, QLabel#hintLabel { color: #9aa0a6; font-size: 12px; }
            QLabel#formLabel { color: #c8ccd1; }
            QPushButton#primaryButton { background-color: #2f6feb; border: 1px solid #2f6feb;
                                        border-radius: 6px; padding: 6px 12px;
                                        color: #ffffff; }
            QPushButton#primaryButton:hover { background-color: #4285f4; }
            QPushButton#dangerButton { background-color: #7a2b2b; border: 1px solid #7a2b2b;
                                       border-radius: 6px; padding: 6px 12px;
                                       color: #ffffff; }
            QPushButton#dangerButton:hover { background-color: #933333; }
            QListWidget#navList { background: transparent; border: 0; }
            QListWidget#navList::item { padding: 9px 12px; border-radius: 6px; }
            QListWidget#navList::item:hover { background-color: #2c2f33; }
            QListWidget#navList::item:selected { background-color: #2f6feb; color: #ffffff; }
        )")
        : QStringLiteral(R"(
            QGroupBox#card { border: 1px solid #dcdfe4; border-radius: 8px;
                             margin-top: 14px; background-color: #ffffff; }
            QGroupBox#card::title { subcontrol-origin: margin; left: 10px;
                                    padding: 0 4px; color: #646a73; }
            QLabel#pageTitle { font-size: 20px; font-weight: 600; }
            QLabel#pageDescription, QLabel#hintLabel { color: #646a73; font-size: 12px; }
            QLabel#formLabel { color: #434a54; }
            QPushButton#primaryButton { background-color: #2f6feb; border: 1px solid #2f6feb;
                                        border-radius: 6px; padding: 6px 12px;
                                        color: #ffffff; }
            QPushButton#primaryButton:hover { background-color: #4285f4; }
            QPushButton#dangerButton { background-color: #d93026; border: 1px solid #d93026;
                                       border-radius: 6px; padding: 6px 12px;
                                       color: #ffffff; }
            QPushButton#dangerButton:hover { background-color: #e8463c; }
            QListWidget#navList { background: transparent; border: 0; }
            QListWidget#navList::item { padding: 9px 12px; border-radius: 6px; }
            QListWidget#navList::item:hover { background-color: #eef0f3; }
            QListWidget#navList::item:selected { background-color: #2f6feb; color: #ffffff; }
        )");

    if (qApp) qApp->setStyleSheet(sheet);

    AppLogger::instance().info(QStringLiteral("界面样式：Fusion（%1）")
                                   .arg(dark ? QStringLiteral("深色")
                                             : QStringLiteral("浅色")));
}

} // namespace ui
} // namespace clashqt
