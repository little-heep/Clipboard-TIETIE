//
// Created by Micheal on 2026/3/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ClipboardItemWidget.h" resolved

#include "ClipboardItemWidget.h"

#include <QPushButton>


ClipboardItemWidget::ClipboardItemWidget(const QString &text,const QImage &img,QWidget *parent) : QWidget(parent) {
    setMaximumHeight(100);
    setFixedWidth(100);
    setAttribute(Qt::WA_StyledBackground);
    setAutoFillBackground(false);

    //判断image是否为空设置不同的类型

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(12, 8, 12, 8);

    QLabel *contentLabel = new QLabel(text);
    contentLabel->setWordWrap(true);
    lay->addWidget(contentLabel, 1);

    // 示例：加个小删除按钮
    QPushButton *delBtn = new QPushButton("×");
    delBtn->setFixedSize(24, 24);
    // delBtn->setStyleSheet("...圆角+背景...");
    lay->addWidget(delBtn);

    // 核心：阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(10);
    shadow->setXOffset(2);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 60));  // 透明度别太高
    setGraphicsEffect(shadow);

    // 圆角背景（很重要！）
    setStyleSheet(R"(
            QWidget {
                background: white;
                border: 1px solid #e0e0e0;
            }
        )");
}


ClipboardItemWidget::~ClipboardItemWidget() {

}