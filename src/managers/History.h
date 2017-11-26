#ifndef HISTORY_H
#define HISTORY_H

#include "../entities/Note.h"

#include <QObject>

class History : public QObject
{
    Q_OBJECT
public:
    explicit History(QObject* parent = nullptr);

signals:
    void forwardAwailable(bool);
    void backwardAwailable(bool);
    void noteOpenedFromHistory(shared_ptr<Note> note);

public slots:
    void goForward();
    void goBackward();
    void clearHistory();
    void clearForwardHistory();

protected slots:
    void noteOpened(shared_ptr<Note> note);
    void noteRemoved(shared_ptr<Note> note);

protected:
    shared_ptr<Note> forward();
    shared_ptr<Note> backward();

private:
    shared_ptr<Note> current;
    QList<shared_ptr<Note>> forwardStack;
    QList<shared_ptr<Note>> backwardStack;
};

#endif // HISTORY_H
