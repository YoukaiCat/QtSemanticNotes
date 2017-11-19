#include "NoteManager.h"

using std::make_unique;

NoteManager::NoteManager(QObject* parent)
    : QObject(parent)

{}

void NoteManager::createItemsAndAddToMap(const vector<shared_ptr<Note>> & notes,
                                         QHash<Id, NoteItem*> & itemByNoteId)
{
    for(auto& note : notes) {
        auto item = new NoteItem(note);
        itemByNoteId.insert(note->getId(), item);
    }
}

void NoteManager::setItemChilds(const vector<shared_ptr<Note>> & notes,
                                const QHash<Id, NoteItem*> & itemByNoteId)
{
    for(auto& note : notes) {
        auto noteId = note->getId();
        auto parentId = note->getParentId();
        auto item = make_unique<NoteItem>(itemByNoteId[noteId]);
        auto parent = make_unique<NoteItem>(itemByNoteId[parentId]);
        parent->addChild(move(item));
    }
}

void NoteManager::setupRootChildren(QHash<Id, NoteItem*> & itemByNoteId)
{
    auto notes = Note::getAll();
    createItemsAndAddToMap(notes, itemByNoteId);
    setItemChilds(notes, itemByNoteId);
}

void NoteManager::setupTree()
{
    auto rootNote = RootNote::getRootNote();
    auto rootItem = new NoteItem(rootNote);
    auto firstItem = new NoteItem(rootNote);

    QHash<Id, NoteItem*> itemByNoteId;
    itemByNoteId.insert(rootNote->getId(), firstItem);

    setupRootChildren(itemByNoteId);

    rootItem->addChild(make_unique<NoteItem>(firstItem));
}
