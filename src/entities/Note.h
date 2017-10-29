#ifndef NOTE_H
#define NOTE_H

#include "AbstractNote.h"

#include <optional>

class Note : public AbstractNote
{
public:
    explicit Note();
    Note(const Id& id, const QString& title, const QString& content,
         const QDateTime& createdAt, const QDateTime& updatedAt, const Id& parentId);

    static QList<Note> getAll();
    static std::optional<Note> getById(Id id);

    virtual bool isRoot() const override;

    virtual Id getParentId() const override;
    virtual void setParentId(const Id& value) override;

    virtual void create() const override;
    virtual void update() const override;
    virtual void remove() const override;

private:
    Id parentId;
};

#endif // NOTE_H
