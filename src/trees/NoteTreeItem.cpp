#include "NoteTreeItem.h"

using std::holds_alternative;
using std::get;

NoteTreeItem::NoteTreeItem(NoteTreeItem* parent)
    : note(monostate()),
      parentItem(parent)
{}

NoteTreeItem::NoteTreeItem(RootNote* value, NoteTreeItem* parent)
    : note(value),
      parentItem(parent)
{
    if (parentItem) parent->subnotes.append(this);
}

NoteTreeItem::NoteTreeItem(Note* value, NoteTreeItem* parent)
    : note(value),
      parentItem(parent)
{
    if (parentItem) parent->subnotes.append(this);
}

NoteTreeItem::~NoteTreeItem()
{
    qDeleteAll(subnotes);
}

void NoteTreeItem::addSubnote(NoteTreeItem* note)
{
    subnotes.append(note);
    note->parentItem = this;
}

int NoteTreeItem::subnoteNumber() const
{
    if (parentItem) {
        return parentItem->subnotes.indexOf(const_cast<NoteTreeItem*>(this));
    } else {
        return 0;
    }
}

NoteTreeItem* NoteTreeItem::getSubnote(const int& index) const
{
    return subnotes.at(index);
}

QString NoteTreeItem::getNoteTitle() const
{
    if (holds_alternative<RootNote*>(note))
        return get<RootNote*>(note)->getTitle();

    if (holds_alternative<Note*>(note))
        return get<Note*>(note)->getTitle();

    return QString("(monostate)");
}
