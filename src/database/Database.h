#ifndef DATABASE_H
#define DATABASE_H

#include "Database.h"

#include <QObject>
#include <QSqlQuery>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject* parent = nullptr);
    ~Database();

    void openDatabase(const QString& path);
    void closeDatabase();
    bool isOpen();

signals:
    void databaseOpened(const QString& name);
    void databaseClosed();

public slots:

private:
    void defineSchema();
    void safeExecQuery(const QString& query);
    void safeExecPreparedQuery(QSqlQuery& query);
};

#endif // DATABASE_H
