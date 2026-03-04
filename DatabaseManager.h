//
// Created by Micheal on 2026/3/2.
//

#ifndef REMINDER_DATABASEMANAGER_H
#define REMINDER_DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QImage>

typedef struct ClipboardRecord {
    int id;
    QString text;
    QImage image;
    QDateTime time;
    QString tag;
}ClipboardRecord;//一项信息

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();  // 单例模式

    bool openDatabase(const QString &dbPath = "clipboard_history.db");
    bool isOpen() const;

    // 插入新记录，返回新生成的 id（失败返回 -1）
    int addRecord(const QString &text, const QImage &image, const QDateTime &createdTime, const QString &tag = "");
    // 更新标签
    bool updateTag(int recordId, const QString &newTag);
    // 删除记录
    bool deleteRecord(int recordId);
    // 删除所有记录
    bool clearAll();
    //加载所有记录
    QList<ClipboardRecord> getAllRecords();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    void initDatabase();

    QSqlDatabase db;
};

#endif //REMINDER_DATABASEMANAGER_H