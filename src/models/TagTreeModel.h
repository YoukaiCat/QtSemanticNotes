#ifndef TAGTREEMODEL_H
#define TAGTREEMODEL_H

#include "../trees/TagItem.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include <memory>
using std::unique_ptr;

class TagTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TagTreeModel(unique_ptr<TagItem> && rootItem, QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QStringList mimeTypes() const override;

    QMimeData* mimeData(const QModelIndexList& indexes) const override;

    TagItem* itemFromIndex(const QModelIndex& index) const;
    QString tagFromIndex(const QModelIndex& index) const;

    void deleteTagAtIndex(const QModelIndex& index);

private:
    unique_ptr<TagItem> rootItem;
};


#endif // TAGTREEMODEL_H
