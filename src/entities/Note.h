#ifndef NOTE_H
#define NOTE_H

#include "AbstractNote.h"

#include <vector>
#include <optional>
#include <memory>
using std::vector;
using std::optional;
using std::unique_ptr;
using std::move;

class Note : public AbstractNote
{
public:
    inline Note(Note&& other) noexcept
        : AbstractNote(),
          id(move(other.id)),
          title(move(other.title)),
          content(move(other.content)),
          createdAt(move(other.createdAt)),
          updatedAt(move(other.updatedAt)),
          parentId(move(other.parentId))
    {}

    inline Note & operator=(Note&& other) noexcept
    {
        id = move(other.id);
        title = move(other.title);
        content = move(other.content);
        createdAt = move(other.createdAt);
        updatedAt = move(other.updatedAt);
        parentId = move(other.parentId);
        return *this;
    }

    Note(const Note&) = delete;
    Note & operator=(const Note&) = delete;

    static vector<unique_ptr<Note>> getAll();
    static optional<unique_ptr<Note>> getById(const Id& id);

    virtual Id getId() const override;

    virtual QString getTitle() const override;
    virtual void setTitle(const QString& title) override;

    virtual QString getContent() override;
    virtual void setContent(const QString& content) override;

    virtual QDateTime getCreatedAt() const override;
    virtual QDateTime getUpdatedAt() const override;

    Id getParentId() const;
    void setParentId(const Id& value);

    virtual QString toString() const override;

    static unique_ptr<Note> create(const QString& title,
                                   const QString& content,
                                   const Id& parentId);
    void update();
    void remove();

private:
    Id id;
    QString title;
    optional<QString> content;
    QDateTime createdAt;
    QDateTime updatedAt;
    Id parentId;

    Note(const Id& id,
         const QString& title,
         const QDateTime& createdAt,
         const QDateTime& updatedAt,
         const Id& parentId);

    Note(const Id& id,
         const QString& title,
         const optional<QString>& content,
         const QDateTime& createdAt,
         const QDateTime& updatedAt,
         const Id& parentId);
};

#endif // NOTE_H
