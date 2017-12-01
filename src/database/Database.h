/*************************************************************************
 ** Copyright (C) 2017 Vladislav Mileshkin
 **
 ** This file is part of QtSemanticNotes.
 **
 ** QtSemanticNotes is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** QtSemanticNotes is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with QtSemanticNotes. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

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
            qDebug() << q.lastQuery();
            qDebug() << q.lastError().text();
            throw QueryException(m);
        }
    }

    inline static void safeExecPreparedQuery(QSqlQuery& query)
    {
        if(!query.exec()) {
            QString m("Query error:\n");
            m.append(query.lastQuery());
            m.append(query.lastError().text());
            qDebug() << query.executedQuery();
            qDebug() << query.boundValues();
            qDebug() << query.lastQuery();
            qDebug() << query.lastError().text();
            throw QueryException(m);
        }
    }

private:
    static void defineSchema();
};

#endif // DATABASE_H
