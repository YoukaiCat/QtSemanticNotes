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
