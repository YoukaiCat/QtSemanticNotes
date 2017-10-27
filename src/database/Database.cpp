#include "Database.h"

#include <QSqlDatabase>
#include <QSqlError>

#include <QDebug>

Database::Database(QObject* parent)
    : QObject(parent)
{}

Database::~Database()
{
    closeDatabase();
}

void Database::openDatabase(const QString& path)
{
    if (QSqlDatabase::database().isOpen()) {
        qDebug() << "Database already open";
        return;
    }

    if (!QSqlDatabase::database().isDriverAvailable("QSQLITE")) {
        qDebug() << "QSQLITE driver isn't available";
        qDebug() << "Available drivers: ";
        qDebug() << QSqlDatabase::database().drivers().join(",");
        return;
    }

    QSqlDatabase::addDatabase("QSQLITE");
    if (!QSqlDatabase::database().isValid()) {
        qDebug() << "Driver is invalid";
        return;
    }

    QSqlDatabase::database().setDatabaseName(path);
    if (!QSqlDatabase::database().open()) {
        qDebug() << QSqlDatabase::database().lastError();
        return;
    }

    QSqlQuery query("select name from application");
    if (!(query.next() && (query.value(0).toString() == QString("QtSemanticNotes")))) {
        defineSchema();
    }

    if (QSqlDatabase::database().lastError().isValid()) {
        qDebug() << QSqlDatabase::database().lastError();
        return;
    }

    emit databaseOpened(path);
}

void Database::closeDatabase()
{
    if (QSqlDatabase::database().isOpen()) {
        QSqlDatabase::database().close();
    }
    emit databaseClosed();
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
    |-> note_tags |#id, @note_id, @tag_id| <- tags |#id, tag|
*/
    safeExecQuery("CREATE TABLE notes "
               "(id INTEGER PRIMARY KEY, "
               "title TEXT UNIQUE NOT NULL, "
               "content CLOB NOT NULL DEFAULT '', "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    //query.exec("CREATE INDEX notes_title ON notes(title COLLATE NOCASE)");

    //Remove all subnotes when parent note is deleted
    safeExecQuery("CREATE TRIGGER on_delete_note AFTER DELETE ON notes BEGIN "
               "DELETE FROM notes WHERE notes.id IN "
               "(SELECT note_id FROM relations WHERE parent_id = OLD.id); "
               "END");

    safeExecQuery("CREATE TABLE aliases "
               "(id INTEGER PRIMARY KEY, "
               "alias TEXT UNIQUE NOT NULL, "
               "note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    //query.exec("CREATE INDEX aliases_alias ON aliases(alias COLLATE NOCASE)");

    safeExecQuery("CREATE INDEX alias_note_id_fk ON aliases(note_id)");

    //1. Note as child (note_id) relation deleted by ON DELETE CASCADE
    //2. Note as parent (parent_id) relations implicitly deleted by trigger on_delete_note:
    //   1. Each subnote itself deleted by DELETE
    //   2. Each subnote as child (note_id) relation deleted by ON DELETE CASCADE
    safeExecQuery("CREATE TABLE relations "
               "(id INTEGER PRIMARY KEY, "
               "note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
               "parent_id INTEGER NOT NULL REFERENCES notes(id), "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    safeExecQuery("CREATE INDEX relations_note_id_fk ON relations(note_id)");
    safeExecQuery("CREATE INDEX relations_parent_id_fk ON relations(parent_id)");

    safeExecQuery("CREATE TABLE links "
               "(id INTEGER PRIMARY KEY, "
               "from_note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
               "to_note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    safeExecQuery("CREATE INDEX links_from_note_id_fk ON links(from_note_id)");
    safeExecQuery("CREATE INDEX links_to_note_id_fk ON links(to_note_id)");

    safeExecQuery("CREATE TABLE tags "
               "(id INTEGER PRIMARY KEY, "
               "tag TEXT COLLATE NOCASE UNIQUE NOT NULL, "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    safeExecQuery("CREATE TABLE note_tags "
               "(id INTEGER PRIMARY KEY, "
               "note_id INTEGER NOT NULL REFERENCES notes(id) ON DELETE CASCADE, "
               "tag_id INTEGER NOT NULL REFERENCES tags(id) ON DELETE CASCADE, "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "updated_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    safeExecQuery("CREATE INDEX note_note_id_fk ON note_tags(note_id)");
    safeExecQuery("CREATE INDEX note_tag_id_fk ON note_tags(tag_id)");

    safeExecQuery("CREATE TABLE application "
               "(id INTEGER PRIMARY KEY, "
               "name TEXT NOT NULL DEFAULT 'QtSemanticNotes', "
               "database_version REAL NOT NULL DEFAULT '1.0' "
               ")");

    //CREATE VIRTUAL TABLE note_fts USING fts5(title, aliases, content, tokenize = 'porter unicode61 remove_diacritics 1');
    //https://github.com/abiliojr/fts5-snowball

    //Root note with id = 1
    safeExecQuery("insert into notes (id, title)"
               "values (1, 'root')");

    safeExecQuery("insert into application (name, database_version)"
               "values ('QtSemanticNotes', '1.0')");

    safeExecQuery("insert into notes (title) values ('test')");
    safeExecQuery("insert into notes (title) values ('test1')");
    safeExecQuery("insert into relations (note_id, parent_id) values (2, 1)");
    safeExecQuery("insert into relations (note_id, parent_id) values (3, 2)");
}

void Database::safeExecQuery(const QString& query)
{
    QSqlQuery q;
    if(!q.exec(query)) {
        qDebug() << "Query error: ";
        qDebug() << q.lastQuery();
        qDebug() << q.lastError();
    }
}

void Database::safeExecPreparedQuery(QSqlQuery& query)
{
    if(!query.exec()) {
        qDebug() << "Query error: ";
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
    }
}
