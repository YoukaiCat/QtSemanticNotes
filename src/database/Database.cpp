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
 ** along with QtSemanticNotesr. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#include "Database.h"

#include <QSqlDatabase>
#include <QSqlError>

#include <QDebug>

OpenDBException::OpenDBException(const QString& message)
    : message(message)
{}

OpenDBException::OpenDBException(const QSqlError& error)
    : message(error.text())
{}

void OpenDBException::raise() const
{
    throw *this;
}

OpenDBException* OpenDBException::clone() const
{
    return new OpenDBException(*this);
}

QString OpenDBException::getMessage() const
{
    return message;
}

QueryException::QueryException(const QString& message)
    : message(message)
{}

QueryException::QueryException(const QSqlError& error)
    : message(error.text())
{}

void QueryException::raise() const
{
    throw *this;
}

QueryException* QueryException::clone() const
{
    return new QueryException(*this);
}

QString QueryException::getMessage() const
{
    return message;
}

void Database::openDatabase(const QString& path)
{
    if (QSqlDatabase::database().isOpen()) {
        throw OpenDBException("Database already open");
    }

    if (!QSqlDatabase::database().isDriverAvailable("QSQLITE")) {
        QString m("QSQLITE driver isn't available\nAvailable drivers:\n");
        m.append(QSqlDatabase::database().drivers().join(","));
        throw OpenDBException(m);
    }

    QSqlDatabase::addDatabase("QSQLITE");
    if (!QSqlDatabase::database().isValid()) {
        throw OpenDBException("Driver is invalid");
    }

    QSqlDatabase::database().setDatabaseName(path);
    if (!QSqlDatabase::database().open()) {
        throw OpenDBException(QSqlDatabase::database().lastError());
    }

    //For some magical reason ON DELETE CASCADE
    //works only when the pragma query is the first query
    //https://forum.qt.io/topic/77967/foreign-key-is-ignored-when-deleting/3
    safeExecQuery("PRAGMA foreign_keys = ON;");

    QSqlQuery query("select name from application");
    if (!(query.next() && (query.value(0).toString() == QString("QtSemanticNotes")))) {
        if (QSqlDatabase::database().transaction()) {
            defineSchema();
            QSqlDatabase::database().commit();
        } else {
            throw OpenDBException("Driver does not support transactions");
        }
    }

    if (QSqlDatabase::database().lastError().isValid()) {
        throw OpenDBException(QSqlDatabase::database().lastError());
    }
}

void Database::closeDatabase()
{
    if (QSqlDatabase::database().isOpen()) {
        QSqlDatabase::database().close();
    }
}

bool Database::isOpen()
{
    return QSqlDatabase::database().isOpen();
}

