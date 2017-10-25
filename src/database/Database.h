#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject* parent = nullptr)
        : QObject(parent) {}

    virtual void openDatabase(const QString& path) = 0;
    virtual void closeDatabase() = 0;
    virtual bool isOpen() = 0;

signals:
    void databaseOpened(const QString name);
    void databaseClosed();

public slots:
};

#endif // DATABASE_H
