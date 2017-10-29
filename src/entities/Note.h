#ifndef NOTE_H
#define NOTE_H

#include "AbstractNote.h"

#include <vector>
#include <optional>
using std::vector;
using std::optional;

class Note : public AbstractNote
{
public:
    explicit Note();
    inline Note(const Id& id, const QString& title, const QString& content,
         const QDateTime& createdAt, const QDateTime& updatedAt, const Id& parentId)
        : AbstractNote(id, title, content, createdAt, updatedAt),
          parentId(parentId)
    {}

    inline Note(Note&& other) noexcept
        : AbstractNote(move(other.id),
                       move(other.title),
                       move(other.content),
                       move(other.createdAt),
                       move(other.updatedAt)),
          parentId(other.parentId)
    {}

    inline Note & operator=(Note&& other) noexcept
    {
        AbstractNote::operator=(move(other));
        parentId = move(other.parentId);
        return *this;
    }

    Note(const Note&) = delete;
    Note & operator=(const Note&) = delete;

    static vector<Note> getAll();
    static optional<Note> getById(Id id);

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
