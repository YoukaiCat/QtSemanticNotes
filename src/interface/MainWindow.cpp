#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QHash>

#include <QSqlQuery>
#include <QSqlRecord>

#include <QMessageBox>

#include <QDebug>

#include <memory>
using std::make_unique;

#include "../database/Database.h"

#include "../entities/AbstractNote.h"
#include "../entities/Note.h"
#include "../entities/RootNote.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Database db;
    //db.openDatabase("/home/parsee/Projects/Active/QtSemanticNotes/test.sqlite");
    db.openDatabase(":memory:");

    rootNote = RootNote::getRootNote();

    headerItem = new NoteTreeItem();

    rootItem = new NoteTreeItem(rootNote.get(), headerItem);

    idToTreeItem.insert(rootNote->getId(), rootItem);

    notes = Note::getAll();

    for(auto& note : notes) {
        NoteTreeItem* item = new NoteTreeItem(note.get());
        idToTreeItem.insert(note->getId(), item);
    }

    for(auto& note : notes) {
        NoteTreeItem* item = idToTreeItem[note->getId()];
        NoteTreeItem* parent = idToTreeItem[note->getParentId()];
        parent->addSubnote(item);
    }

    noteTreeModel = make_unique<NoteTreeModel>(headerItem);
    ui->treeViewNotes->setModel(noteTreeModel.get());

    notesContextMenu.addAction("Open", [this](){
        ui->textBrowserNoteContent->setText(selectedItem->getNote()->getContent());
    });
    notesContextMenu.addAction("Open in new tab", [this](){
        ui->textBrowserNoteContent->setText(selectedItem->getNote()->getContent());
    });
    notesContextMenu.addAction("Add Subnote", [](){

    });
    notesContextMenu.addAction("Rename", [](){

    });
    notesContextMenu.addSeparator();
    notesContextMenu.addAction("Delete", [](){

    });

    notesRootContextMenu.addAction("Open", [](){

    });
    notesRootContextMenu.addAction("Open in new tab", [](){

    });
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
    QModelIndex index = ui->treeViewNotes->indexAt(point);

    if (!index.isValid())
        return;

    auto globalPoint = ui->treeViewNotes->viewport()->mapToGlobal(point);
    selectedItem = noteTreeModel->itemFromIndex(index);
    if (selectedItem == rootItem) {
        notesRootContextMenu.exec(globalPoint);
    } else {
        notesContextMenu.exec(globalPoint);
    }
}
