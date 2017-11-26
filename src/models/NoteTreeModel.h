#ifndef NOTENoteTreeModel_H
#define NOTENoteTreeModel_H

#include "../trees/NoteItem.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class NoteTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    NoteTreeModel(unique_ptr<NoteItem> && rootItem, QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    Qt::DropActions supportedDropActions() const override;

    QStringList mimeTypes() const override;

    QMimeData* mimeData(const QModelIndexList& indexes) const override;

    bool canDropMimeData(const QMimeData* data,
                         Qt::DropAction action,
                         int row, int column,
                         const QModelIndex& parent) const override;

    bool dropMimeData(const QMimeData* data,
                      Qt::DropAction action,
                      int row, int column,
                      const QModelIndex& parent) override;

    NoteItem* itemFromIndex(const QModelIndex& index) const;
    shared_ptr<Note> noteFromIndex(const QModelIndex& index) const;

    optional<shared_ptr<Note>> renameNoteAtIndex(const QString& title, const QModelIndex& index);
    optional<shared_ptr<Note>> addSubnoteAtIndex(const QString& title, const QModelIndex& parentIndex);
    void deleteNoteAtIndex(const QModelIndex& index);

    optional<QModelIndex> findIndexByNoteId(const Id& id, const QModelIndex& rootIndex);

private:
    unique_ptr<NoteItem> rootItem;
};


#endif // NOTENoteTreeModel_H
