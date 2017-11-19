#ifndef ALIASESMANAGER_H
#define ALIASESMANAGER_H

#include <QObject>

class AliasesManager : public QObject
{
    Q_OBJECT
public:
    explicit AliasesManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // ALIASESMANAGER_H
