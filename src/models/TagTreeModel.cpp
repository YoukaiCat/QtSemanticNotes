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

#include "TagTreeModel.h"

#include <QMimeData>
#include <QDataStream>

TagTreeModel::TagTreeModel(unique_ptr<TagItem> && rootItem, QObject* parent)
    : QAbstractItemModel(parent),
      rootItem(move(rootItem))
{}

int TagTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant TagTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    return itemFromIndex(index)->getWord();
}

Qt::ItemFlags TagTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsDragEnabled | QAbstractItemModel::flags(index);
}

QVariant TagTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Name";

    return QVariant();
}

QModelIndex TagTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TagItem* parentItem;

    if (!parent.isValid()) {
        parentItem = rootItem.get();
    } else {
        parentItem = itemFromIndex(parent);
    }

    TagItem* childItem = parentItem->getChild(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex TagTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    TagItem* childItem = itemFromIndex(index);
    TagItem* parentItem = childItem->getParent();

    if (parentItem == rootItem.get())
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int TagTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    TagItem* parentItem;
    if (!parent.isValid()) {
        parentItem = rootItem.get();
    } else {
        parentItem = itemFromIndex(parent);
    }

    return parentItem->childCount();
}

QStringList TagTreeModel::mimeTypes() const
{
    QStringList types;
    types << "application/Tag";
    return types;
}

QMimeData* TagTreeModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for(const QModelIndex& index : indexes) {
        if (index.isValid()) {
            TagItem* item = itemFromIndex(index);
            QStringList words;
            QString fullTag = item->getFullTag(words);
            stream << fullTag;
        }
    }

    mimeData->setData("application/Tag", encodedData);
    return mimeData;
}

TagItem* TagTreeModel::itemFromIndex(const QModelIndex& index) const
{
    return static_cast<TagItem*>(index.internalPointer());
}

QString TagTreeModel::tagFromIndex(const QModelIndex& index) const
{
    QStringList words;
    return itemFromIndex(index)->getFullTag(words);
}

void TagTreeModel::deleteTagAtIndex(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    TagItem* item = itemFromIndex(index);

    TagItem* parent = item->getParent();
    int row = item->childNumber();
    beginRemoveRows(index.parent(), row, row);
        parent->removeChild(row);
    endRemoveRows();
    delete item;
}
