#ifndef TAG_H
#define TAG_H

#include <QString>
#include <QDateTime>

#include <vector>
#include <optional>
#include <memory>
using std::vector;
using std::optional;
using std::unique_ptr;
using std::move;

typedef uint Id;

class Tag
{
public:
    inline Tag(Tag&& other) noexcept
        : id(move(other.id)),
          name(move(other.name)),
          createdAt(move(other.createdAt)),
          updatedAt(move(other.updatedAt))
    {}

    inline Tag & operator=(Tag&& other) noexcept
    {
        id = move(other.id);
        name = move(other.name);
        createdAt = move(other.createdAt);
        updatedAt = move(other.updatedAt);
        return *this;
    }

    Tag(const Tag&) = delete;
    Tag & operator=(const Tag&) = delete;

    static vector<unique_ptr<Tag>> getAll();
    static optional<unique_ptr<Tag>> getById(const Id& id);

    Id getId() const;

    QString getName() const;
    void setName(const QString& name);

    QDateTime getCreatedAt() const;
    QDateTime getUpdatedAt() const;

    QString toString() const;

    static unique_ptr<Tag> create(const QString& name);

    void update();
    void remove();

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
