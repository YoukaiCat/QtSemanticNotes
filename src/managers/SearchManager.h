#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>

class SearchManager : public QObject
{
    Q_OBJECT
public:
    explicit SearchManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SEARCHMANAGER_H