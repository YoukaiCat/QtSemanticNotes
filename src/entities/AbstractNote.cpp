#include "AbstractNote.h"

AbstractNote::AbstractNote()
{}

Id AbstractNote::getId() const
{
    return id;
}

void AbstractNote::setId(const Id& value)
{
    id = value;
}

QString AbstractNote::getTitle() const
{
    return title;
}

void AbstractNote::setTitle(const QString& value)
{
    title = value;
}

QString AbstractNote::getContent() const
{
    return content;
}

void AbstractNote::setContent(const QString& value)
{
    content = value;
}

QDateTime AbstractNote::getCreatedAt() const
{
    return createdAt;
}

void AbstractNote::setCreatedAt(const QDateTime& value)
{
    createdAt = value;
}

QDateTime AbstractNote::getUpdatedAt() const
{
    return updatedAt;
}

void AbstractNote::setUpdatedAt(const QDateTime& value)
{
    updatedAt = value;
}

QString AbstractNote::toString() const
{
    return QString("id: %1, title: %2, content: %3, created_at: %4, updated_at: %5")
            .arg(QString::number(id))
            .arg(title)
            .arg(content)
            .arg(createdAt.toString())
            .arg(updatedAt.toString());
}
