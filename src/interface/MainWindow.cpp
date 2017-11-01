#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QHash>

#include <QSqlQuery>
#include <QSqlRecord>

#include <QMessageBox>

#include <QDebug>

#include "../database/Database.h"

#include "../entities/AbstractNote.h"
#include "../entities/Note.h"
#include "../entities/RootNote.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    try {

        Database db;
        //db.openDatabase("/home/parsee/Projects/Active/QtSemanticNotes/test.sqlite");
        db.openDatabase(":memory:");

        rootNote = RootNote::getRootNote();

        QHash<Id, NoteTreeItem*> idToTreeItem;

        NoteTreeItem* headerItem = new NoteTreeItem("Title");

        NoteTreeItem* rootItem = new NoteTreeItem(rootNote.get(), headerItem);

        idToTreeItem.insert(rootNote->getId(), rootItem);

        auto notes = Note::getAll();

        for(auto& note : notes) {
            NoteTreeItem* item = new NoteTreeItem(note.get());
            idToTreeItem.insert(note->getId(), item);
        }

        for(auto& note : notes) {
            NoteTreeItem* item = idToTreeItem[note->getId()];
            NoteTreeItem* parent = idToTreeItem[note->getParentId()];
            parent->subnotes.append(item);
            item->parentItem = parent;
        }

        noteTreeModel = new NoteTreeModel(headerItem);
        ui->treeViewNotes->setModel(noteTreeModel);
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
    QMenu m;
    m.addAction("hello");
    m.addAction("world");

    QModelIndex index = ui->treeViewNotes->indexAt(point);
    if (index.isValid()) {
        m.exec(ui->treeViewNotes->viewport()->mapToGlobal(point));
    }
}
