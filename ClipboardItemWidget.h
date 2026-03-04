//
// Created by Micheal on 2026/3/2.
//

#ifndef REMINDER_CLIPBOARDITEMWIDGET_H
#define REMINDER_CLIPBOARDITEMWIDGET_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlDatabase>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QToolTip>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QLineEdit>


class ClipboardItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit ClipboardItemWidget(int recordId,                    // 数据库中的 id
        const QString &text,
        const QImage &img,
        const QDateTime &createdTime,
        const QString &tag = QString(),
        QWidget *parent = nullptr);

    int getRecordId() const { return m_recordId; }
    QString getTag() const { return m_tag; }

    signals:
        void tagChanged(int recordId, const QString &newTag);
        void requestDelete(int recordId);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onDeleteClicked();
    void onEditingFinished();

private:
    void setupUi();

    int         m_recordId;
    QString     m_text;
    QImage      m_image;
    QDateTime   m_createdTime;
    QString     m_tag;

    QLabel      *m_timeLabel    = nullptr;
    QPushButton *m_deleteBtn    = nullptr;
    QLabel      *m_imageLabel   = nullptr;
    QLabel      *m_textLabel    = nullptr;
    QLineEdit      *m_tagLabel     = nullptr;
    bool m_isEditing = false;

    // 简单截断 + 加省略号（支持按字符数 + 行数双重限制）
    QString elideText(const QString &text, int maxChars, int maxLines);

};


#endif //REMINDER_CLIPBOARDITEMWIDGET_H