#ifndef ROOTNOTE_H
#define ROOTNOTE_H

#include "Note.h"

class RootNote : public Note
{
public:
    static unique_ptr<RootNote> getRootNote();

    virtual Id getParentId() const override;
    virtual void setParentId(const Id& value) override;

    virtual QString toString() const override;

    void update() override;
    void remove() override;

    RootNote(const RootNote&) = delete;
    RootNote & operator=(const RootNote&) = delete;
    RootNote(RootNote&& other) noexcept = delete;
    RootNote & operator=(RootNote&& other) noexcept = delete;

private:
    RootNote(const Id& id,
             const QString& title,
             const QDateTime& createdAt,
             const QDateTime& updatedAt);
};

#endif // ROOTNOTE_H
