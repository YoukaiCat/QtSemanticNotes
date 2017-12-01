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
