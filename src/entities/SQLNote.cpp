#include "SQLNote.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>

#include <QDebug>

SQLNote::SQLNote()
{}

QList<SQLNote> SQLNote::getAll()
{
    QSqlQuery q;
    q.prepare("select id, title, content, created_at, updated_at from notes");
    q.exec();

    QList<SQLNote> notes;
    while (q.next()) {
         SQLNote note;
         note.setId(q.value(0).toInt());
         note.setTitle(q.value(1).toString());
         note.setContent(q.value(2).toString());
         note.setCreatedAt(q.value(3).toDateTime());
         note.setUpdatedAt(q.value(4).toDateTime());
         notes.append(note);
    }
    return notes;
}

SQLNote SQLNote::getById(int id)
{
    QSqlQuery q;
    q.prepare("select id, title, content, created_at, updated_at "
              "from notes where id = :id");
    q.bindValue(":id", id);
    q.exec();

    SQLNote note;
    while (q.next()) {
         note.setId(q.value(0).toInt());
         note.setTitle(q.value(1).toString());
         note.setContent(q.value(2).toString());
         note.setCreatedAt(q.value(3).toDateTime());
         note.setUpdatedAt(q.value(4).toDateTime());
    }
    return note;
}

int SQLNote::getId() const
{
    return id;
}

void SQLNote::setId(int value)
{
    id = value;
}

QString SQLNote::getTitle() const
{
    return title;
}

void SQLNote::setTitle(const QString& value)
{
    title = value;
}

QString SQLNote::getContent() const
{
    return content;
}

void SQLNote::setContent(const QString& value)
{
    content = value;
}

QDateTime SQLNote::getCreatedAt() const
{
    return createdAt;
}

void SQLNote::setCreatedAt(const QDateTime& value)
{
    createdAt = value;
}

QDateTime SQLNote::getUpdatedAt() const
{
    return updatedAt;
}

void SQLNote::setUpdatedAt(const QDateTime& value)
{
    updatedAt = value;
}

void SQLNote::create()
{
    QSqlQuery q;
    q.prepare("insert into notes (title, content, created_at, updated_at) "
              "values (:title, :content, :created_at, :updated_at)");
    q.bindValue(":title", getTitle());
    q.bindValue(":content", getContent());
    q.bindValue(":created_at", getCreatedAt().toString());
    q.bindValue(":updated_at", getUpdatedAt().toString());
    q.exec();
}

void SQLNote::update()
{
    QSqlQuery q;
    q.prepare("update notes"
              "set title = :title, content = :content, "
              "created_at = :created_at, updated_at = :updated_at "
              "where id = :id");
    q.bindValue(":id", getId());
    q.bindValue(":title", getTitle());
    q.bindValue(":content", getContent());
    q.bindValue(":created_at", getCreatedAt().toString());
    q.bindValue(":updated_at", getUpdatedAt().toString());
    q.exec();
}

QString SQLNote::toString()
{
    return QString("id: %1, title: %2, content: %3, created_at: %4, updated_at: %5")
            .arg(QString::number(id))
            .arg(title)
            .arg(content)
            .arg(createdAt.toString())
            .arg(updatedAt.toString());
}
