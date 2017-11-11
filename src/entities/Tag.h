#ifndef TAG_H
#define TAG_H

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

class Tag
{
public:
    static unique_ptr<Tag> create(const QString& name);
    static vector<unique_ptr<Tag>> getAll();
    static optional<unique_ptr<Tag>> getById(const Id& id);

    Id getId() const;

    QString getName() const;
    void setName(const QString& name);

    QDateTime getCreatedAt() const;
    QDateTime getUpdatedAt() const;

    QString toString() const;

    void remove();

    Tag(const Tag&) = delete;
    Tag & operator=(const Tag&) = delete;
    Tag(Tag&& other) noexcept = delete;
    Tag & operator=(Tag&& other) noexcept = delete;

private:
    Tag(const Id& id,
        const QString& name,
        const QDateTime& createdAt,
        const QDateTime& updatedAt);

    Id id;
    QString name;
    QDateTime createdAt;
    QDateTime updatedAt;
};

#endif // TAG_H
