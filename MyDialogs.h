//
// Created by Micheal on 2026/3/4.
//

#ifndef REMINDER_MYDIALOGS_H
#define REMINDER_MYDIALOGS_H
#include <QFormLayout>
#include <QLabel>
#include "KeyRecognition.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>

class AboutDialog : public QDialog {
public:
    explicit AboutDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("关于 TIETIE");
        setFixedSize(300, 380);

        // 1. 设置整体样式
        // 背景色采用淡淡的紫粉色，文字采用深紫色
        this->setStyleSheet(R"(
            QDialog {
                background-color: #fdf2f8;
                border: 1px solid #e5d5e0;
            }
            QLabel {
                color: #1a1a2e;
                font-family: "Segoe UI", "Microsoft YaHei";
            }
            a { color: #0066cc; text-decoration: none; }
        )");

        // 去掉问号按钮
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

        // 主布局
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(30, 40, 30, 20);
        mainLayout->setSpacing(0);

        // --- 中间内容区 ---
        QHBoxLayout *contentLayout = new QHBoxLayout();
        contentLayout->setSpacing(25);
        contentLayout->setAlignment(Qt::AlignTop);

        // 左侧：图标
        QLabel *iconLabel = new QLabel();
        QPixmap logo(":/image/setting.png"); // 替换为你的图标路径
        iconLabel->setPixmap(logo.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconLabel->setAlignment(Qt::AlignTop);
        contentLayout->addWidget(iconLabel);

        // 右侧：文本信息
        QVBoxLayout *textLayout = new QVBoxLayout();
        textLayout->setSpacing(8);

        // 应用名称和版本
        QLabel *titleLabel = new QLabel("TIETIE 2026.1");
        titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2d1a47;background-color:transparent;border:none;");

        QLabel *buildLabel = new QLabel("2026年3月1日 构建");
        buildLabel->setStyleSheet("font-size: 12px; color: #4a4a6a;background-color:transparent;border:none;");

        QLabel *licenseLabel = new QLabel("授权给 Micheal\n订阅有效期至 2027年3月1日。\nFor non-commercial use only.");
        licenseLabel->setStyleSheet("font-size: 12px; margin-top: 15px; line-height: 150%;background-color:transparent;border:none;");

        QLabel *techLabel = new QLabel("运行环境: Qt 6.5.3 + MSVC 2019 amd64\n"
                                       "版权所有 © 2024–2026 <a href='https://yourwebsite.com'>Micheal Studio s.r.o.</a>");
        techLabel->setOpenExternalLinks(true);
        techLabel->setStyleSheet("font-size: 12px; margin-top: 20px; color: #666;background-color:transparent;border:none;");

        textLayout->addWidget(titleLabel);
        textLayout->addWidget(buildLabel);
        textLayout->addWidget(licenseLabel);
        textLayout->addWidget(techLabel);
        textLayout->addStretch();

        contentLayout->addLayout(textLayout);
        mainLayout->addLayout(contentLayout);

        // --- 底部按钮区 ---
        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addStretch();

        QPushButton *closeBtn = new QPushButton("关闭(C)");

        // 按钮样式
        QString btnStyle = R"(
            QPushButton {
                padding: 6px 18px;
                border-radius: 4px;
                font-size: 13px;
            }
        )";

        closeBtn->setStyleSheet(btnStyle + R"(
            QPushButton {
                background-color: white;
                color: #333;
                border: 1px solid #c0c0c0;
            }
            QPushButton:hover { background-color: #f5f5f5; }
        )");

        btnLayout->addWidget(closeBtn);
        mainLayout->addLayout(btnLayout);

        // 按钮功能连接
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    }
};

class SettingsDialog : public QDialog {
public:
    KeyRecognition *keyEdit;
    QPushButton *saveBtn;
    QPushButton *cancelBtn;

    SettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("系统设置");
        setFixedSize(200, 170);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // 去掉问号

        // 窗口整体背景
        this->setStyleSheet("QDialog { background-color: white;border:none; }"
                            "QPushButton { border-radius: 5px;border: 1px solid #E0E0E0; }");

        QVBoxLayout *mainlayout = new QVBoxLayout(this);

       // QHBoxLayout *clayout = new QHBoxLayout();
        mainlayout->setContentsMargins(25, 25, 25, 20);
        mainlayout->setSpacing(15);

        keyEdit = new KeyRecognition(this);

        QLabel *label = new QLabel("输入框为蓝色时输入快捷键:");
        label->setStyleSheet("font-size: 10px; border: none;");
        mainlayout->addWidget(label);
        mainlayout->addWidget(keyEdit);
        // 按钮区域
        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->setSpacing(10);

        saveBtn = new QPushButton("保存配置");
        cancelBtn = new QPushButton("取消");

        btnLayout->addWidget(cancelBtn);
        btnLayout->addWidget(saveBtn);
        //mainlayout->addLayout(clayout);
        mainlayout->addLayout(btnLayout);

        // 连接信号
        connect(saveBtn, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    }
};


#endif //REMINDER_MYDIALOGS_H