/*************************************************************************
 ** Copyright (C) 2017 Vladislav Mileshkin
 **
 ** This file is part of QtSemanticNotes.
 **
 ** QtSemanticNotes is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** QtSemanticNotes is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with QtSemanticNotes. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#include "Note.h"

#include "../database/Database.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QLocale>
#include <QRegularExpression>

#include <QDebug>

using std::make_unique;

Note::Note(const Id& id,
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

Note::Note(const Id& id,
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

vector<shared_ptr<Note>> Note::getAll()
{
    QSqlQuery q;
    q.prepare("SELECT id, title, created_at, updated_at, parent_id FROM notes "
              "WHERE NOT id = 1");
    Database::safeExecPreparedQuery(q);

    vector<shared_ptr<Note>> notes;
    while (q.next()) {
        shared_ptr<Note> note(new Note(q.value(0).toUInt(),
                                       q.value(1).toString(),
                                       q.value(2).toDateTime(),
                                       q.value(3).toDateTime(),
                                       q.value(4).toUInt()));
        notes.push_back(move(note));
    }
    return notes;
}

optional<shared_ptr<Note>> Note::getById(const Id& id)
{
    QSqlQuery q;
    q.prepare("SELECT id, title, created_at, updated_at, parent_id FROM notes"
              "WHERE id = :id AND NOT id = 1");
    q.bindValue(":id", id);
    Database::safeExecPreparedQuery(q);

    if (q.next()) {
        shared_ptr<Note> note(new Note(q.value(0).toUInt(),
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

shared_ptr<Note> Note::create(const QString& title,
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

    shared_ptr<Note> note(new Note(insertNotesQuery.lastInsertId().toUInt(), title, content, now, now, parentId));
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
    deleted = true;
}

bool Note::isDeleted()
{
    return deleted;
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

QPair<QHash<QString,Id>,QString> Note::getPossibleLinks()
{
    QSqlQuery q;
    q.prepare("SELECT id, title, length(title) FROM notes "
              "UNION "
              "SELECT note_id, alias, length(alias) FROM aliases "
              "ORDER BY 3 DESC");
    Database::safeExecPreparedQuery(q);

    QHash<QString,Id> titleToId;
    QStringList possibleLinksOrdered;
    while(q.next()) {
         Id id = q.value(0).toUInt();
         QString title = q.value(1).toString();
         QString casefoldTitle = QLocale::system().toLower(title);
         QString escapedTitle = QRegularExpression::escape(casefoldTitle);
         titleToId.insert(escapedTitle, id);
         possibleLinksOrdered.append(escapedTitle);
    }
    return qMakePair(titleToId, possibleLinksOrdered.join('|'));
}

void Note::addNoteLink(const Id& noteFrom, const Id& noteTo)
{
    QDateTime now = QDateTime::currentDateTime();
    QString now_s = now.toString(Qt::ISODateWithMs);

    QSqlQuery insertNoteAlias;
    insertNoteAlias.prepare("INSERT INTO links (from_note_id, to_note_id, created_at) "
                             "VALUES (:from_note_id, :to_note_id, :created_at)");
    insertNoteAlias.bindValue(":from_note_id", noteFrom);
    insertNoteAlias.bindValue(":to_note_id", noteTo);
    insertNoteAlias.bindValue(":created_at", now_s);
    Database::safeExecPreparedQuery(insertNoteAlias);
}


void Note::clearLinksFrom(const Id& noteFrom)
{
    QSqlQuery q;
    q.prepare("DELETE FROM links "
              "WHERE from_note_id = :from_note_id");
    q.bindValue(":from_note_id", noteFrom);
    Database::safeExecPreparedQuery(q);
}

void Note::clearLinksTo(const Id& noteTo)
{
    QSqlQuery q;
    q.prepare("DELETE FROM links "
              "WHERE to_note_id = :to_note_id");
    q.bindValue(":to_note_id", noteTo);
    Database::safeExecPreparedQuery(q);
}

QDebug operator<<(QDebug stream, const Note& note) {
    stream << note.getTitle();
    return stream;
}

QDebug operator<<(QDebug stream, const shared_ptr<Note> note)
{
    stream << *note;
    return stream;
}
