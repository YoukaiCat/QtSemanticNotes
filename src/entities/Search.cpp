#include "Search.h"

#include "../database/Database.h"

QSqlQuery Search::findNotesByContent(const QString& words)
{
    QSqlQuery q;
    q.prepare("SELECT rowid, content FROM notes_fts WHERE notes_fts MATCH 'content : :words'");
    q.bindValue(":words", words);
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
