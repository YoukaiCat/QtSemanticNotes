#include "SQLiteDatabase.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

SQliteDatabase::SQliteDatabase(QObject* parent)
    : Database(parent)
{}

SQliteDatabase::~SQliteDatabase()
{
    closeDatabase();
}

void SQliteDatabase::openDatabase(const QString& path)
{
    if (!QSqlDatabase::database().isOpen()) {
        QSqlDatabase::addDatabase("QSQLITE");
        QSqlDatabase::database().setDatabaseName(path);
        QSqlDatabase::database().open();
    }
}

void SQliteDatabase::closeDatabase()
{
    if (QSqlDatabase::database().isOpen()) {
        QSqlDatabase::database().close();
    }
}

bool SQliteDatabase::isOpen()
{
    return QSqlDatabase::database().isOpen();
}

void SQliteDatabase::defineSchema()
{
    QSqlQuery query;

/**
    notes |#id, title, content, created, modified|
    |-> aliases |#id, @note, alias|
    |-> links |#id, @from, @to|
    |-> relations |#id, @note, @parent|
    |-> note_tags |#id, @note, tag|
*/

    query.exec("create table notes "
               "(id INTEGER primary key, "
               "title TEXT UNIQUE NOT NULL, "
               "content CLOB NOT NULL DEFAULT '', "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "modified_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    query.exec("create table aliases "
               "(id INTEGER primary key, "
               "note_id INTEGER UNSIGNED NOT NULL, "
               "alias TEXT UNIQUE NOT NULL, "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "modified_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    query.exec("create table links "
               "(id INTEGER primary key, "
               "from_note_id INTEGER UNSIGNED NOT NULL, "
               "to_note_id INTEGER UNSIGNED NOT NULL, "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "modified_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    query.exec("create table relations "
               "(id INTEGER primary key, "
               "note_id INTEGER UNSIGNED NOT NULL, "
               "parent_id INTEGER UNSIGNED NOT NULL, "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "modified_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    query.exec("create table tags "
               "(id INTEGER primary key, "
               "note_id INTEGER UNSIGNED NOT NULL, "
               "tag TEXT UNIQUE NOT NULL, "
               "created_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000', "
               "modified_at TEXT NOT NULL DEFAULT '2017-01-01 00:00:00.000' "
               ")");

    query.exec("create table database_version "
               "(id INTEGER primary key, "
               "version REAL NOT NULL DEFAULT '1.0', "
               ")");

    query.exec("insert into notes (title)"
               "values ('root')");

    query.exec("insert into database_version (version)"
               "values ('1.0')");
}
