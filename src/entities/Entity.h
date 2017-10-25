#ifndef ENTITY_H
#define ENTITY_H

#include <QObject>

class Entity : public QObject
{
    Q_OBJECT

public:
    explicit Entity(QObject* parent = nullptr);

    int getId() const;
    void setId(int value);

signals:

public slots:

private:
    int Id;
};

#endif // ENTITY_H
