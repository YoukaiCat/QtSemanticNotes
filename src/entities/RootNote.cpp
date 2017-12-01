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

#include "RootNote.h"

#include "../database/Database.h"

#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>

using std::make_unique;

RootNote::RootNote(const Id& id,
                          const QString& title,
                          const QDateTime& createdAt,
                          const QDateTime& updatedAt)
    : Note(id, title, createdAt, updatedAt, Id())
{}

shared_ptr<RootNote> RootNote::getRootNote()
{
    QSqlQuery q;
    q.prepare("SELECT id, title, created_at, updated_at "
              "FROM notes WHERE parent_id IS NULL");
    Database::safeExecPreparedQuery(q);
    q.next();

    shared_ptr<RootNote> note(new RootNote(
                                  q.value(0).toString(),
                                  q.value(1).toString(),
                                  q.value(2).toDateTime(),
                                  q.value(3).toDateTime()));
    return note;
}

Id RootNote::getParentId() const
{
    throw "Root note don't have parent";
}

void RootNote::setParentId(const Id& value)
{
    Q_UNUSED(value)
    throw "Root note don't have parent";
}

QString RootNote::toString() const
{
    return QString("id: %1, title: %2, content: %3, "
                   "created_at: %4, updated_at: %5 ")
            .arg(id.toString())
            .arg(title)
            .arg(content.value_or("(not loaded)"))
            .arg(createdAt.toString())
            .arg(updatedAt.toString());
}

void RootNote::update()
{
    throw "Root note can't be updated";
}

void RootNote::remove()
{
    throw "Root note can't be removed";
}
