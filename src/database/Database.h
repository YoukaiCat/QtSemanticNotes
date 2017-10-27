#ifndef DATABASE_H
#define DATABASE_H

#include "Database.h"

#include <QSqlQuery>

class Database
{
public:
    explicit Database();
    ~Database();

    static void openDatabase(const QString& path);
    static void closeDatabase();
    static bool isOpen();

    static void safeExecQuery(const QString& query);
    static void safeExecPreparedQuery(QSqlQuery& query);

private:
    static void defineSchema();
};

#endif // DATABASE_H
