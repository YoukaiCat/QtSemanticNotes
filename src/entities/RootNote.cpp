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
    : Note(id, title, createdAt, updatedAt, 0)
{}

shared_ptr<RootNote> RootNote::getRootNote()
{
    QSqlQuery q;
    q.prepare("SELECT id, title, created_at, updated_at "
              "FROM notes WHERE id = 1");
    Database::safeExecPreparedQuery(q);
    q.next();

    shared_ptr<RootNote> note(new RootNote(
                                  q.value(0).toUInt(),
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
            .arg(QString::number(id))
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
