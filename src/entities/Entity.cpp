#include "Entity.h"

Entity::Entity(QObject* parent)
    : QObject(parent) {}

int Entity::getId() const
{
    return Id;
}

void Entity::setId(int value)
{
    Id = value;
}
