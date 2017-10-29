#include "RootNote.h"

RootUnsupportedActionException::RootUnsupportedActionException(const QString& message)
    : message(message)
{}

void RootUnsupportedActionException::raise() const
{
    throw *this;
}

RootUnsupportedActionException* RootUnsupportedActionException::clone() const
{
    return new RootUnsupportedActionException(*this);
}

QString RootUnsupportedActionException::getMessage() const
{
    return message;
}

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>

#include <QException>

#include <QDebug>

#include "../database/Database.h"

RootNote::RootNote()
    : AbstractNote()
{}

RootNote::RootNote(const Id& id, const QString& title, const QString& content,
                   const QDateTime& createdAt, const QDateTime& updatedAt)
    : AbstractNote(id, title, content, createdAt, updatedAt)
{}

std::optional<RootNote> RootNote::getRoot()
{
    QSqlQuery q;
    q.prepare("SELECT id, title, content, created_at, updated_at "
              "FROM notes WHERE id = 1");
    Database::safeExecPreparedQuery(q);

    if (q.next()) {
        RootNote rootNote(q.value(0).toUInt(),
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toDateTime(),
            q.value(4).toDateTime());
        return rootNote;
    } else {
        return std::nullopt;
    }
}

bool RootNote::isRoot() const
{
    return true;
}

Id RootNote::getParentId() const
{
    throw RootUnsupportedActionException("Root note can't have parent");
}

void RootNote::setParentId(const Id& value)
{
    Q_UNUSED(value)
    throw RootUnsupportedActionException("Root note can't have parent");
}

void RootNote::create() const
{
    throw RootUnsupportedActionException("Root note can have only one instance");
}

void RootNote::update() const
{
    QSqlQuery q;
    q.prepare("UPDATE notes "
              "SET title = :title, "
              "content = :content, "
              "created_at = :created_at, "
              "updated_at = :updated_at "
              "WHERE id = :id");
    q.bindValue(":id", id);
    q.bindValue(":title", title);
    q.bindValue(":content", content);
    q.bindValue(":created_at", createdAt.toString());
    q.bindValue(":updated_at", updatedAt.toString());
    Database::safeExecPreparedQuery(q);
}

void RootNote::remove() const
{
    throw RootUnsupportedActionException("Root note can not be removed");
}
