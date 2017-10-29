#ifndef ABSTRACTNOTE_H
#define ABSTRACTNOTE_H

#include <QString>
#include <QDateTime>

typedef quint32 Id;

class AbstractNote
{
public:
    explicit AbstractNote();
    AbstractNote(const Id& id, const QString& title, const QString& content,
             const QDateTime& createdAt, const QDateTime& updatedAt);
    virtual ~AbstractNote() {}

    virtual Id getId() const;
    virtual void setId(Id id);

    virtual QString getTitle() const;
    virtual void setTitle(const QString& title);

    virtual QString getContent() const;
    virtual void setContent(const QString& content);

    virtual QDateTime getCreatedAt() const;
    virtual void setCreatedAt(const QDateTime& datetime);

    virtual QDateTime getUpdatedAt() const;
    virtual void setUpdatedAt(const QDateTime& datetime);

    virtual bool isRoot() const = 0;

    virtual Id getParentId() const = 0;
    virtual void setParentId(const Id& value) = 0;

    virtual void create() const = 0;
    virtual void update() const = 0;
    virtual void remove() const = 0;

    virtual QString toString() const;

protected:
    Id id;
    QString title;
    QString content;
    QDateTime createdAt;
    QDateTime updatedAt;
};

#endif // ABSTRACTNOTE_H
