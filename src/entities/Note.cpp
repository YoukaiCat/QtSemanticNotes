#include "Note.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>

#include <QDebug>

#include "../database/Database.h"

Note::Note()
    : AbstractNote()
{}

Note::Note(const Id& id, const QString& title, const QString& content,
           const QDateTime& createdAt, const QDateTime& updatedAt, const Id& parentId)
    : AbstractNote(id, title, content, createdAt, updatedAt),
      parentId(parentId)
{}

QList<Note> Note::getAll()
{
    QSqlQuery q;
    q.prepare("SELECT notes.id, notes.title, notes.content, "
              "notes.created_at, notes.updated_at, relations.parent_id "
              "FROM notes JOIN relations ON relations.note_id = notes.id "
              "WHERE NOT notes.id = 1");
    Database::safeExecPreparedQuery(q);

    QList<Note> notes;
    while (q.next()) {
        Note note(q.value(0).toUInt(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toDateTime(),
            q.value(4).toDateTime(),
            q.value(5).toUInt());
        notes.append(note);
    }
    return notes;
}

std::optional<Note> Note::getById(Id id)
{
    QSqlQuery q;
    q.prepare("SELECT notes.id, notes.title, notes.content, "
              "notes.created_at, notes.updated_at, relations.parent_id "
              "FROM notes JOIN relations ON relations.note_id = notes.id "
              "WHERE notes.id = :id AND NOT notes.id = 1");
    q.bindValue(":id", id);
    Database::safeExecPreparedQuery(q);

    if (q.next()) {
        Note note(q.value(0).toUInt(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toDateTime(),
            q.value(4).toDateTime(),
            q.value(5).toUInt());
        return note;
    } else {
        return std::nullopt;
    }
}

bool Note::isRoot() const
{
    return false;
}

Id Note::getParentId() const
{
    return parentId;
}

void Note::setParentId(const Id& value)
{
    parentId = value;
}

void Note::create() const
{
    QSqlQuery q;
    q.prepare("INSERT INTO notes (title, content, created_at, updated_at) "
              "VALUES (:title, :content, :created_at, :updated_at)");
    q.bindValue(":title", title);
    q.bindValue(":content", content);
    q.bindValue(":created_at", createdAt.toString());
    q.bindValue(":updated_at", updatedAt.toString());
    Database::safeExecPreparedQuery(q);
}

void Note::update() const
{
    QSqlQuery q;
    q.prepare("UPDATE notes "
              "SET title = :title, "
              "content = :content, "
              "created_at = :created_at, "
              "updated_at = :updated_at "
              "WHERE id = :id");
    q.bindValue(":id", id);
    q.bindValue(":title", title);
    q.bindValue(":content", content);
    q.bindValue(":created_at", createdAt.toString());
    q.bindValue(":updated_at", updatedAt.toString());
    Database::safeExecPreparedQuery(q);

    q.prepare("UPDATE relations "
              "SET parent_id = :parent_id, "
              "updated_at = :updated_at "
              "WHERE note_id = :note_id");
    q.bindValue(":note_id", id);
    q.bindValue(":parent_id", parentId);
    q.bindValue(":parent_id", updatedAt.toString());
    Database::safeExecPreparedQuery(q);
}

void Note::remove() const
{
    QSqlQuery q;
    q.prepare("DELETE FROM notes "
              "WHERE id = :id");
    q.bindValue(":id", id);
    Database::safeExecPreparedQuery(q);
}
