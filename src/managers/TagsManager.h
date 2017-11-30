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
 ** along with QtSemanticNotesr. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#ifndef TAGSMANAGER_H
#define TAGSMANAGER_H

#include "../entities/Tag.h"
#include "../trees/TagItem.h"
#include "../models/TagTreeModel.h"

#include <QTreeView>

class TagsManager : public QObject
{
    Q_OBJECT
public:
    explicit TagsManager(QObject* parent = nullptr);

signals:
    void tagRemoved();
    void searchByTagRequested(const QString& fulltag);
    void tagSelected();
    void tagNotSelected();

public slots:
    void setup(QTreeView * view);

    void removeTag(const QModelIndex& index);
    void removeSelectedTag();
    void requestSearchByTag(const QModelIndex& index);
    void requestSearchBySelectedTag();
    void loadTags();

private slots:
    void onSelectionChange();

private:
    void addChildren(TagItem& rootItem,
                     const vector<shared_ptr<Tag>>& tags,
                     const QString& tagPartSeparator);
    unique_ptr<TagItem> createTree(const QString& tagPartSeparator);
    unique_ptr<TagTreeModel> createTreeModel(unique_ptr<TagItem> && rootItem);
    void setupTreeView(QTreeView& view, TagTreeModel* model);

    unique_ptr<TagTreeModel> model;
    QTreeView* view;
};

#endif // TAGSMANAGER_H
