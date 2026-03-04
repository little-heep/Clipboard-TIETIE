//
// Created by Micheal on 2026/3/1.
//

#ifndef REMINDER_RESOURCEMANAGER_H
#define REMINDER_RESOURCEMANAGER_H

#include <QMainWindow>
#include <QClipboard>
#include <QMainWindow>
#include <QTextEdit>
#include <QKeySequence>
#include <QShortcut>
#include <QListWidget>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QHotKey>
#include "ClipboardItemWidget.h"


class ResourceManager : public QMainWindow {
    Q_OBJECT

public:
    explicit ResourceManager(QWidget *parent = nullptr);

    ~ResourceManager() override;

private:
    QClipboard *clipboard;
    QList<QImage> images;

    QWidget *centralWidget;
    //上部
    QWidget *topWidget;
    QVBoxLayout *topLayout;
    QLabel *titleLabel;
    QLineEdit *serchLabel;
    QPushButton *deleteallbtn;
    //下部
    QWidget *historyArea;
    QVBoxLayout *historyLayout;
    QListWidget *listWidget;


    void setstyle();

    void positionToTopRight();
    void displayExistingRecord(int id, const QString &text, const QImage &image, const QDateTime &time, const QString &tag);
    void addHistoryItem(const QString &text,const QImage &image);

    void loadHistory();

private slots:
    void toggleWindow() ;
    void copy() ;
    void onClipboardChanged();

    void onDeleteRecord(int recordId);

    void onTagUpdated(int recordId, const QString &newTag);
};


#endif //REMINDER_RESOURCEMANAGER_H