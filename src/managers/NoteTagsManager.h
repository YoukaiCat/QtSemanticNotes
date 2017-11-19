#ifndef NOTETAGSMANAGER_H
#define NOTETAGSMANAGER_H

#include <QObject>

class NoteTagsManager : public QObject
{
    Q_OBJECT
public:
    explicit NoteTagsManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // NOTETAGSMANAGER_H