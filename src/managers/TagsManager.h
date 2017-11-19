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
    explicit TagsManager(QTreeView * view, QObject* parent = nullptr);

signals:

public slots:
    void setup();

private:
    void addChildren(TagItem& rootItem,
                     const vector<shared_ptr<Tag>>& tags,
                     const QString& tagPartSeparator);
    unique_ptr<TagItem> createTree(const QString& tagPartSeparator);
    unique_ptr<TagTreeModel> createTreeModel(unique_ptr<TagItem> && rootItem);
    void setupTreeView(QTreeView& view, TagTreeModel* model);

    unique_ptr<TagTreeModel> noteTagModel;
    QTreeView* view;
};

#endif // TAGSMANAGER_H