void Database::defineSchema()
{
/*
    notes |#id, title, content, parent_id|
    |-> aliases |#id, alias, @note_id|
    |-> links |#id, @from_note_id, @to_note_id|
    |-> note_tags |#id, @note_id, @tag_id| <- tags |#id, name|
*/
    safeExecQuery(
        "CREATE TABLE notes ( "
            "id INTEGER PRIMARY KEY, "
            "title TEXT COLLATE NOCASE UNIQUE NOT NULL, "
            "content CLOB NOT NULL DEFAULT '', "
            "parent_id INTEGER REFERENCES notes(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')), "
            "updated_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')) "
        ")");

    safeExecQuery("CREATE INDEX notes_parent_id_fk ON notes(parent_id)");

    //Insert root note before fts triggers are created
    safeExecQuery("INSERT INTO notes (id, title, parent_id)"
        "values (1, 'root', NULL)");

    safeExecQuery(
        "CREATE TABLE aliases ( "
            "id INTEGER PRIMARY KEY, "
            "alias TEXT COLLATE NOCASE UNIQUE NOT NULL, "
            "note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')), "
            "UNIQUE(alias, note_id) ON CONFLICT REPLACE "
        ")");

    safeExecQuery("CREATE INDEX alias_note_id_fk ON aliases(note_id)");

    safeExecQuery(
        "CREATE TABLE links ( "
            "id INTEGER PRIMARY KEY, "
            "from_note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "to_note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')), "
            "UNIQUE(from_note_id, to_note_id) ON CONFLICT REPLACE "
        ")");

    safeExecQuery("CREATE INDEX links_from_note_id_fk ON links(from_note_id)");
    safeExecQuery("CREATE INDEX links_to_note_id_fk ON links(to_note_id)");

    safeExecQuery(
        "CREATE TABLE tags ( "
            "id INTEGER PRIMARY KEY, "
            "name TEXT COLLATE NOCASE UNIQUE NOT NULL, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')) "
        ")");

    safeExecQuery(
        "CREATE TABLE note_tags ( "
            "id INTEGER PRIMARY KEY, "
            "note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "tag_id INTEGER NOT NULL REFERENCES tags(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')), "
            "UNIQUE(note_id, tag_id) ON CONFLICT REPLACE "
        ")");

    //Delete tag if it is no longer used by note_tags
    safeExecQuery(
        "CREATE TRIGGER after_delete_note_tags_delete_tag "
        "AFTER DELETE ON note_tags "
        "BEGIN "
            "DELETE FROM tags "
            "WHERE tags.id = OLD.tag_id AND "
                "NOT EXISTS (SELECT note_tags.tag_id "
                            "FROM note_tags "
                            "WHERE note_tags.tag_id = OLD.tag_id); "
        "END;");

    safeExecQuery("CREATE INDEX note_note_id_fk ON note_tags(note_id)");
    safeExecQuery("CREATE INDEX note_tag_id_fk ON note_tags(tag_id)");

    safeExecQuery(
        "CREATE TABLE application ( "
            "id INTEGER PRIMARY KEY, "
            "name TEXT NOT NULL DEFAULT 'QtSemanticNotes', "
            "database_version REAL NOT NULL DEFAULT '1.0' "
        ")");

    //Full-text search
    //TODO https://github.com/abiliojr/fts5-snowball
    safeExecQuery(
        "CREATE VIRTUAL TABLE notes_fts USING fts5 ("
            "title, "
            "content, "
            "content = notes, "
            "content_rowid = id, "
            "tokenize = 'porter unicode61 remove_diacritics 1'"
        ")");

    safeExecQuery(
        "CREATE TRIGGER after_insert_note_update_index "
        "AFTER INSERT ON notes "
        "BEGIN "
            "INSERT INTO notes_fts(rowid, title, content) "
                "VALUES (NEW.id, NEW.title, NEW.content); "
        "END;");

    safeExecQuery(
        "CREATE TRIGGER after_delete_note_update_index "
        "AFTER DELETE ON notes "
        "BEGIN "
            "INSERT INTO notes_fts(notes_fts, rowid, title, content) "
                "VALUES('delete', OLD.id, OLD.title, OLD.content); "
        "END;");

    safeExecQuery(
        "CREATE TRIGGER after_update_note_update_index "
        "AFTER UPDATE ON notes BEGIN "
            "INSERT INTO notes_fts(notes_fts, rowid, title, content) "
                "VALUES('delete', OLD.id, OLD.title, OLD.content); "
            "INSERT INTO notes_fts(rowid, title, content) "
                "VALUES (NEW.id, NEW.title, NEW.content); "
        "END;");

    safeExecQuery(
        "CREATE VIRTUAL TABLE aliases_fts USING fts5 ("
            "alias, "
            "note_id UNINDEXED, "
            "content = aliases, "
            "content_rowid = id, "
            "tokenize = 'porter unicode61 remove_diacritics 1'"
        ")");

    safeExecQuery("CREATE TRIGGER after_insert_alias_update_index "
        "AFTER INSERT ON aliases "
        "BEGIN "
            "INSERT INTO aliases_fts(rowid, alias, note_id) "
                "VALUES (NEW.id, NEW.alias, NEW.note_id); "
        "END;");

    safeExecQuery("CREATE TRIGGER after_delete_alias_update_index "
        "AFTER DELETE ON aliases "
        "BEGIN "
            "INSERT INTO aliases_fts(aliases_fts, rowid, alias, note_id) "
                "VALUES('delete', OLD.id, OLD.alias, OLD.note_id); "
        "END;");

    safeExecQuery(
        "CREATE TRIGGER after_update_alias_update_index "
        "AFTER UPDATE ON aliases BEGIN "
            "INSERT INTO aliases_fts(aliases_fts, rowid, alias, note_id) "
                "VALUES('delete', OLD.id, OLD.alias, OLD.note_id); "
            "INSERT INTO aliases_fts(rowid, alias, note_id) "
                "VALUES (NEW.id, NEW.alias, NEW.note_id); "
        "END;");

    safeExecQuery("INSERT INTO application (name, database_version)"
        "values ('QtSemanticNotes', '1.0')");
}
