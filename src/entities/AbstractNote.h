#ifndef ABSTRACTNOTE_H
#define ABSTRACTNOTE_H

#include <QString>
#include <QDateTime>

typedef quint32 Id;

class AbstractNote
{
public:
    explicit AbstractNote();
    virtual ~AbstractNote();

//    explicit BaseNote();
//    inline BaseNote(const QString& title, const QString& content)
//        : title(title),
//          content(content)
//    {}
//    virtual ~BaseNote() {}

//    inline BaseNote(BaseNote&& other) noexcept
//        : title(move(other.title)),
//          content(move(other.content))
//    {}

//    inline BaseNote & operator=(BaseNote&& other) noexcept
//    {
//        title = move(other.title);
//        content = move(other.content);
//        return *this;
//    }

//    AbstractNote(const AbstractNote&) = delete;
//    AbstractNote & operator=(const AbstractNote&) = delete;

    virtual Id getId() const = 0;

    virtual QString getTitle() const = 0;
    virtual void setTitle(const QString& title) = 0;

    virtual QString getContent() = 0;
    virtual void setContent(const QString& content) = 0;

    virtual QDateTime getCreatedAt() const = 0;
    virtual QDateTime getUpdatedAt() const = 0;

    virtual QString toString() const = 0;
};

#endif // BASENOTE_H
