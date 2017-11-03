#include "NoteItem.h"

using std::holds_alternative;
using std::get;

NoteItem::NoteItem(NoteItem* parent)
    : value(monostate()),
      parent(parent)
{}

NoteItem::NoteItem(RootNote* value, NoteItem* parent)
    : value(value),
      parent(parent)
{
    if (parent) parent->children.append(this);
}

NoteItem::NoteItem(Note* value, NoteItem* parent)
    : value(value),
      parent(parent)
{
    if (parent) parent->children.append(this);
}

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
    if (holds_alternative<Note*>(item->value)) {
        uint id = this->getAsAbstractNote()->getId();
        Note* note = item->getAsNote();
        note->setParentId(id);
        note->update();
    }
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

AbstractNote* NoteItem::getAsAbstractNote() const
{
    if (holds_alternative<RootNote*>(value)) {
        return get<RootNote*>(value);
    } else {
        return get<Note*>(value);
    }
}

RootNote* NoteItem::getAsRoot() const
{
    return get<RootNote*>(value);
}

Note* NoteItem::getAsNote() const
{
    return get<Note*>(value);
}
