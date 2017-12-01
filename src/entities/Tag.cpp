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

#include "Tag.h"

#include "../database/Database.h"

#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>

using std::make_unique;

Tag::Tag(const Id& id,
                const QString& name,
                const QDateTime& createdAt)
    : id(id),
      name(name),
      createdAt(createdAt)
{}

vector<shared_ptr<Tag>> Tag::getAll()
{
    QSqlQuery q;
    q.prepare("SELECT id, name, created_at FROM tags");
    Database::safeExecPreparedQuery(q);

    vector<shared_ptr<Tag>> tags;
    while (q.next()) {
        shared_ptr<Tag> tag(new Tag(q.value(0).toString(),
                                    q.value(1).toString(),
                                    q.value(2).toDateTime()));
        tags.push_back(move(tag));
    }
    return tags;
}

optional<shared_ptr<Tag>> Tag::getById(const Id& id)
{
    QSqlQuery q;
    q.prepare("SELECT id, name, created_at FROM tags "
              "WHERE id = :id");
    q.bindValue(":id", id.toString());
    Database::safeExecPreparedQuery(q);

    if (q.next()) {
        shared_ptr<Tag> tag(new Tag(q.value(0).toString(),
                                    q.value(1).toString(),
                                    q.value(2).toDateTime()));
        return tag;
    } else {
        return {};
    }
}

optional<shared_ptr<Tag> > Tag::getByName(const QString& name)
{
    QSqlQuery q;
    q.prepare("SELECT id, name, created_at FROM tags "
              "WHERE name = :name");
    q.bindValue(":name", name);
    Database::safeExecPreparedQuery(q);

    if (q.next()) {
        shared_ptr<Tag> tag(new Tag(q.value(0).toString(),
                                    q.value(1).toString(),
                                    q.value(2).toDateTime()));
        return tag;
    } else {
        return {};
    }
}

Id Tag::getId() const
{
    return id;
}

QString Tag::getName() const
{
    return name;
}

void Tag::setName(const QString& value)
{
    name = value;
}

QDateTime Tag::getCreatedAt() const
{
    return createdAt;
}

QString Tag::toString() const
{
    return QString("id: %1, name: %2, created_at: %3")
            .arg(id.toString())
            .arg(name)
            .arg(createdAt.toString());
}

shared_ptr<Tag> Tag::create(const QString& name)
{
    QDateTime now = QDateTime::currentDateTime();
    QString now_s = now.toString(Qt::ISODateWithMs);

    Id id;

    QSqlQuery insertTagQuery;
    insertTagQuery.prepare("INSERT INTO tags (id, name, created_at) "
                           "VALUES (:id, :name, :created_at)");
    insertTagQuery.bindValue(":id", id.toString());
    insertTagQuery.bindValue(":name", name);
    insertTagQuery.bindValue(":created_at", now_s);
    Database::safeExecPreparedQuery(insertTagQuery);

    unique_ptr<Tag> tag(new Tag(id, name, now));
    return tag;
}

shared_ptr<Tag> Tag::getOrCreate(const QString& name)
{
    auto tagOpt = Tag::getByName(name);
    if(tagOpt.has_value()) {
        return move(tagOpt.value());
    } else {
        return Tag::create(name);
    }
}

void Tag::remove()
{
    QSqlQuery q;
    q.prepare("DELETE FROM tags "
              "WHERE id = :id");
    q.bindValue(":id", id.toString());
    Database::safeExecPreparedQuery(q);
}

void Tag::addNoteTag(const Note* note, const Tag* tag)
{
    QDateTime now = QDateTime::currentDateTime();
    QString now_s = now.toString(Qt::ISODateWithMs);

    QSqlQuery insertNoteTagsQuery;
    insertNoteTagsQuery.prepare("INSERT INTO note_tags (id, note_id, tag_id, created_at) "
                                "VALUES (:id, :note_id, :tag_id, :created_at)");
    insertNoteTagsQuery.bindValue(":id", Id().toString());
    insertNoteTagsQuery.bindValue(":note_id", note->getId().toString());
    insertNoteTagsQuery.bindValue(":tag_id", tag->getId().toString());
    insertNoteTagsQuery.bindValue(":created_at", now_s);
    Database::safeExecPreparedQuery(insertNoteTagsQuery);
}

//cs.lang.java AND cs.lang.java.*
//cs but not cs* (csgo for ex)
void Tag::deleteTagAndSubtags(const QString& fulltag)
{
    QSqlQuery q;
    q.prepare("DELETE FROM tags "
              "WHERE name = :name OR name LIKE :like_name || '%'");
    q.bindValue(":name", fulltag);
    q.bindValue(":like_name", fulltag + '.');
    Database::safeExecPreparedQuery(q);
}
