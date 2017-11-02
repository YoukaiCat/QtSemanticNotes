#ifndef ABSTRACTNOTE_H
#define ABSTRACTNOTE_H

#include <QString>
#include <QDateTime>

typedef uint Id;

class AbstractNote
{
public:
    explicit AbstractNote();
    virtual ~AbstractNote();

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
