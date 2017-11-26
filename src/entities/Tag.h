#ifndef TAG_H
#define TAG_H

#include "Id.h"
#include "Note.h"

#include <QString>
#include <QDateTime>

#include <vector>
#include <optional>
#include <memory>
using std::vector;
using std::optional;
using std::unique_ptr;
using std::move;

class Tag
{
public:
    static shared_ptr<Tag> create(const QString& name);
    static shared_ptr<Tag> getOrCreate(const QString& name);
    static vector<shared_ptr<Tag>> getAll();
    static optional<shared_ptr<Tag>> getById(const Id& id);
    static optional<shared_ptr<Tag>> getByName(const QString& name);

    Id getId() const;

    QString getName() const;
    void setName(const QString& name);

    QDateTime getCreatedAt() const;

    QString toString() const;

    void remove();

    Tag(const Tag&) = delete;
    Tag & operator=(const Tag&) = delete;
    Tag(Tag&& other) noexcept = delete;
    Tag & operator=(Tag&& other) noexcept = delete;

    static void addNoteTag(const Note* note, const Tag* tag);

    static void deleteTagAndSubtags(const QString& fulltag);

private:
    Tag(const Id& id,
        const QString& name,
        const QDateTime& createdAt);

    Id id;
    QString name;
    QDateTime createdAt;
};

#endif // TAG_H
