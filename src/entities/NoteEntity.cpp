#include "NoteEntity.h"

NoteEntity::NoteEntity(QObject* parent)
    : Entity(parent)
{

}

QString NoteEntity::getTitle() const
{
    return title;
}

void NoteEntity::setTitle(const QString& value)
{
    title = value;
}

QString NoteEntity::getContent() const
{
    return content;
}

void NoteEntity::setContent(const QString& value)
{
    content = value;
}

QDateTime NoteEntity::getcreatedAt() const
{
    return createdAt;
}

void NoteEntity::setcreatedAt(const QDateTime& value)
{
    createdAt = value;
}

QDateTime NoteEntity::getmodifiedAt() const
{
    return modifiedAt;
}

void NoteEntity::setmodifiedAt(const QDateTime& value)
{
    modifiedAt = value;
}
