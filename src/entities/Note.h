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
using std::unique_ptr;
using std::move;

class Note
{
public:
    static unique_ptr<Note> create(const QString& title,
                                   const QString& content,
                                   const Id& parentId);
    static vector<unique_ptr<Note>> getAll();
    static optional<unique_ptr<Note>> getById(const Id& id);
    static bool existWithTitle(const QString& title);

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

    Note(const Note&) = delete;
    Note & operator=(const Note&) = delete;
    Note(Note&& other) noexcept = delete;
    Note & operator=(Note&& other) noexcept = delete;

    static QStringList getPossibleLinks();
    static void addNoteAlias(const Note* note, const QString& alias);

protected:
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
