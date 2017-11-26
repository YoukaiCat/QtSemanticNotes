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
