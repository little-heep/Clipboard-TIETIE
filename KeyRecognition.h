//
// Created by Micheal on 2026/3/4.
//

#ifndef REMINDER_KEYRECOGNITION_H
#define REMINDER_KEYRECOGNITION_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QKeySequence>

class KeyRecognition : public QLineEdit {
    Q_OBJECT

public:
    explicit KeyRecognition(QWidget *parent = nullptr);

    ~KeyRecognition() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:

};


#endif //REMINDER_KEYRECOGNITION_H