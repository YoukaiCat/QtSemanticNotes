#ifndef NOTE_H
#define NOTE_H

#include "Id.h"

#include <QString>
#include <QDateTime>

#include <vector>
#include <optional>
#include <memory>
using std::vector;
using std::optional;
using std::shared_ptr;
using std::move;

class Note
{
public:
    static shared_ptr<Note> create(const QString& title,
                                   const QString& content,
                                   const Id& parentId);
    static vector<shared_ptr<Note>> getAll();
    static optional<shared_ptr<Note>> getById(const Id& id);
    static bool existWithTitleOrAlias(const QString& titleOrAlias);

    virtual ~Note() {}

    virtual Id getId() const;

    virtual QString getTitle() const;
    virtual void setTitle(const QString& title);

    virtual QString getContent();
    virtual void setContent(const QString& content);

    virtual QDateTime getCreatedAt() const;
    virtual QDateTime getUpdatedAt() const;

    virtual Id getParentId() const;
    virtual void setParentId(const Id& value);

    virtual QString toString() const;

    virtual void update();
    virtual void remove();

    virtual bool isDeleted();

    Note(const Note&) = delete;
    Note & operator=(const Note&) = delete;
    Note(Note&& other) noexcept = delete;
    Note & operator=(Note&& other) noexcept = delete;

    static void addNoteAlias(const Note* note, const QString& alias);

    static QPair<QHash<QString,Id>,QString> getPossibleLinks();
    static void addNoteLink(const Id& noteFrom, const Id& noteTo);
    static void clearLinksFrom(const Id& noteFrom);
    static void clearLinksTo(const Id& noteTo);

    friend QDebug operator<<(QDebug stream, const Note &note);
    friend QDebug operator<<(QDebug stream, const shared_ptr<Note> note);

protected:
    Id id;
    QString title;
    optional<QString> content;
    QDateTime createdAt;
    QDateTime updatedAt;
    Id parentId;
    bool deleted = false;

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
