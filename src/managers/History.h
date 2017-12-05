/*************************************************************************
 ** Copyright (C) 2017 Vladislav Mileshkin
 **
 ** This file is part of QtSemanticNotes.
 **
 ** QtSemanticNotes is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** QtSemanticNotes is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with QtSemanticNotes. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

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
    void truncate();

    shared_ptr<Note> current;
    QList<shared_ptr<Note>> forwardStack;
    QList<shared_ptr<Note>> backwardStack;
};

#endif // HISTORY_H
