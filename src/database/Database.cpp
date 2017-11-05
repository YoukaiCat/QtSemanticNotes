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
/**
    notes |#id, title, content|
    |-> aliases |#id, alias, @note_id|
    |-> relations |#id, @note_id, @parent_id|
    |-> links |#id, @from_note_id, @to_note_id|
    |-> note_tags |#id, @note_id, @tag_id| <- tags |#id, name|
*/
    safeExecQuery(
        "CREATE TABLE notes ( "
            "id INTEGER PRIMARY KEY, "
            "title TEXT UNIQUE NOT NULL, "
            "content CLOB NOT NULL DEFAULT '', "
            "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
        ")");

    //query.exec("CREATE INDEX notes_title ON notes(title COLLATE NOCASE)");

    //Remove all subnotes when parent note is deleted
    safeExecQuery(
        "CREATE TRIGGER after_delete_note_delete_subnotes "
        "AFTER DELETE ON notes "
        "BEGIN "
            "DELETE FROM notes WHERE notes.id IN "
                "(SELECT note_id FROM relations WHERE parent_id = OLD.id); "
        "END");

    safeExecQuery(
        "CREATE TABLE aliases ( "
            "id INTEGER PRIMARY KEY, "
            "alias TEXT UNIQUE NOT NULL, "
            "note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "UNIQUE(alias, note_id) ON CONFLICT REPLACE "
        ")");

    //query.exec("CREATE INDEX aliases_alias ON aliases(alias COLLATE NOCASE)");

    safeExecQuery("CREATE INDEX alias_note_id_fk ON aliases(note_id)");

    //1. Note as child (note_id) relation deleted by ON DELETE CASCADE
    //2. Note as parent (parent_id) relations implicitly deleted by trigger on_delete_note:
    //   1. Each subnote itself deleted by DELETE
    //   2. Each subnote as child (note_id) relation deleted by ON DELETE CASCADE
    safeExecQuery(
        "CREATE TABLE relations ( "
            "id INTEGER PRIMARY KEY, "
            "note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "parent_id INTEGER NOT NULL REFERENCES notes(id), "
            "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "UNIQUE(note_id, parent_id) ON CONFLICT REPLACE "
        ")");

    safeExecQuery("CREATE INDEX relations_note_id_fk ON relations(note_id)");
    safeExecQuery("CREATE INDEX relations_parent_id_fk ON relations(parent_id)");

    safeExecQuery(
        "CREATE TABLE links ( "
            "id INTEGER PRIMARY KEY, "
            "from_note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "to_note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "UNIQUE(from_note_id, to_note_id) ON CONFLICT REPLACE "
        ")");

    safeExecQuery("CREATE INDEX links_from_note_id_fk ON links(from_note_id)");
    safeExecQuery("CREATE INDEX links_to_note_id_fk ON links(to_note_id)");

    safeExecQuery(
        "CREATE TABLE tags ( "
            "id INTEGER PRIMARY KEY, "
            "name TEXT COLLATE NOCASE UNIQUE NOT NULL, "
            "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
        ")");

    safeExecQuery(
        "CREATE TABLE note_tags ( "
            "id INTEGER PRIMARY KEY, "
            "note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
            "tag_id INTEGER NOT NULL REFERENCES tags(id) ON DELETE CASCADE, "
            "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
            "UNIQUE(note_id, tag_id) ON CONFLICT REPLACE "
        ")");

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

    //Root note with id = 1
    safeExecQuery("insert into notes (id, title)"
        "values (1, 'root')");

    safeExecQuery("insert into application (name, database_version)"
        "values ('QtSemanticNotes', '1.0')");

    safeExecQuery("insert into notes (title) values ('1 test')");
    safeExecQuery("insert into notes (title) values ('2 test')");
    safeExecQuery("insert into relations (note_id, parent_id) values (2, 1)");
    safeExecQuery("insert into relations (note_id, parent_id) values (3, 2)");
    safeExecQuery("insert into notes (title) values ('1-1 test')");
    safeExecQuery("insert into notes (title) values ('2-2 test')");
    safeExecQuery("insert into relations (note_id, parent_id) values (4, 1)");
    safeExecQuery("insert into relations (note_id, parent_id) values (5, 4)");
    safeExecQuery("insert into aliases (alias, note_id) values ('a b c', 2)");
    safeExecQuery("insert into aliases (alias, note_id) values ('c d e', 2)");
    safeExecQuery("insert into aliases (alias, note_id) values ('f g h', 2)");
    safeExecQuery("insert into tags (name) values ('cs.lang.java')");
    safeExecQuery("insert into tags (name) values ('cs.lang.scala')");
    safeExecQuery("insert into tags (name) values ('cs.lang.java.variance')");
    safeExecQuery("insert into tags (name) values ('cs.netwoking')");
    safeExecQuery("insert into tags (name) values ('csgo.weapons')");
    safeExecQuery("insert into tags (name) values ('tag')");
    safeExecQuery("insert into note_tags (note_id, tag_id) values (2, 1)");
    safeExecQuery("insert into note_tags (note_id, tag_id) values (4, 2)");
    safeExecQuery("insert into note_tags (note_id, tag_id) values (3, 3)");
    safeExecQuery("insert into note_tags (note_id, tag_id) values (1, 4)");
    safeExecQuery("insert into note_tags (note_id, tag_id) values (1, 5)");
    safeExecQuery("insert into note_tags (note_id, tag_id) values (5, 5)");
}
