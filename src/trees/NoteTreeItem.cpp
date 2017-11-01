#include "NoteTreeItem.h"

NoteTreeItem::NoteTreeItem(const RootNote& value, NoteTreeItem* parent)
{
    parentItem = parent;
    note = value;
}

NoteTreeItem::NoteTreeItem(const Note& value, NoteTreeItem* parent)
{
    parentItem = parent;
    note = value;
}

NoteTreeItem::~NoteTreeItem()
{
    qDeleteAll(subnotes);
}

NoteTreeItem* NoteTreeItem::child(int number)
{
    return subnotes.value(number);
}

int NoteTreeItem::childCount() const
{
    return subnotes.count();
}

int NoteTreeItem::childNumber() const
{
    if (parentItem) {
        return parentItem->subnotes.indexOf(const_cast<NoteTreeItem*>(this));
    } else {
        return 0;
    }
}

int NoteTreeItem::columnCount() const
{
    return 1;
}

QVariant NoteTreeItem::data(int column) const
{
    Q_UNUSED(column)
    return note;
}

bool NoteTreeItem::insertChildren(int position, int count, int columns)
{
    Q_UNUSED(columns)
    if (position < 0 || position > subnotes.size())
        return false;

    for (int row = 0; row < count; ++row) {
        //QVariant data(columns);
        NoteTreeItem* item = new NoteTreeItem(note, this);
        subnotes.insert(position, item);
    }

    return true;
}

bool NoteTreeItem::insertColumns(int position, int columns)
{
    Q_UNUSED(position)
    Q_UNUSED(columns)
//    if (position < 0 || position > 1)
//        return false;

//    for (int column = 0; column < columns; ++column)
//        itemData.insert(position, QVariant());

//    foreach (NoteTree *child, childItems)
//        child->insertColumns(position, columns);

    return false;
}

NoteTreeItem* NoteTreeItem::parent()
{
    return parentItem;
}

bool NoteTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > subnotes.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete subnotes.takeAt(position);

    return true;
}

bool NoteTreeItem::removeColumns(int position, int columns)
{
    Q_UNUSED(position)
    Q_UNUSED(columns)
//    if (position < 0 || position + columns > 1)
//        return false;

//    for (int column = 0; column < columns; ++column)
//        itemData.remove(position);

//    foreach (NoteTree *child, childItems)
//        child->removeColumns(position, columns);

    return false;
}

bool NoteTreeItem::setData(int column, const QVariant& value)
{
    Q_UNUSED(column)
//    if (column < 0 || column >= 1)
//        return false;

    note = value;
    return true;
}
