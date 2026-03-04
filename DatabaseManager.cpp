//
// Created by Micheal on 2026/3/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DatabaseManager.h" resolved

#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <qiodevice.h>
#include <QBuffer>

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

    QByteArray bytes;
    if (!image.isNull()) {
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        // 建议保存为 PNG 格式以保留透明度并保证无损
        image.save(&buffer, "PNG");
    }

    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO clipboard_history (content, is_image, image_data, created_at, tag)
        VALUES (?, ?, ?, ?, ?)
    )");
    query.addBindValue(text);
    query.addBindValue(!image.isNull() ? 1 : 0);
    if (bytes.isEmpty()) {
        query.addBindValue(QVariant(QMetaType::fromType<QByteArray>())); // 存入空值
    } else {
        query.addBindValue(bytes); // 存入图片二进制流
    }
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

bool DatabaseManager::clearAll() {
    if (!db.isOpen()) {
        qWarning() << "数据库未打开";
        return false;
    }
    QSqlQuery query(db);

    // 开启事务处理（提高效率并保证原子性）
    db.transaction();
    // 删除所有数据
    if (!query.exec("DELETE FROM clipboard_history")) {
        qWarning() << "删除记录失败:" << query.lastError().text();
        db.rollback();
        return false;
    }
    // 重置自增 ID 计数器
    query.exec("DELETE FROM sqlite_sequence WHERE name='clipboard_history'");
    // 提交事务
    if (!db.commit()) {
        qWarning() << "事务提交失败";
        return false;
    }
    // 释放磁盘空间
    query.exec("VACUUM");

    return true;
}

QList<ClipboardRecord> DatabaseManager::getAllRecords() {
    QList<ClipboardRecord> records;
    QSqlQuery query(db);

    if (!query.exec("SELECT id, content, image_data, created_at, tag FROM clipboard_history ORDER BY created_at ASC")) {
        qDebug() << "查询失败：" << query.lastError().text();
        return records;
    }

    while (query.next()) {
        ClipboardRecord rec;
        rec.id = query.value(0).toInt();
        rec.text = query.value(1).toString();

        // 将 BLOB 转回 QImage
        QByteArray ba = query.value(2).toByteArray();
        if (!ba.isEmpty()) {
            rec.image.loadFromData(ba, "PNG");
        }

        rec.time = QDateTime::fromString(query.value(3).toString(), Qt::ISODate);
        rec.tag = query.value(4).toString();
        records.append(rec);
    }
    return records;
}