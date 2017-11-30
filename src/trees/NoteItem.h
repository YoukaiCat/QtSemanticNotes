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
 ** along with QtSemanticNotesr. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#ifndef NOTEITEM_H
#define NOTEITEM_H

#include "../entities/Note.h"

#include <QList>

using std::unique_ptr;

class NoteItem
{
public:
    explicit NoteItem(shared_ptr<Note> note);
    ~NoteItem();

    void addChild(NoteItem* item);
    void addChildAndUpdateNoteParent(NoteItem* item);

    int childNumber() const;
    int childCount() const;

    NoteItem* getChild(const int& index) const;
    NoteItem* getParent() const;

    void removeChild(const int& index);

    shared_ptr<Note> getValue() const;

private:
    shared_ptr<Note> value;
    QList<NoteItem*> children;
    NoteItem* parent;
};

#endif // NOTEITEM_H
