#include "RootNote.h"

#include "../database/Database.h"

using std::move;

#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>

optional<RootNote> RootNote::getRootNote()
{
    QSqlQuery q;
    q.prepare("SELECT id, title, created_at, updated_at "
              "FROM notes WHERE id = 1");
    Database::safeExecPreparedQuery(q);

    if (q.next()) {
        RootNote rootNote(q.value(0).toUInt(),
            q.value(1).toString(),
            q.value(2).toDateTime(),
            q.value(3).toDateTime());
        return rootNote;
    } else {
        return {};
    }
}

Id RootNote::getId() const
{
    return id;
}

QString RootNote::getTitle() const
{
    return title;
}

void RootNote::setTitle(const QString& value)
{
    title = value;
}

QString RootNote::getContent()
{
    if(content.has_value()) {
        return content.value();
    } else {
        QSqlQuery q;
        q.prepare("SELECT content FROM notes "
                  "WHERE id = 1");
        Database::safeExecPreparedQuery(q);

        if (q.next()) {
            content = q.value(0).toString();
            return content.value();
        }
    }
}

void RootNote::setContent(const QString& value)
{
    content = value;
}

QDateTime RootNote::getCreatedAt() const
{
    return createdAt;
}

QDateTime RootNote::getUpdatedAt() const
{
    return updatedAt;
}

QString RootNote::toString() const
{
    return QString("id: %1, title: %2, content: %3, "
                   "created_at: %4, updated_at: %5, parent_id: %6")
            .arg(QString::number(id))
            .arg(title)
            .arg(content.value_or("(not loaded)"))
            .arg(createdAt.toString())
            .arg(updatedAt.toString());
}

void RootNote::update()
{
    QDateTime now = QDateTime::currentDateTime();
    QString now_s = now.toString(Qt::ISODateWithMs);

    QSqlQuery updateRootQuery;
    if (content.has_value()) {
        updateRootQuery.prepare("UPDATE notes "
                  "SET title = :title, "
                  "content = :content, "
                  "updated_at = :updated_at "
                  "WHERE id = 1");
        updateRootQuery.bindValue(":title", title);
        updateRootQuery.bindValue(":content", content.value());
        updateRootQuery.bindValue(":updated_at", now_s);
    } else {
        updateRootQuery.prepare("UPDATE notes "
                  "SET title = :title, "
                  "updated_at = :updated_at "
                  "WHERE id = 1");
        updateRootQuery.bindValue(":title", title);
        updateRootQuery.bindValue(":updated_at", now_s);
    }

    updatedAt = now;
}
