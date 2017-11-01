#ifndef ROOTNOTE_H
#define ROOTNOTE_H

#include "AbstractNote.h"

#include <optional>
#include <memory>
using std::optional;
using std::unique_ptr;
using std::move;

class RootNote : public AbstractNote
{
public:
    inline RootNote(RootNote&& other) noexcept
        : id(move(other.id)),
          title(move(other.title)),
          content(move(other.content)),
          createdAt(move(other.createdAt)),
          updatedAt(move(other.updatedAt))
    {}

    inline RootNote & operator=(RootNote&& other) noexcept
    {
        id = move(other.id);
        title = move(other.title);
        content = move(other.content);
        createdAt = move(other.createdAt);
        updatedAt = move(other.updatedAt);
        return *this;
    }

    RootNote(const RootNote&) = delete;
    RootNote & operator=(const RootNote&) = delete;

    static unique_ptr<RootNote> getRootNote();

    virtual Id getId() const override;

    virtual QString getTitle() const override;
    virtual void setTitle(const QString& title) override;

    virtual QString getContent() override;
    virtual void setContent(const QString& content) override;

    virtual QDateTime getCreatedAt() const override;
    virtual QDateTime getUpdatedAt() const override;

    virtual QString toString() const override;

    void update();

private:
    Id id;
    QString title;
    optional<QString> content;
    QDateTime createdAt;
    QDateTime updatedAt;

    RootNote(const Id& id,
             const QString& title,
             const QDateTime& createdAt,
             const QDateTime& updatedAt);
};

#endif // ROOTNOTE_H
