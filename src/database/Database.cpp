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
            "id TEXT COLLATE NOCASE PRIMARY KEY, "
            "title TEXT COLLATE NOCASE UNIQUE NOT NULL, "
            "content CLOB NOT NULL DEFAULT '', "
            "parent_id TEXT REFERENCES notes(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')), "
            "updated_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')) "
        ")");

    safeExecQuery("CREATE INDEX notes_parent_id_fk ON notes(parent_id)");

    //Insert root note before fts triggers are created
    safeExecQuery("INSERT INTO notes (id, title, parent_id)"
        "values ('026d6efe-b835-474c-98aa-2cf9aebce321', 'root', NULL)");

    safeExecQuery(
        "CREATE TABLE aliases ( "
            "id TEXT COLLATE NOCASE PRIMARY KEY, "
            "alias TEXT COLLATE NOCASE UNIQUE NOT NULL, "
            "note_id TEXT NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')), "
            "UNIQUE(alias, note_id) ON CONFLICT REPLACE "
        ")");

    safeExecQuery("CREATE INDEX alias_note_id_fk ON aliases(note_id)");

    safeExecQuery(
        "CREATE TABLE links ( "
            "id TEXT COLLATE NOCASE PRIMARY KEY, "
            "from_note_id TEXT NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "to_note_id TEXT NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')), "
            "UNIQUE(from_note_id, to_note_id) ON CONFLICT REPLACE "
        ") WITHOUT ROWID");

    safeExecQuery("CREATE INDEX links_from_note_id_fk ON links(from_note_id)");
    safeExecQuery("CREATE INDEX links_to_note_id_fk ON links(to_note_id)");

    safeExecQuery(
        "CREATE TABLE tags ( "
            "id TEXT COLLATE NOCASE PRIMARY KEY, "
            "name TEXT COLLATE NOCASE UNIQUE NOT NULL, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')) "
        ") WITHOUT ROWID");

    safeExecQuery(
        "CREATE TABLE note_tags ( "
            "id TEXT COLLATE NOCASE PRIMARY KEY, "
            "note_id TEXT NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "tag_id TEXT NOT NULL REFERENCES tags(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%d %H:%M:%f', 'now')), "
            "UNIQUE(note_id, tag_id) ON CONFLICT REPLACE "
        ") WITHOUT ROWID");

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
            "name TEXT NOT NULL DEFAULT 'QtSemanticNotes', "
            "database_version REAL NOT NULL DEFAULT '1.0' "
        ")");

    //Full-text search
    //HACK: SQLite FTS table with external content support only
    //integer key as content_rowid. Unfortunatly we use UUID as PK
    //and can not use it with content_rowid. However, sqlite tables contains
    //implicit primary key called 'rowid' and we can use it!
    //http://sqlite.1065341.n5.nabble.com/non-integer-rowid-document-id-td86158.html
    //TODO: https://github.com/abiliojr/fts5-snowball
    safeExecQuery(
        "CREATE VIRTUAL TABLE notes_fts USING fts5 ("
            "id UNINDEXED, "
            "title, "
            "content, "
            "content = notes, "
            "content_rowid = rowid, "
            "tokenize = 'porter unicode61 remove_diacritics 1'"
        ")");

    safeExecQuery(
        "CREATE TRIGGER after_insert_note_update_index "
        "AFTER INSERT ON notes "
        "BEGIN "
            "INSERT INTO notes_fts(rowid, id, title, content) "
                "VALUES (NEW.rowid, NEW.id, NEW.title, NEW.content); "
        "END;");

    safeExecQuery(
        "CREATE TRIGGER after_delete_note_update_index "
        "AFTER DELETE ON notes "
        "BEGIN "
            "INSERT INTO notes_fts(notes_fts, rowid, id, title, content) "
                "VALUES('delete', OLD.rowid, OLD.id, OLD.title, OLD.content); "
        "END;");

    safeExecQuery(
        "CREATE TRIGGER after_update_note_update_index "
        "AFTER UPDATE ON notes BEGIN "
            "INSERT INTO notes_fts(notes_fts, rowid, id, title, content) "
                "VALUES('delete', OLD.rowid, OLD.id, OLD.title, OLD.content); "
            "INSERT INTO notes_fts(rowid, id, title, content) "
                "VALUES (NEW.rowid, NEW.id, NEW.title, NEW.content); "
        "END;");

    safeExecQuery(
        "CREATE VIRTUAL TABLE aliases_fts USING fts5 ("
            "id UNINDEXED, "
            "alias, "
            "note_id UNINDEXED, "
            "content = aliases, "
            "content_rowid = rowid, "
            "tokenize = 'porter unicode61 remove_diacritics 1'"
        ")");

    safeExecQuery("CREATE TRIGGER after_insert_alias_update_index "
        "AFTER INSERT ON aliases "
        "BEGIN "
            "INSERT INTO aliases_fts(rowid, id, alias, note_id) "
                "VALUES (NEW.rowid, NEW.id, NEW.alias, NEW.note_id); "
        "END;");

    safeExecQuery("CREATE TRIGGER after_delete_alias_update_index "
        "AFTER DELETE ON aliases "
        "BEGIN "
            "INSERT INTO aliases_fts(aliases_fts, rowid, id, alias, note_id) "
                "VALUES('delete', OLD.rowid, OLD.id, OLD.alias, OLD.note_id); "
        "END;");

    safeExecQuery(
        "CREATE TRIGGER after_update_alias_update_index "
        "AFTER UPDATE ON aliases BEGIN "
            "INSERT INTO aliases_fts(aliases_fts, rowid, id, alias, note_id) "
                "VALUES('delete', OLD.rowid, OLD.id, OLD.alias, OLD.note_id); "
            "INSERT INTO aliases_fts(rowid, id, alias, note_id) "
                "VALUES (NEW.rowid, NEW.id, NEW.alias, NEW.note_id); "
        "END;");

    safeExecQuery("INSERT INTO application (name, database_version)"
        "values ('QtSemanticNotes', '1.0')");
}
