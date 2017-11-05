#ifndef DATABASE_H
#define DATABASE_H

#include <QException>
#include <QString>
#include <QSqlError>

#include <QDebug>

class OpenDBException : public QException
{
public:
    OpenDBException(const QString& message);
    OpenDBException(const QSqlError& error);
    void raise() const;
    OpenDBException* clone() const;
    QString getMessage() const;

private:
    QString message;
};

class QueryException : public QException
{
public:
    QueryException(const QString& message);
    QueryException(const QSqlError& error);
    void raise() const;
    QueryException* clone() const;
    QString getMessage() const;

private:
    QString message;
};

#include <QSqlQuery>

class Database
{
public:
    static void openDatabase(const QString& path);
    static void closeDatabase();
    static bool isOpen();

    inline static void safeExecQuery(const QString& query)
    {
        static QSqlQuery q;
        if(!q.exec(query)) {
            QString m("Query error:\n");
            m.append(q.lastQuery());
            m.append(q.lastError().text());
            throw QueryException(m);
        }
    }

    inline static void safeExecPreparedQuery(QSqlQuery& query)
    {
        if(!query.exec()) {
            QString m("Query error:\n");
            m.append(query.lastQuery());
            m.append(query.lastError().text());
            qDebug() << query.lastQuery();
            qDebug() << query.lastError().text();
            throw QueryException(m);
        }
    }

private:
    static void defineSchema();
};

#endif // DATABASE_H
