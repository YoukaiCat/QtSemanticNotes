#ifndef ROOTNOTE_H
#define ROOTNOTE_H

#include "Note.h"

class RootNote : public Note
{
public:
//    inline RootNote(RootNote&& other) noexcept
//        : id(move(other.id)),
//          title(move(other.title)),
//          content(move(other.content)),
//          createdAt(move(other.createdAt)),
//          updatedAt(move(other.updatedAt))
//    {}

//    inline RootNote & operator=(RootNote&& other) noexcept
//    {
//        id = move(other.id);
//        title = move(other.title);
//        content = move(other.content);
//        createdAt = move(other.createdAt);
//        updatedAt = move(other.updatedAt);
//        return *this;
//    }

    RootNote(const RootNote&) = delete;
    RootNote & operator=(const RootNote&) = delete;

    static unique_ptr<RootNote> getRootNote();

    virtual Id getParentId() const override;
    virtual void setParentId(const Id& value) override;

    virtual QString toString() const override;

    void update() override;
    void remove() override;

private:
    RootNote(const Id& id,
             const QString& title,
             const QDateTime& createdAt,
             const QDateTime& updatedAt);
};

#endif // ROOTNOTE_H
