//
// Created by Micheal on 2026/3/2.
//

#ifndef REMINDER_CLIPBOARDITEMWIDGET_H
#define REMINDER_CLIPBOARDITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHboxLayout>
#include <QGraphicsDropShadowEffect>


class ClipboardItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit ClipboardItemWidget(const QString &text,const QImage &img,QWidget *parent = nullptr);

    ~ClipboardItemWidget() override;

private:

};


#endif //REMINDER_CLIPBOARDITEMWIDGET_H