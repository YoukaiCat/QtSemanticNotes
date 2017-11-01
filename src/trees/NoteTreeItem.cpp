#include "NoteTreeItem.h"

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

void NoteTreeItem::addSubnote(NoteTreeItem* note)
{
    subnotes.append(note);
    note->parentItem = this;
}

//bool NoteTreeItem::insertChildren(int position, int count, int columns)
//{
//    Q_UNUSED(columns)
//    if (position < 0 || position > subnotes.size())
//        return false;

//    for (int row = 0; row < count; ++row) {
//        if (std::holds_alternative<RootNote*>(note)) {
//            NoteTreeItem* item = new NoteTreeItem(this);
//            subnotes.insert(position, item);
//        } else {
//            NoteTreeItem* item = new NoteTreeItem(this);
//            subnotes.insert(position, item);
//        }
//    }
//    return true;
//}

//bool NoteTreeItem::removeChildren(int position, int count)
//{
//    if (position < 0 || position + count > subnotes.size())
//        return false;

//    for (int row = 0; row < count; ++row)
//        delete subnotes.takeAt(position);

//    return true;
//}

int NoteTreeItem::childNumber() const
{
    if (parentItem) {
        return parentItem->subnotes.indexOf(const_cast<NoteTreeItem*>(this));
    } else {
        return 0;
    }
}
