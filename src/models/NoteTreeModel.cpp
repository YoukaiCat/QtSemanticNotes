#include "NoteTreeModel.h"
#include "../trees/TreeItem.h"

#include <QStringList>

NoteTreeModel::NoteTreeModel(TreeItem* rootItem, QObject* parent)
    : QAbstractItemModel(parent),
      rootItem(rootItem)
{}

NoteTreeModel::~NoteTreeModel()
{
    delete rootItem;
}

int NoteTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return rootItem->columnCount();
}

QVariant NoteTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TreeItem* item = getItem(index);

    return item->data(index.column());
}

Qt::ItemFlags NoteTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

TreeItem* NoteTreeModel::getItem(const QModelIndex& index) const
{
    if (index.isValid()) {
        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

QVariant NoteTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex NoteTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem* parentItem = getItem(parent);

    TreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool NoteTreeModel::insertColumns(int position, int columns, const QModelIndex& parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool NoteTreeModel::removeColumns(int position, int columns, const QModelIndex& parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool NoteTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem* parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

bool NoteTreeModel::removeRows(int position, int rows, const QModelIndex& parent)
{
    TreeItem* parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

QModelIndex NoteTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem* childItem = getItem(index);
    TreeItem* parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int NoteTreeModel::rowCount(const QModelIndex& parent) const
{
    TreeItem* parentItem = getItem(parent);

    return parentItem->childCount();
}

bool NoteTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TreeItem* item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool NoteTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant& value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}
