#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include "Database.h"

class SQliteDatabase : public Database
{
public:
    explicit SQliteDatabase(QObject* parent = nullptr);
    ~SQliteDatabase();

    void openDatabase(const QString& path);
    void closeDatabase();
    bool isOpen();

signals:
    void databaseOpened(const QString name);
    void databaseClosed();

public slots:

private:
    void defineSchema();
};

#endif // SQLITEDATABASE_H
