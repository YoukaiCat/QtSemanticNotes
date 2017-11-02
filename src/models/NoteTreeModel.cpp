#include "NoteTreeModel.h"

#include <QMimeData>
#include <QDataStream>

NoteTreeModel::NoteTreeModel(NoteTreeItem* rootItem, QObject* parent)
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

    if (role != Qt::DisplayRole)
        return QVariant();

    return itemFromIndex(index)->getNote()->getTitle();
}

Qt::ItemFlags NoteTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

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

    NoteTreeItem* parentItem;

    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = itemFromIndex(parent);
    }

    NoteTreeItem* note = parentItem->getSubnote(row);
    if (note) {
        return createIndex(row, column, note);
    } else {
        return QModelIndex();
    }
}

QModelIndex NoteTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    NoteTreeItem* childItem = itemFromIndex(index);
    NoteTreeItem* parentItem = childItem->parentItem;

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->subnoteNumber(), 0, parentItem);
}

int NoteTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    NoteTreeItem* parentItem;
    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = itemFromIndex(parent);
    }

    return parentItem->subnotes.count();
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
            NoteTreeItem* note = itemFromIndex(index);
            quintptr intptr = reinterpret_cast<quintptr>(note);
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

    if (action != Qt::MoveAction)
        return false;

    if (!data->hasFormat("application/note"))
        return false;

    if (column > 0)
        return false;

    if (!parentIndex.isValid())
        return false;

    QByteArray encodedData = data->data("application/note");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        quintptr intptr;
        stream >> intptr;

        NoteTreeItem* note = reinterpret_cast<NoteTreeItem*>(intptr);
        NoteTreeItem* parent = itemFromIndex(parentIndex);

        if (note->parentItem == parent)
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

        NoteTreeItem* note = reinterpret_cast<NoteTreeItem*>(intptr);
        NoteTreeItem* parent = itemFromIndex(parentIndex);

        QModelIndex oldParent = createIndex(0, 0, note->parentItem);
        int oldRow = note->subnoteNumber();
        beginRemoveRows(oldParent, oldRow, oldRow);
            note->parentItem->subnotes.removeAt(oldRow);
        endRemoveRows();

        int newRow = parent->subnotes.count();
        beginInsertRows(parentIndex, newRow, newRow);
            parent->addSubnoteAndUpdateParent(note);
        endInsertRows();
    }

    return true;
}

inline NoteTreeItem* NoteTreeModel::itemFromIndex(const QModelIndex& index) const
{
    return static_cast<NoteTreeItem*>(index.internalPointer());
}

void NoteTreeModel::renameNoteAtIndex(const QString& title, const QModelIndex& index)
{
    if (!index.isValid())
        return;

    NoteTreeItem* item = itemFromIndex(index);
    item->getNote()->setTitle(title);

    emit dataChanged(index, index, QVector<int>());
}

void NoteTreeModel::addSubnoteAtIndex(Note* note, const QModelIndex& parentIndex)
{
    if (!parentIndex.isValid())
        return;

    NoteTreeItem* noteItem = new NoteTreeItem(note);
    NoteTreeItem* parentItem = itemFromIndex(parentIndex);
    int newRow = parentItem->subnotes.count();
    beginInsertRows(parentIndex, newRow, newRow);
        parentItem->addSubnoteAndUpdateParent(noteItem);
    endInsertRows();
}

void NoteTreeModel::deleteNoteAtIndex(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    NoteTreeItem* item = itemFromIndex(index);
    int oldRow = item->subnotes.count();
    beginRemoveRows(index, oldRow, oldRow);
        item->parentItem->subnotes.removeAt(oldRow);
    endRemoveRows();

    item->getAsNote()->remove();
    delete item;
}
