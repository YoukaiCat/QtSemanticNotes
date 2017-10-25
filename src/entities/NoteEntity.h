#ifndef NOTEENTITY_H
#define NOTEENTITY_H

#include "Entity.h"

#include "QDateTime"

class NoteEntity : public Entity
{
public:
    NoteEntity(QObject* parent = nullptr);

    QString getTitle() const;
    void setTitle(const QString& value);

    QString getContent() const;
    void setContent(const QString& value);

    QDateTime getcreatedAt() const;
    void setcreatedAt(const QDateTime& value);

    QDateTime getmodifiedAt() const;
    void setmodifiedAt(const QDateTime& value);

private:
    QString title;
    QString content;
    QDateTime createdAt;
    QDateTime modifiedAt;
};

#endif // NOTEENTITY_H
