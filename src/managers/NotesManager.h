#ifndef NOTESMANAGER_H
#define NOTESMANAGER_H

#include "../entities/RootNote.h"
#include "../models/NoteTreeModel.h"
#include "TabsManager.h"

#include <QTreeView>
#include <QTextDocument>

class NotesManager : public QObject
{
    Q_OBJECT
public:
    explicit NotesManager(QObject* parent = nullptr);
    optional<QModelIndex> findIndexByNoteId(Id id);
    shared_ptr<Note> noteByIndex(const QModelIndex& index);

    shared_ptr<Note> selectedNote();

signals:
    void noteAdded(shared_ptr<Note> note);
    void noteRemoved(shared_ptr<Note> note);
    void noteOpened(shared_ptr<Note> note);
    void noteClosed();
    void noteEdited(shared_ptr<Note> note);
    void noteSaved();
    void noteRenamed(shared_ptr<Note> note);
    void noteSelected();
    void noteNotSelected();

    void noteModified();

public slots:
    void setup(QTreeView* view, TabsManager* tabsManager);

    void selectNoteInTree(shared_ptr<Note> note);
    void selectNoteInTreeById(Id id);
    void selectNoteInTreeByIndex(QModelIndex index);

    void addNote(const QString& title);
    void addSubnote(const QString& title, const QModelIndex& index);
    void addSubnoteToSelected(const QString& title);
    void removeNote(const QModelIndex& index);
    void removeSelectedNote();
    void openNote(shared_ptr<Note> note);
    void openNote(const QModelIndex& index);
    void openNote(const QUrl &url);
    void openSelectedNote();
    void closeNote();
    void saveNote();
    void renameNote(const QString& title, const QModelIndex& index);
    void renameSelectedNote(const QString& title);

    void switchMode();

private slots:
    void onSelectionChange();

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

    unique_ptr<NoteTreeModel> model;
    QTreeView* view;

    TabsManager* tabsManager;
};

#endif // NOTESMANAGER_H