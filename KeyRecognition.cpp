//
// Created by Micheal on 2026/3/4.
//

// You may need to build the project (run Qt uic code generator) to get "ui_KeyRecognition.h" resolved

#include "KeyRecognition.h"


KeyRecognition::KeyRecognition(QWidget *parent) : QLineEdit(parent){
    setReadOnly(true);
    setAlignment(Qt::AlignCenter);
    setPlaceholderText("点击此处录制快捷键");
    // 默认样式
    setStyleSheet(R"(
            QLineEdit {
                border: 2px solid #E0E0E0;
                border-radius: 8px;
                padding: 8px;
                background-color: #F9F9F9;
                font-size: 10px;
                font-weight: bold;
                color: #333;
            }
            QLineEdit:focus {
                border: 2px solid #0078D4; /* 激活录制时变蓝色 */
                background-color: #FFFFFF;
            }
        )");
}

KeyRecognition::~KeyRecognition() {

}

void KeyRecognition::keyPressEvent(QKeyEvent *event){
    int key = event->key();

    // 忽略单独的修饰键（Ctrl, Alt, Shift 等）
    if (key == Qt::Key_Control || key == Qt::Key_Shift ||
        key == Qt::Key_Alt || key == Qt::Key_Meta) {
        return;
        }

    // 获取修饰键状态
    Qt::KeyboardModifiers modifiers = event->modifiers();

    // 组合成快捷键序列
    QKeySequence seq(modifiers | key);
    setText(seq.toString()); // 显示出来，例如 "Ctrl+Alt+V"

    // 失去焦点，表示录制完成
    clearFocus();
}