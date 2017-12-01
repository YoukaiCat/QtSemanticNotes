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

#include "NoteItem.h"

NoteItem::NoteItem(shared_ptr<Note> value)
    : value(move(value))
{}

NoteItem::~NoteItem()
{
    qDeleteAll(children);
}

void NoteItem::addChild(NoteItem* item)
{
    children.append(item);
    item->parent = this;
}

void NoteItem::addChildAndUpdateNoteParent(NoteItem* item)
{
    addChild(item);
    uint id = this->getValue()->getId();
    auto note = item->getValue();
    note->setParentId(id);
    note->update();
}

int NoteItem::childNumber() const
{
    if (parent) {
        return parent->children.indexOf(const_cast<NoteItem*>(this));
    } else {
        return 0;
    }
}

int NoteItem::childCount() const
{
    return children.count();
}

void NoteItem::removeChild(const int& index)
{
    children.removeAt(index);
}

NoteItem* NoteItem::getChild(const int& index) const
{
    return children.at(index);
}

NoteItem* NoteItem::getParent() const
{
    return parent;
}

shared_ptr<Note> NoteItem::getValue() const
{
    return value;
}
