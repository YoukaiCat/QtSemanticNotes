#include "History.h"

History::History(QObject* parent)
    : QObject(parent)
{}

void History::goForward()
{
    emit noteOpenedFromHistory(forward());
}

void History::goBackward()
{
    emit noteOpenedFromHistory(backward());
}

shared_ptr<Note> History::forward()
{
    auto note = forwardStack.takeFirst();
    if (forwardStack.isEmpty()) {
        emit forwardAwailable(false);
    }
    backwardStack.push_front(current);
    current = note;
    emit backwardAwailable(true);
    return note;
}

shared_ptr<Note> History::backward()
{
    auto note = backwardStack.takeFirst();
    if (backwardStack.isEmpty()) {
        emit backwardAwailable(false);
    }
    forwardStack.push_front(current);
    current = note;
    emit forwardAwailable(true);
    return note;
}

void History::noteOpened(shared_ptr<Note> note)
{
    if (current.get() == nullptr) {
        current = note;
    } else {
        if (current != note) {
            if (!forwardStack.isEmpty() && forwardStack.first() != note){
                clearForwardHistory();
            }
            backwardStack.push_front(current);
            emit backwardAwailable(true);
            current = note;
        }
    }
}

void History::noteRemoved(shared_ptr<Note> note)
{
    backwardStack.removeAll(note);
    forwardStack.removeAll(note);
    if (backwardStack.isEmpty()) {
        emit backwardAwailable(false);
    }
    if (forwardStack.isEmpty()) {
        emit forwardAwailable(false);
    }
}

void History::clearForwardHistory()
{
    forwardStack.clear();
    emit forwardAwailable(false);
}

void History::clearHistory()
{
    backwardStack.clear();
    forwardStack.clear();
    emit backwardAwailable(false);
    emit forwardAwailable(false);
}
