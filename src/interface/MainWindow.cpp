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
#include <QInputDialog>

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

    auto onAddSubnote = [this](){
        bool ok;
        QString title = QInputDialog::getText(this, tr("Add Subnote"),
                                             tr("Title:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !title.isEmpty()) {
            auto note = Note::create(title, "", selectedItem->getAsAbstractNote()->getId());
            Note* noteptr = note.get();
            notes.push_back(std::move(note));
            noteTreeModel->addSubnoteAtIndex(noteptr, selectedIndex);
        }
    };

    auto onRenameNote = [this](){
        bool ok;
        QString title = QInputDialog::getText(this, tr("Rename note"),
                                             tr("New Title:"), QLineEdit::Normal,
                                             selectedItem->getAsAbstractNote()->getTitle(), &ok);
        if (ok && !title.isEmpty()) {
            noteTreeModel->renameNoteAtIndex(title, selectedIndex);
        }
    };

    noteTreeModel = make_unique<NoteTreeModel>(headerItem);
    ui->treeViewNotes->setModel(noteTreeModel.get());

    notesContextMenu.addAction("Open", [this](){
        Note* note = selectedItem->getAsNote();
        ui->textBrowserNoteContent->setText(note->getContent());
    });
    notesContextMenu.addAction("Open in new tab", [](){});
    notesContextMenu.addAction("Add Subnote", onAddSubnote);
    notesContextMenu.addAction("Rename", onRenameNote);
    notesContextMenu.addSeparator();
    notesContextMenu.addAction("Delete", [this](){
        QMessageBox msgBox;
        msgBox.setText("Remove note?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int answer = msgBox.exec();
        if (answer == QMessageBox::Yes) {
            noteTreeModel->deleteNoteAtIndex(selectedIndex);
        }
    });

    notesRootContextMenu.addAction("Open", [this](){
        RootNote* note = selectedItem->getAsRoot();
        ui->textBrowserNoteContent->setText(note->getContent());
    });
    notesRootContextMenu.addAction("Open in new tab", [](){});
    notesRootContextMenu.addAction("Add Subnote", onAddSubnote);
    notesRootContextMenu.addAction("Rename", onRenameNote);
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
