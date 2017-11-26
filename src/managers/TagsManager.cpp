#include "TagsManager.h"

#include "../entities/Tag.h"

using std::make_unique;

TagsManager::TagsManager(QObject* parent)
    : QObject(parent)
{}

void TagsManager::setup(QTreeView * view)
{
    this->view = view;

    loadTags();

    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &TagsManager::onSelectionChange);
}

void TagsManager::removeTag(const QModelIndex& index)
{
    QString fulltag = model->tagFromIndex(index);
    Tag::deleteTagAndSubtags(fulltag);
    model->deleteTagAtIndex(index);
    emit tagRemoved();
}

void TagsManager::removeSelectedTag()
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        removeTag(indexes.first());
    }
}

void TagsManager::requestSearchByTag(const QModelIndex& index)
{
    QString fulltag = model->tagFromIndex(index);
    emit searchByTagRequested(fulltag);
}

void TagsManager::requestSearchBySelectedTag()
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        requestSearchByTag(indexes.first());
    }
}

void TagsManager::loadTags()
{
    //TODO Preferences for tag part separator
    QString tagPartSeparator(".");
    auto rootItem = createTree(tagPartSeparator);
    model = createTreeModel(move(rootItem));
    setupTreeView(*view, model.get());
}

void TagsManager::onSelectionChange()
{
    if(view->selectionModel()->selectedIndexes().size() > 0) {
        emit tagSelected();
    } else {
        emit tagNotSelected();
    }
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
    return make_unique<TagTreeModel>(move(rootItem));
}

void TagsManager::setupTreeView(QTreeView& view, TagTreeModel* model)
{
    view.setModel(model);
}
