#include "Note.h"

#include "../database/Database.h"

#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>

using std::make_unique;

inline Note::Note(const Id& id,
                  const QString& title,
                  const QDateTime& createdAt,
                  const QDateTime& updatedAt,
                  const Id& parentId)
    : id(id),
      title(title),
      createdAt(createdAt),
      updatedAt(updatedAt),
      parentId(parentId)
{}

inline Note::Note(const Id& id,
                  const QString& title,
                  const optional<QString>& content,
                  const QDateTime& createdAt,
                  const QDateTime& updatedAt,
                  const Id& parentId)
    : id(id),
      title(title),
      content(content),
      createdAt(createdAt),
      updatedAt(updatedAt),
      parentId(parentId)
{}

vector<unique_ptr<Note>> Note::getAll()
{
    QSqlQuery q;
    q.prepare("SELECT id, title, created_at, updated_at, parent_id FROM notes "
              "WHERE NOT id = 1");
    Database::safeExecPreparedQuery(q);

    vector<unique_ptr<Note>> notes;
    while (q.next()) {
        unique_ptr<Note> note(new Note(q.value(0).toUInt(),
                                       q.value(1).toString(),
                                       q.value(2).toDateTime(),
                                       q.value(3).toDateTime(),
                                       q.value(4).toUInt()));
        notes.push_back(move(note));
    }
    return notes;
}

optional<unique_ptr<Note>> Note::getById(const Id& id)
{
    QSqlQuery q;
    q.prepare("SELECT id, title, created_at, updated_at, parent_id FROM notes"
              "WHERE id = :id AND NOT id = 1");
    q.bindValue(":id", id);
    Database::safeExecPreparedQuery(q);

    if (q.next()) {
        unique_ptr<Note> note(new Note(q.value(0).toUInt(),
                                       q.value(1).toString(),
                                       q.value(2).toDateTime(),
                                       q.value(3).toDateTime(),
                                       q.value(4).toUInt()));
        return note;
    } else {
        return {};
    }
}

bool Note::existWithTitleOrAlias(const QString& titleOrAlias)
{
    QSqlQuery q;
    q.prepare("SELECT id FROM notes WHERE title = :title "
              "UNION "
              "SELECT id FROM aliases WHERE alias = :alias");
    q.bindValue(":title", titleOrAlias);
    q.bindValue(":alias", titleOrAlias);
    Database::safeExecPreparedQuery(q);
    return q.next();
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
        q.next();

        content = q.value(0).toString();
        return content.value();
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

unique_ptr<Note> Note::create(const QString& title,
                              const QString& content,
                              const Id& parentId)
{
    QDateTime now = QDateTime::currentDateTime();
    QString now_s = now.toString(Qt::ISODateWithMs);

    QSqlQuery insertNotesQuery;
    insertNotesQuery.prepare("INSERT INTO notes (title, content, created_at, updated_at, parent_id) "
                             "VALUES (:title, :content, :created_at, :updated_at, :parent_id)");
    insertNotesQuery.bindValue(":title", title);
    insertNotesQuery.bindValue(":content", content);
    insertNotesQuery.bindValue(":created_at", now_s);
    insertNotesQuery.bindValue(":updated_at", now_s);
    insertNotesQuery.bindValue(":parent_id", parentId);
    Database::safeExecPreparedQuery(insertNotesQuery);

    unique_ptr<Note> note(new Note(insertNotesQuery.lastInsertId().toUInt(), title, content, now, now, parentId));
    return note;
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
                                 "updated_at = :updated_at, "
                                 "parent_id = :parent_id "
                                 "WHERE id = :id");
        updateNotesQuery.bindValue(":id", id);
        updateNotesQuery.bindValue(":title", title);
        updateNotesQuery.bindValue(":content", content.value());
        updateNotesQuery.bindValue(":updated_at", now_s);
        updateNotesQuery.bindValue(":parent_id", parentId);
    } else {
        updateNotesQuery.prepare("UPDATE notes "
                                 "SET title = :title, "
                                 "updated_at = :updated_at, "
                                 "parent_id = :parent_id "
                                 "WHERE id = :id");
        updateNotesQuery.bindValue(":id", id);
        updateNotesQuery.bindValue(":title", title);
        updateNotesQuery.bindValue(":updated_at", now_s);
        updateNotesQuery.bindValue(":parent_id", parentId);
    }
    Database::safeExecPreparedQuery(updateNotesQuery);

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

QStringList Note::getPossibleLinks()
{
    QSqlQuery q;
    q.prepare("SELECT id, title, length(title) FROM notes "
              "UNION "
              "SELECT notes.id, aliases.alias, length(aliases.alias) FROM notes "
              "JOIN aliases ON aliases.note_id = notes.id "
              "ORDER BY 3 DESC");
    Database::safeExecPreparedQuery(q);

    QStringList possibleLinksList;
    while(q.next())
        possibleLinksList.append(q.value(1).toString());
    return possibleLinksList;
}

void Note::addNoteAlias(const Note* note, const QString& alias)
{
    QDateTime now = QDateTime::currentDateTime();
    QString now_s = now.toString(Qt::ISODateWithMs);

    QSqlQuery insertNoteAlias;
    insertNoteAlias.prepare("INSERT INTO aliases (alias, note_id, created_at) "
                             "VALUES (:alias, :note_id, :created_at)");
    insertNoteAlias.bindValue(":alias", alias);
    insertNoteAlias.bindValue(":note_id", note->getId());
    insertNoteAlias.bindValue(":created_at", now_s);
    Database::safeExecPreparedQuery(insertNoteAlias);
}
