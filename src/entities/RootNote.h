#ifndef ROOTNOTE_H
#define ROOTNOTE_H

#include <QException>
#include <QString>

class RootUnsupportedActionException : public QException
{
public:
    RootUnsupportedActionException(const QString& message);
    void raise() const;
    RootUnsupportedActionException* clone() const;
    QString getMessage() const;

private:
    QString message;
};

#include "AbstractNote.h"

#include <optional>

class RootNote : public AbstractNote
{
public:
    static std::optional<RootNote> getRoot();

    virtual bool isRoot() const override;

    virtual Id getParentId() const override;
    virtual void setParentId(const Id& value) override;

    virtual void create() const override;
    virtual void update() const override;
    virtual void remove() const override;

private:
    explicit RootNote();
    RootNote(const Id& id, const QString& title, const QString& content,
             const QDateTime& createdAt, const QDateTime& updatedAt);
};

#endif // ROOTNOTE_H
