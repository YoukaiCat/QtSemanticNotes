#ifndef NOTESMANAGER_H
#define NOTESMANAGER_H

#include "../entities/RootNote.h"
#include "../models/NoteTreeModel.h"

#include <QTreeView>

class NotesManager : public QObject
{
    Q_OBJECT
public:
    explicit NotesManager(QTreeView* view, QObject* parent = nullptr);

signals:

public slots:
    void setup();

private:
    void createItemsAndAddToMap(const vector<shared_ptr<Note>>& notes,
                                QHash<Id, NoteItem*>& itemByNoteId);
    void addChildren(const vector<shared_ptr<Note>>& notes,
                     const QHash<Id, NoteItem*>& itemByNoteId);
    void makeTree(NoteItem* rootItem,
                  const vector<shared_ptr<Note>>& notes);
    unique_ptr<NoteItem> createTree();
    unique_ptr<NoteTreeModel> createTreeModel(unique_ptr<NoteItem> && rootItem);
    void setupTreeView(QTreeView& view, NoteTreeModel* model);
    void changeRootIndex(QTreeView& view);

    unique_ptr<NoteTreeModel> noteTreeModel;
    QTreeView* view;
};

#endif // NOTESMANAGER_H
