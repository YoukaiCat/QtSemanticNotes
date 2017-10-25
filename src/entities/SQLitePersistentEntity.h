#ifndef SQLITEPERSISTENTENTITY_H
#define SQLITEPERSISTENTENTITY_H

#include "PersistentEntity.h"

class SQLitePersistentEntity : public PersistentEntity
{
    Q_OBJECT

public:
    explicit SQLitePersistentEntity(QObject* parent = nullptr)
        : PersistentEntity(parent) {}
signals:

public slots:
};

#endif // SQLITEPERSISTENTENTITY_H
