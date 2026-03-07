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

    // 设置是否开机自启
    void setAutoStart(bool enable);
    // 检查当前是否已经开启了开机自启
    bool checkAutoStart();

protected:
    // 重写鼠标事件以实现拖拽
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QClipboard *clipboard;
    QList<QImage> images;
    QHotkey *hotkey;
    bool m_isDragging = false;
    QPoint m_dragPosition;

    QWidget *centralWidget;
    //上部
    QWidget *topWidget;
    QVBoxLayout *topLayout;
    QLabel *titleLabel;
    QLineEdit *serchLabel;
    QPushButton *settingbtn;
    QMenu *settingsMenu = new QMenu(this);
    QAction *actionPreferences;
    QAction *actionClearHistory;
    QAction *actionAbout;
    QAction *actionExit;

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
    void onSearchTextChanged(const QString &text);
    void onDeleteRecord(int recordId);
    void onTagUpdated(int recordId, const QString &newTag);
    void onClearAll();
    void onPreferenceUpdated();
    void onAbout();
};


#endif //REMINDER_RESOURCEMANAGER_H