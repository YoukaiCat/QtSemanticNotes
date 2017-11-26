#include "NotesManager.h"

#include <QItemSelectionModel>
#include <QDebug>

using std::make_unique;

NotesManager::NotesManager(QObject* parent)
    : QObject(parent)
{}

optional<QModelIndex> NotesManager::findIndexByNoteId(Id id)
{
    return model->findIndexByNoteId(id, view->rootIndex());
}

shared_ptr<Note> NotesManager::noteByIndex(const QModelIndex& index)
{
    return model->noteFromIndex(index);
}

shared_ptr<Note> NotesManager::selectedNote()
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    Q_ASSERT(!indexes.isEmpty());
    return noteByIndex(indexes.first());
}

void NotesManager::setup(QTreeView* view, TabsManager* tabsManager)
{
    this->view = view;
    auto rootItem = createTree();
    model = createTreeModel(move(rootItem));
    setupTreeView(*view, model.get());

    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &NotesManager::onSelectionChange);

    this->tabsManager = tabsManager;

    connect(tabsManager, &TabsManager::modified,
            this, &NotesManager::noteModified);
    connect(tabsManager, &TabsManager::noteOpenedFromHistory,
            this, &NotesManager::selectNoteInTree);
}

void NotesManager::selectNoteInTree(shared_ptr<Note> note)
{
    selectNoteInTreeById(note->getId());
}

void NotesManager::selectNoteInTreeById(Id id)
{
    auto index = findIndexByNoteId(id);
    if(index.has_value()){
        selectNoteInTreeByIndex(index.value());
    }
}

void NotesManager::selectNoteInTreeByIndex(QModelIndex index)
{
    view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
}

void NotesManager::addNote(const QString& title)
{
    addSubnote(title, view->rootIndex());
}

void NotesManager::addSubnote(const QString& title, const QModelIndex& index)
{
    auto note = model->addSubnoteAtIndex(title, index);
    emit noteAdded(note.value());
}

void NotesManager::addSubnoteToSelected(const QString& title)
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        addSubnote(title, indexes.first());
    }
}

void NotesManager::removeNote(const QModelIndex& index)
{
    auto note = noteByIndex(index);
    model->deleteNoteAtIndex(index);
    tabsManager->removeNote(note);
    emit noteRemoved(note);
}

void NotesManager::removeSelectedNote()
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        removeNote(indexes.first());
    }
}

void NotesManager::openNote(shared_ptr<Note> note)
{
    tabsManager->openNote(note);
    emit noteOpened(note);
}

void NotesManager::openNote(const QModelIndex& index)
{
    auto note = model->noteFromIndex(index);
    openNote(note);
}

void NotesManager::openNote(const QUrl& url)
{
    Id id = url.query().toUInt();
    selectNoteInTreeById(id);
}

void NotesManager::openSelectedNote()
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        openNote(indexes.first());
    }
}

void NotesManager::closeNote()
{
    tabsManager->closeNote();
    emit noteClosed();
}

void NotesManager::saveNote()
{
    tabsManager->saveNote();
    emit noteSaved();
}

void NotesManager::renameNote(const QString& title, const QModelIndex& index)
{
    auto note = model->renameNoteAtIndex(title, index);
    Q_ASSERT(note.has_value());
    emit noteRenamed(note.value());
}

void NotesManager::renameSelectedNote(const QString& title)
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        renameNote(title, indexes.first());
    }
}

void NotesManager::switchMode()
{
    tabsManager->switchMode();
}

void NotesManager::onSelectionChange()
{
    if(view->selectionModel()->selectedIndexes().size() > 0) {
        emit noteSelected();
    } else {
        emit noteNotSelected();
    }
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
        parent->addChild(item);
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
    auto firstItem = new NoteItem(rootNote);

    auto notes = Note::getAll();
    makeTree(firstItem, notes);

    rootItem->addChild(firstItem);
    return rootItem;
}

unique_ptr<NoteTreeModel> NotesManager::createTreeModel(unique_ptr<NoteItem> && rootItem)
{
    return make_unique<NoteTreeModel>(move(rootItem));
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
