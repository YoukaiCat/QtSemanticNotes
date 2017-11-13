#include "NoteTreeModel.h"

#include <QMimeData>
#include <QDataStream>

NoteTreeModel::NoteTreeModel(NoteItem* rootItem, QObject* parent)
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
    return 1;
}

QVariant NoteTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
        return itemFromIndex(index)->getValue()->getTitle();

    if (role == Qt::UserRole)
        return itemFromIndex(index)->getValue()->getId();

    return QVariant();
}

Qt::ItemFlags NoteTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsDragEnabled |
           Qt::ItemIsDropEnabled |
           QAbstractItemModel::flags(index);
}

QVariant NoteTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Title";

    return QVariant();
}

QModelIndex NoteTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    NoteItem* parentItem;

    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = itemFromIndex(parent);
    }

    NoteItem* childItem = parentItem->getChild(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex NoteTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    NoteItem* childItem = itemFromIndex(index);
    NoteItem* parentItem = childItem->getParent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int NoteTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    NoteItem* parentItem;
    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = itemFromIndex(parent);
    }

    return parentItem->childCount();
}

Qt::DropActions NoteTreeModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

QStringList NoteTreeModel::mimeTypes() const
{
    QStringList types;
    types << "application/note";
    return types;
}

QMimeData* NoteTreeModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for(const QModelIndex& index : indexes) {
        if (index.isValid()) {
            NoteItem* item = itemFromIndex(index);
            quintptr intptr = reinterpret_cast<quintptr>(item);
            stream << intptr;
        }
    }

    mimeData->setData("application/note", encodedData);
    return mimeData;
}

bool NoteTreeModel::canDropMimeData(const QMimeData* data,
                                    Qt::DropAction action,
                                    int row, int column,
                                    const QModelIndex& parentIndex) const
{
    Q_UNUSED(row)
    Q_UNUSED(column)

    if (action != Qt::MoveAction)
        return false;

    if (!data->hasFormat("application/note"))
        return false;

    if (!parentIndex.isValid())
        return false;

    QByteArray encodedData = data->data("application/note");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        quintptr intptr;
        stream >> intptr;

        NoteItem* item = reinterpret_cast<NoteItem*>(intptr);
        NoteItem* parent = itemFromIndex(parentIndex);

        if (item->getParent() == parent)
            return false;
    }

    return true;
}

bool NoteTreeModel::dropMimeData(const QMimeData* data,
                                 Qt::DropAction action,
                                 int row, int column,
                                 const QModelIndex& parentIndex)
{
    Q_UNUSED(action)
    Q_UNUSED(row)
    Q_UNUSED(column)

    QByteArray encodedData = data->data("application/note");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        quintptr intptr;
        stream >> intptr;

        NoteItem* item = reinterpret_cast<NoteItem*>(intptr);
        NoteItem* parent = itemFromIndex(parentIndex);

        int oldRow = item->childNumber();
        NoteItem* oldParent = item->getParent();
        QModelIndex oldParentIndex = createIndex(0, 0, oldParent);
        beginRemoveRows(oldParentIndex, oldRow, oldRow);
            oldParent->removeChild(oldRow);
        endRemoveRows();

        int newRow = parent->childCount();
        beginInsertRows(parentIndex, newRow, newRow);
            parent->addChildAndUpdateNoteParent(item);
        endInsertRows();
    }

    return true;
}

inline NoteItem* NoteTreeModel::itemFromIndex(const QModelIndex& index) const
{
    return static_cast<NoteItem*>(index.internalPointer());
}

void NoteTreeModel::renameNoteAtIndex(const QString& title, const QModelIndex& index)
{
    if (!index.isValid())
        return;

    NoteItem* item = itemFromIndex(index);
    Note* note = item->getValue();
    note->setTitle(title);
    note->update();

    emit dataChanged(index, index, QVector<int>());
}

void NoteTreeModel::addSubnoteAtIndex(Note* note, const QModelIndex& parentIndex)
{
    if (!parentIndex.isValid())
        return;

    NoteItem* noteItem = new NoteItem(note);
    NoteItem* parentItem = itemFromIndex(parentIndex);

    int newRow = parentItem->childCount();
    beginInsertRows(parentIndex, newRow, newRow);
        parentItem->addChildAndUpdateNoteParent(noteItem);
    endInsertRows();
}

void NoteTreeModel::deleteNoteAtIndex(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    NoteItem* item = itemFromIndex(index);

    NoteItem* parent = item->getParent();
    int row = item->childNumber();
    beginRemoveRows(index.parent(), row, row);
        parent->removeChild(row);
    endRemoveRows();

    //Subnotes should be removed by sql trigger
    item->getValue()->remove();
    delete item;
}

optional<QModelIndex> NoteTreeModel::findIndexByNoteId(const Id& id, const QModelIndex& rootIndex)
{
    QModelIndexList indexes = match(rootIndex, Qt::UserRole, QVariant(id), 1,
                                    Qt::MatchFlags(Qt::MatchExactly | Qt::MatchRecursive));
    if(indexes.isEmpty()) {
        return {};
    } else {
        return indexes.first();
    }
}
