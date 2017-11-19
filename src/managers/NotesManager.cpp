#include "NotesManager.h"

using std::make_unique;

NotesManager::NotesManager(QTreeView* view, QObject* parent)
    : QObject(parent),
      view(view)
{}

void NotesManager::setup()
{
    auto rootItem = createTree();
    noteTreeModel = createTreeModel(move(rootItem));
    setupTreeView(*view, noteTreeModel.get());
}

void NotesManager::createItemsAndAddToMap(const vector<shared_ptr<Note>>& notes,
                                          QHash<Id, NoteItem*>& itemByNoteId)
{
    for(auto& note : notes) {
        auto item = new NoteItem(note);
        itemByNoteId.insert(note->getId(), item);
    }
}

void NotesManager::addChildren(const vector<shared_ptr<Note>>& notes,
                               const QHash<Id, NoteItem*>& itemByNoteId)
{
    for(auto& note : notes) {
        auto noteId = note->getId();
        auto parentId = note->getParentId();
        auto item = itemByNoteId[noteId];
        auto parent = itemByNoteId[parentId];
        parent->addChild(make_unique<NoteItem>(item));
    }
}

void NotesManager::makeTree(NoteItem* rootItem,
                            const vector<shared_ptr<Note>>& notes)
{
    QHash<Id, NoteItem*> itemByNoteId;
    itemByNoteId.insert(rootItem->getValue()->getId(), rootItem);
    createItemsAndAddToMap(notes, itemByNoteId);
    addChildren(notes, itemByNoteId);
}

unique_ptr<NoteItem> NotesManager::createTree()
{
    auto rootNote = RootNote::getRootNote();
    auto rootItem = make_unique<NoteItem>(NoteItem(rootNote));
    auto firstItem = make_unique<NoteItem>(NoteItem(rootNote));

    auto notes = Note::getAll();
    makeTree(firstItem.get(), notes);

    rootItem->addChild(move(firstItem));
    return rootItem;
}

unique_ptr<NoteTreeModel> NotesManager::createTreeModel(unique_ptr<NoteItem> && rootItem)
{
    return make_unique<NoteTreeModel>(rootItem); //bug
}

void NotesManager::setupTreeView(QTreeView& view, NoteTreeModel* model)
{
    view.setModel(model);
    changeRootIndex(view);
}

void NotesManager::changeRootIndex(QTreeView& view)
{
    QModelIndex rootIndex = view.model()->index(0, 0, QModelIndex());
    view.setRootIndex(rootIndex);
}
