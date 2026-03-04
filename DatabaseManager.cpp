//
// Created by Micheal on 2026/3/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DatabaseManager.h" resolved

#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    openDatabase();
    initDatabase();
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) db.close();
}

bool DatabaseManager::openDatabase(const QString &dbPath)
{
    db = QSqlDatabase::addDatabase("QSQLITE", "clipboard_conn");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qWarning() << "数据库打开失败:" << db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::isOpen() const
{
    return db.isOpen();
}

void DatabaseManager::initDatabase()
{
    QSqlQuery query(db);
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS clipboard_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            content TEXT,
            is_image INTEGER DEFAULT 0,
            image_data BLOB,
            created_at TEXT NOT NULL,
            tag TEXT DEFAULT ''
        )
    )");
}

int DatabaseManager::addRecord(const QString &text, const QImage &image, const QDateTime &createdTime, const QString &tag)
{
    if (!db.isOpen()) return -1;

    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO clipboard_history (content, is_image, image_data, created_at, tag)
        VALUES (?, ?, ?, ?, ?)
    )");
    query.addBindValue(text);
    query.addBindValue(!image.isNull() ? 1 : 0);
    query.addBindValue(QByteArray());  // 暂不存图片，可后续扩展
    query.addBindValue(createdTime.toString(Qt::ISODate));
    query.addBindValue(tag);

    if (!query.exec()) {
        qWarning() << "插入失败:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

bool DatabaseManager::updateTag(int recordId, const QString &newTag)
{
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("UPDATE clipboard_history SET tag = ? WHERE id = ?");
    query.addBindValue(newTag);
    query.addBindValue(recordId);

    if (!query.exec()) {
        qWarning() << "更新标签失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteRecord(int recordId)
{
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM clipboard_history WHERE id = ?");
    query.addBindValue(recordId);

    if (!query.exec()) {
        qWarning() << "删除失败:" << query.lastError().text();
        return false;
    }
    return true;
}