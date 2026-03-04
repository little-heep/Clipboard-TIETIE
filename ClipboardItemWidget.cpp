//
// Created by Micheal on 2026/3/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ClipboardItemWidget.h" resolved

#include "ClipboardItemWidget.h"

#include <QPushButton>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

ClipboardItemWidget::ClipboardItemWidget(
    int recordId,
    const QString &text,
    const QImage &img,
    const QDateTime &createdTime,
    const QString &tag,
    QWidget *parent)
    : QWidget(parent),
      m_recordId(recordId),
      m_text(text),
      m_image(img),
      m_createdTime(createdTime),
      m_tag(tag.isEmpty() ? "无" : tag)
{

    setupUi();

    // 时间显示（右上角）
    m_timeLabel->setText(m_createdTime.toString("yyyy-MM-dd HH:mm"));

    // 标签显示（右下角）
    m_tagLabel->setText(m_tag);
    m_tagLabel->setCursor(Qt::PointingHandCursor);
    m_tagLabel->setToolTip("点击修改标签");

    QString displayText = m_text;

    const int MAX_DISPLAY_CHARS  = 180;   // 可调整，建议 120~250 之间
    const int MAX_DISPLAY_LINES  = 8;     // 最多显示几行

    if (!m_image.isNull()) {
        // 有图的情况
        QPixmap pixmap = QPixmap::fromImage(m_image.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_imageLabel->setPixmap(pixmap);
        m_imageLabel->setVisible(true);

        if (!m_text.isEmpty()) {
            // 图 + 文字 → 文字也截断显示
            displayText = elideText(displayText, MAX_DISPLAY_CHARS, MAX_DISPLAY_LINES);
            m_textLabel->setText(displayText);
            m_textLabel->setWordWrap(true);
            m_textLabel->setVisible(true);
        }
    }
    else if (!m_text.isEmpty()) {
        // 纯文本
        displayText = elideText(displayText, MAX_DISPLAY_CHARS, MAX_DISPLAY_LINES);
        m_textLabel->setText(displayText);
        m_textLabel->setWordWrap(true);
        m_textLabel->setVisible(true);

        // 可选：限制最大高度（更保险）
        QFontMetrics fm(m_textLabel->font());
        int lineHeight = fm.lineSpacing();
        m_textLabel->setMaximumHeight(MAX_DISPLAY_LINES * lineHeight + 12);
    }
    else {
        m_textLabel->setText("(空)");
        m_textLabel->setStyleSheet("color: gray;");
        m_textLabel->setVisible(true);
    }

    // 鼠标提示：告诉用户点一下就能复制完整内容
    if (m_text.length() > MAX_DISPLAY_CHARS || displayText.contains("…")) {
        setToolTip("点击复制完整内容");
    }


    setCursor(Qt::PointingHandCursor);
}

void ClipboardItemWidget::setupUi()
{
    setFixedWidth(330);
    setStyleSheet("border: 1.5px solid black");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(12);          // 模糊半径：越大越柔和（推荐 8~15）
    shadow->setXOffset(3);              // 水平偏移：正数向右
    shadow->setYOffset(4);              // 垂直偏移：正数向下（模拟光源在上方）
    shadow->setColor(QColor(0, 0, 0, 80));  // 黑色半透明（80/255 透明度，建议 60~120）
    this->setGraphicsEffect(shadow);
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(0);

    // 顶部：时间 + 删除按钮
    auto *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0,0,0,0);
    m_timeLabel = new QLabel(this);
    m_timeLabel->setFixedSize(100,30);
    m_timeLabel->setStyleSheet("border:none;color: #757575; font-size: 10px;");
    topLayout->addWidget(m_timeLabel);
    topLayout->addStretch();
    m_deleteBtn = new QPushButton("×", this);
    m_deleteBtn->setFixedSize(24, 24);
    m_deleteBtn->setCursor(Qt::PointingHandCursor);
    m_deleteBtn->setStyleSheet(
    "QPushButton { border: none; color: black; background: transparent; }"
    "QPushButton:hover { color: red; }"
    );
    connect(m_deleteBtn, &QPushButton::clicked, this, &ClipboardItemWidget::onDeleteClicked);
    topLayout->addWidget(m_deleteBtn);

    mainLayout->addLayout(topLayout);

    // 中间：图片或文本
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setVisible(false);
    mainLayout->addWidget(m_imageLabel, 0, Qt::AlignCenter);

    m_textLabel = new QLabel(this);
    m_textLabel->setWordWrap(true);
    m_textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_textLabel->setVisible(false);
    m_textLabel->setStyleSheet("border: none;border-radius: 10px;color:black;background-color: rgb(235,235,235);"
                               "padding-left: 10px; "
                               "padding-right: 10px; "
                               "padding-bottom: 5px;"
                               "padding-top: 5px;");
    mainLayout->addWidget(m_textLabel);

    QWidget *nowidget = new QWidget(this);
    nowidget->setFixedHeight(10);
    nowidget->setStyleSheet("border:none");
    mainLayout->addWidget(nowidget);

    // 底部：标签
    m_tagLabel = new QLineEdit(this);
    m_tagLabel->setAlignment(Qt::AlignRight);
    m_tagLabel->setFixedHeight(20);
    m_tagLabel->setCursor(Qt::PointingHandCursor);
    m_tagLabel->setStyleSheet("border: 1.5px inset #999999; border-radius: 7px;color:gray;");
    m_tagLabel->setReadOnly(true);  // 初始为只读
    m_tagLabel->setMaxLength(10);
    m_tagLabel->setFixedWidth(22);
    m_tagLabel->installEventFilter(this);  // 安装事件过滤器监听双击
    // 连接编辑完成信号（回车和失去焦点都会触发这个信号）
    connect(m_tagLabel, &QLineEdit::editingFinished, this, &ClipboardItemWidget::onEditingFinished);

    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_tagLabel);
    mainLayout->addLayout(bottomLayout);

    // 基础样式
   // this->setStyleSheet( "border: 1px solid rgba(220, 220, 220, 0.8);border-radius: 12px; padding: 15px;");

}

