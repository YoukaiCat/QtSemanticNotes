#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../database/Database.h"

#include "../entities/AbstractNote.h"
#include "../entities/Note.h"
#include "../entities/RootNote.h"

#include <QHash>

#include <QSqlQuery>
#include <QSqlRecord>

#include <QMessageBox>

#include <QDebug>

#include <memory>
using std::make_unique;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Database db;
    //db.openDatabase("/home/parsee/Projects/Active/QtSemanticNotes/test.sqlite");
    db.openDatabase(":memory:");

    rootNote = RootNote::getRootNote();

    headerItem = new NoteItem();

    rootItem = new NoteItem(rootNote.get(), headerItem);

    idToItem.insert(rootNote->getId(), rootItem);

    notes = Note::getAll();

    for(auto& note : notes) {
        NoteItem* item = new NoteItem(note.get());
        idToItem.insert(note->getId(), item);
    }

    for(auto& note : notes) {
        NoteItem* item = idToItem[note->getId()];
        NoteItem* parent = idToItem[note->getParentId()];
        parent->addChild(item);
    }

    noteTreeModel = make_unique<NoteTreeModel>(headerItem);
    ui->treeViewNotes->setModel(noteTreeModel.get());

    notesContextMenu.addAction("Open", [this](){
        Note* note = selectedItem->getAsNote();
        ui->textBrowserNoteContent->setText(note->getContent());
    });
    notesContextMenu.addAction("Open in new tab", [](){});
    notesContextMenu.addAction("Add Subnote", [](){
        //auto note = Note::create();
        //noteTreeModel->addSubnoteAtIndex(, selectedIndex);
    });
    notesContextMenu.addAction("Rename", [](){
        //noteTreeModel->renameNoteAtIndex(, selectedIndex);
    });
    notesContextMenu.addSeparator();
    notesContextMenu.addAction("Delete", [this](){
        noteTreeModel->deleteNoteAtIndex(selectedIndex);
    });

    notesRootContextMenu.addAction("Open", [this](){
        RootNote* note = selectedItem->getAsRoot();
        ui->textBrowserNoteContent->setText(note->getContent());
    });
    notesRootContextMenu.addAction("Open in new tab", [](){});
    notesRootContextMenu.addAction("Add Subnote", [](){

    });
    notesRootContextMenu.addAction("Rename", [](){

    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAboutQt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_treeViewNotes_customContextMenuRequested(const QPoint& point)
{
    selectedIndex = ui->treeViewNotes->indexAt(point);

    if (!selectedIndex.isValid())
        return;

    auto globalPoint = ui->treeViewNotes->viewport()->mapToGlobal(point);

    selectedItem = noteTreeModel->itemFromIndex(selectedIndex);
    if (selectedItem == rootItem) {
        notesRootContextMenu.exec(globalPoint);
    } else {
        notesContextMenu.exec(globalPoint);
    }
}
