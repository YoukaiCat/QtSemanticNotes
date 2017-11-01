#include "Note.h"

#include "../database/Database.h"

#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>

vector<Note> Note::getAll()
{
    QSqlQuery q;
    q.prepare("SELECT notes.id, notes.title, "
              "notes.created_at, notes.updated_at, relations.parent_id "
              "FROM notes JOIN relations ON relations.note_id = notes.id "
              "WHERE NOT notes.id = 1");
    Database::safeExecPreparedQuery(q);

    vector<Note> notes;
    while (q.next()) {
        notes.emplace_back(q.value(0).toUInt(),
            q.value(1).toString(),
            q.value(2).toDateTime(),
            q.value(3).toDateTime(),
            q.value(4).toUInt());
    }
    return notes;
}

optional<Note> Note::getById(const Id& id)
{
    QSqlQuery q;
    q.prepare("SELECT notes.id, notes.title, "
              "notes.created_at, notes.updated_at, relations.parent_id "
              "FROM notes JOIN relations ON relations.note_id = notes.id "
              "WHERE notes.id = :id AND NOT notes.id = 1");
    q.bindValue(":id", id);
    Database::safeExecPreparedQuery(q);

    if (q.next()) {
        Note note(q.value(0).toUInt(),
            q.value(1).toString(),
            q.value(2).toDateTime(),
            q.value(3).toDateTime(),
            q.value(4).toUInt());
        return note;
    } else {
        return {};
    }
}

Id Note::getId() const
{
    return id;
}

QString Note::getTitle() const
{
    return title;
}

void Note::setTitle(const QString& value)
{
    title = value;
}

QString Note::getContent()
{
    if(content.has_value()) {
        return content.value();
    } else {
        QSqlQuery q;
        q.prepare("SELECT content FROM notes "
                  "WHERE id = :id");
        q.bindValue(":id", id);
        Database::safeExecPreparedQuery(q);

        if (q.next()) {
            content = q.value(0).toString();
            return content.value();
        }
    }
}

void Note::setContent(const QString& value)
{
    content = value;
}

QDateTime Note::getCreatedAt() const
{
    return createdAt;
}

QDateTime Note::getUpdatedAt() const
{
    return updatedAt;
}

Id Note::getParentId() const
{
    return parentId;
}

void Note::setParentId(const Id& value)
{
    parentId = value;
}

QString Note::toString() const
{
    return QString("id: %1, title: %2, content: %3, "
                   "created_at: %4, updated_at: %5, parent_id: %6")
            .arg(QString::number(id))
            .arg(title)
            .arg(content.value_or("(not loaded)"))
            .arg(createdAt.toString())
            .arg(updatedAt.toString())
            .arg(parentId);
}

Note Note::create(const QString& title, const QString& content, const Id& parentId)
{
    QDateTime now = QDateTime::currentDateTime();
    QString now_s = now.toString(Qt::ISODateWithMs);

    QSqlDatabase::database().transaction();
    try {
        QSqlQuery insertNotesQuery;
        insertNotesQuery.prepare("INSERT INTO notes (title, content, created_at, updated_at) "
                  "VALUES (:title, :content, :created_at, :updated_at)");
        insertNotesQuery.bindValue(":title", title);
        insertNotesQuery.bindValue(":content", content);
        insertNotesQuery.bindValue(":created_at", now_s);
        insertNotesQuery.bindValue(":updated_at", now_s);
        Database::safeExecPreparedQuery(insertNotesQuery);

        Id id;

        QSqlQuery rowIdQuery;
        rowIdQuery.prepare("SELECT last_insert_rowid()");
        Database::safeExecPreparedQuery(rowIdQuery);
        if (rowIdQuery.next()) {
            id = rowIdQuery.value(0).toUInt();
            return Note(id, title, content, now, now, parentId);
        } else {
            throw "";
        }

        QSqlQuery insertRelationsQuery;
        insertRelationsQuery.prepare("INSERT INTO relations (note_id, parent_id, created_at, updated_at) "
                  "VALUES (:note_id, :parent_id, :created_at, :updated_at)");
        insertRelationsQuery.bindValue(":note_id", id);
        insertRelationsQuery.bindValue(":parent_id", parentId);
        insertRelationsQuery.bindValue(":created_at", now_s);
        insertRelationsQuery.bindValue(":updated_at", now_s);
        Database::safeExecPreparedQuery(insertRelationsQuery);

        QSqlDatabase::database().commit();
    } catch (QueryException e) {
        QSqlDatabase::database().rollback();
        throw e;
    }
}

void Note::update()
{
    QDateTime now = QDateTime::currentDateTime();
    QString now_s = now.toString(Qt::ISODateWithMs);

    QSqlQuery updateNotesQuery;
    if (content.has_value()) {
        updateNotesQuery.prepare("UPDATE notes "
                  "SET title = :title, "
                  "content = :content, "
                  "updated_at = :updated_at "
                  "WHERE id = :id");
        updateNotesQuery.bindValue(":id", id);
        updateNotesQuery.bindValue(":title", title);
        updateNotesQuery.bindValue(":content", content.value());
        updateNotesQuery.bindValue(":updated_at", now_s);
    } else {
        updateNotesQuery.prepare("UPDATE notes "
                  "SET title = :title, "
                  "updated_at = :updated_at "
                  "WHERE id = :id");
        updateNotesQuery.bindValue(":id", id);
        updateNotesQuery.bindValue(":title", title);
        updateNotesQuery.bindValue(":updated_at", now_s);
    }

    QSqlQuery updateRelationsQuery;
    updateRelationsQuery.prepare("UPDATE relations "
              "SET parent_id = :parent_id, "
              "updated_at = :updated_at "
              "WHERE note_id = :note_id");
    updateRelationsQuery.bindValue(":note_id", id);
    updateRelationsQuery.bindValue(":parent_id", parentId);
    updateRelationsQuery.bindValue(":updated_at", now_s);

    QSqlDatabase::database().transaction();
    try {
        Database::safeExecPreparedQuery(updateNotesQuery);
        Database::safeExecPreparedQuery(updateRelationsQuery);
        QSqlDatabase::database().commit();
    } catch (QueryException e) {
        QSqlDatabase::database().rollback();
        throw e;
    }

    updatedAt = now;
}

void Note::remove()
{
    QSqlQuery q;
    q.prepare("DELETE FROM notes "
              "WHERE id = :id");
    q.bindValue(":id", id);
    Database::safeExecPreparedQuery(q);
}