void ClipboardItemWidget::onDeleteClicked()
{
    emit requestDelete(m_recordId);
}

bool ClipboardItemWidget::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_tagLabel) {
        // 处理双击事件
        if (event->type() == QEvent::MouseButtonDblClick && !m_isEditing) {
            // 开启编辑模式
            m_tagLabel->setReadOnly(false);
            m_tagLabel->setFocus();
            m_tagLabel->selectAll();  // 选中所有文本便于修改
            m_isEditing = true;

            return true;  // 事件已处理
        }
    }
    return QWidget::eventFilter(obj, event);
}

void ClipboardItemWidget::onEditingFinished() {
    // 只有在编辑模式下才处理完成事件
    if (m_isEditing) {
        // 保存编辑结果
        QString newText = m_tagLabel->text();
        if (newText.isEmpty()) {
                    newText = m_tagLabel->placeholderText();
        }
        // 恢复只读状态
        m_tagLabel->setReadOnly(true);
        m_isEditing = false;

        //调整宽度
        QFontMetrics fm(font());
        int textWidth = fm.horizontalAdvance(newText);
        int newWidth = textWidth + 10;  // 增加一些边距
        // 设置最小和最大宽度限制
        newWidth = qBound(22, newWidth, 300);
        m_tagLabel->setFixedWidth(newWidth);

        emit tagChanged(m_recordId, newText);
    }
}

// 简单截断 + 加省略号（支持按字符数 + 行数双重限制）
QString ClipboardItemWidget::elideText(const QString &text, int maxChars, int maxLines)
{
    if (text.isEmpty()) return "";

    QFontMetrics fm(font());  // 使用当前控件的字体来计算
    QString result = text;

    // 先按字符数粗截
    if (result.length() > maxChars) {
        result = result.left(maxChars).trimmed() + "…";
    }
    // 再按行数精修（更严格）
    QRect boundingRect = fm.boundingRect(0, 0, width() - 20, 10000,
                                         Qt::TextWordWrap | Qt::TextWrapAnywhere,
                                         result);

    if (boundingRect.height() > maxLines * fm.lineSpacing()) {
        // 超高了 → 暴力砍到大约 maxLines 行
        QStringList lines = result.split('\n');
        if (lines.size() > maxLines) {
            lines = lines.mid(0, maxLines - 1);
            lines.last() += "…";
            result = lines.join('\n');
        } else {
            // 单行太长的情况
            result = fm.elidedText(result, Qt::ElideRight, width() - 20, Qt::TextWordWrap);
        }
    }
    return result;
}

void ClipboardItemWidget::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}