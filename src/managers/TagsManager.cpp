#include "TagsManager.h"

#include "../entities/Tag.h"

using std::make_unique;

TagsManager::TagsManager(QTreeView * view, QObject* parent)
    : QObject(parent),
      view(view)
{}

void TagsManager::setup()
{
    //TODO Preferences for tag part separator
    QString tagPartSeparator(".");
    auto rootItem = createTree(tagPartSeparator);
    noteTagModel = createTreeModel(move(rootItem));
    setupTreeView(*view, noteTagModel.get());
}

void TagsManager::addChildren(TagItem& rootItem,
                              const vector<shared_ptr<Tag>>& tags,
                              const QString& tagPartSeparator)
{
    for(auto& tag : tags) {
        QString name = tag->getName();
        QStringList words = name.split(tagPartSeparator);
        rootItem.insert(words);
    }
}

unique_ptr<TagItem> TagsManager::createTree(const QString& tagPartSeparator)
{
    auto rootItem = make_unique<TagItem>(TagItem());
    auto tags = Tag::getAll();
    addChildren(*rootItem, tags, tagPartSeparator);
    return rootItem;
}

unique_ptr<TagTreeModel> TagsManager::createTreeModel(unique_ptr<TagItem> && rootItem)
{
    return make_unique<TagTreeModel>(rootItem); //bug
}

void TagsManager::setupTreeView(QTreeView& view, TagTreeModel* model)
{
    view.setModel(model);
}
