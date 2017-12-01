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

#include "Search.h"

#include "../database/Database.h"

QSqlQuery Search::findNotesByWords(const QString& words)
{
    QSqlQuery q;
    q.prepare("SELECT rowid AS id, title FROM notes_fts WHERE notes_fts MATCH :words "
              "UNION "
              "SELECT note_id AS id, alias AS title FROM aliases_fts WHERE aliases_fts MATCH :alias_words");
    q.bindValue(":words", words);
    q.bindValue(":alias_words", words);
    Database::safeExecPreparedQuery(q);
    return q;
}

QSqlQuery Search::findNotesByContent(const QString& words)
{
    QSqlQuery q;
    //Really tired to fight with bindvalue and colon inside query, sorry
    q.prepare(QString("SELECT rowid AS id, content FROM notes_fts WHERE notes_fts MATCH 'content : \"%1\"'").arg(words));
    Database::safeExecPreparedQuery(q);
    return q;
}

QSqlQuery Search::findNotesByTag(const QString& fulltag)
{
    QSqlQuery q;
    q.prepare("SELECT notes.id, notes.title "
              "FROM notes "
              "JOIN note_tags ON note_tags.note_id = notes.id "
              "JOIN tags ON tags.id = note_tags.tag_id "
              "WHERE tags.name = :name OR tags.name LIKE :like_name || '%'");
    q.bindValue(":name", fulltag);
    q.bindValue(":like_name", fulltag + '.');
    Database::safeExecPreparedQuery(q);
    return q;
}
