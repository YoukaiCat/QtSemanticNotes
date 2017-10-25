#ifndef PERSISTENTENTITY_H
#define PERSISTENTENTITY_H

#include "Entity.h"

class PersistentEntity : public Entity
{
    Q_OBJECT

public:
    explicit PersistentEntity(QObject* parent = nullptr)
        : Entity(parent) {}

signals:

public slots:
};

#endif // PERSISTENTENTITY_H
