#ifndef NOTENoteTreeModel_H
#define NOTENoteTreeModel_H

#include "../trees/NoteTreeItem.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class NoteTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    NoteTreeModel(NoteTreeItem* rootItem, QObject* parent = 0);
    ~NoteTreeModel();

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role = Qt::EditRole) override;

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

private:
    NoteTreeItem* itemFromIndex(const QModelIndex& index) const;

    NoteTreeItem* rootItem;
};


#endif // NOTENoteTreeModel_H
