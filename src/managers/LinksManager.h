#ifndef LINKSMANAGER_H
#define LINKSMANAGER_H

#include <QObject>

class LinksManager : public QObject
{
    Q_OBJECT
public:
    explicit LinksManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LINKSMANAGER_H