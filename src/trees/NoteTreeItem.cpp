#include "NoteTreeItem.h"

NoteTreeItem::NoteTreeItem(const QString& value)
{
    header = value;
}

NoteTreeItem::NoteTreeItem(RootNote* value, NoteTreeItem* parent)
{
    note = value;
    parentItem = parent;
    if (parentItem) {
        parent->subnotes.append(this);
    }
}

NoteTreeItem::NoteTreeItem(Note* value, NoteTreeItem* parent)
{
    note = value;
    parentItem = parent;
    if (parentItem) {
        parent->subnotes.append(this);
    }
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
    return "DATA";
}

bool NoteTreeItem::insertChildren(int position, int count, int columns)
{
    Q_UNUSED(columns)
    if (position < 0 || position > subnotes.size())
        return false;

    for (int row = 0; row < count; ++row) {
        //QVariant data(columns);
        if (std::holds_alternative<RootNote*>(note)) {
            NoteTreeItem* item = new NoteTreeItem(std::get<RootNote*>(note), this);
            subnotes.insert(position, item);
        } else {
            NoteTreeItem* item = new NoteTreeItem(std::get<Note*>(note), this);
            subnotes.insert(position, item);
        }
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

    //note = value;
    return true;
}
