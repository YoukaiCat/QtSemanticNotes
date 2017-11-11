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
    static unique_ptr<Tag> create(const QString& name);
    static unique_ptr<Tag> getOrCreate(const QString& name);
    static vector<unique_ptr<Tag>> getAll();
    static optional<unique_ptr<Tag>> getById(const Id& id);
    static optional<unique_ptr<Tag>> getByName(const QString& name);

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

    static void addNoteTags(const Note* note, const Tag* tag);
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
