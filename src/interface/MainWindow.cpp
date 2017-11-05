#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../database/Database.h"

#include "../entities/AbstractNote.h"
#include "../entities/Note.h"
#include "../entities/RootNote.h"

#include <QHash>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>

#include <QMessageBox>
#include <QInputDialog>

#include <QItemSelectionModel>

#include <QDebug>

#include <memory>
using std::make_unique;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Database db;
    db.openDatabase("/home/parsee/Projects/Active/QtSemanticNotes/test.sqlite");
    //db.openDatabase(":memory:");

    rootNote = RootNote::getRootNote();

    noteHeaderItem = new NoteItem();

    rootItem = new NoteItem(rootNote.get(), noteHeaderItem);

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

    noteTreeModel = make_unique<NoteTreeModel>(noteHeaderItem);
    ui->treeViewNotes->setModel(noteTreeModel.get());

    tags = Tag::getAll();

    tagHeaderItem = new TagItem();

    for(auto& tag : tags) {
        QString name = tag->getName();
        QStringList words = name.split(".");
        tagHeaderItem->placeTag(tag.get(), words);
    }

    tagTreeModel = make_unique<TagTreeModel>(tagHeaderItem);
    ui->treeViewTags->setModel(tagTreeModel.get());

    aliasesModel = make_unique<QSqlTableModel>();
    aliasesModel->setTable("aliases");
    aliasesModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    ui->tableViewAliases->setModel(aliasesModel.get());
    ui->tableViewAliases->hideColumn(0);
    ui->tableViewAliases->hideColumn(2);
    ui->tableViewAliases->hideColumn(3);
    ui->tableViewAliases->hideColumn(4);

    connect(ui->tableViewAliases->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            [this](const QItemSelection &selected, const QItemSelection &deselected)
    {
        Q_UNUSED(selected)
        Q_UNUSED(deselected)
        if(ui->tableViewAliases->selectionModel()->selectedIndexes().size() > 0) {
            ui->toolButtonRemoveAlias->setEnabled(true);
        } else {
            ui->toolButtonRemoveAlias->setEnabled(false);
        }
    });

    tagsModel = make_unique<QSqlRelationalTableModel>();
    tagsModel->setTable("note_tags");
    tagsModel->setRelation(2, QSqlRelation("tags", "id", "name"));
    tagsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    ui->tableViewNoteTags->setModel(tagsModel.get());
    ui->tableViewNoteTags->hideColumn(0);
    ui->tableViewNoteTags->hideColumn(1);
    ui->tableViewNoteTags->hideColumn(3);
    ui->tableViewNoteTags->hideColumn(4);

    connect(ui->tableViewNoteTags->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            [this](const QItemSelection &selected, const QItemSelection &deselected)
    {
        Q_UNUSED(selected)
        Q_UNUSED(deselected)
        if(ui->tableViewNoteTags->selectionModel()->selectedIndexes().size() > 0) {
            ui->toolButtonRemoveTag->setEnabled(true);
        } else {
            ui->toolButtonRemoveTag->setEnabled(false);
        }
    });

    linkedFromModel = make_unique<QSqlRelationalTableModel>();
    linkedFromModel->setTable("links");
    linkedFromModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    linkedFromModel->setRelation(1, QSqlRelation("notes", "id", "title"));
    ui->tableViewLinkedFrom->setModel(linkedFromModel.get());
    ui->tableViewLinkedFrom->hideColumn(0);
    ui->tableViewLinkedFrom->hideColumn(2);
    ui->tableViewLinkedFrom->hideColumn(3);
    ui->tableViewLinkedFrom->hideColumn(4);

    linksToModel = make_unique<QSqlRelationalTableModel>();
    linksToModel->setTable("links");
    linksToModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    linksToModel->setRelation(2, QSqlRelation("notes", "id", "title"));
    ui->tableViewLinksTo->setModel(linksToModel.get());
    ui->tableViewLinksTo->hideColumn(0);
    ui->tableViewLinksTo->hideColumn(1);
    ui->tableViewLinksTo->hideColumn(3);
    ui->tableViewLinksTo->hideColumn(4);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAboutQt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::openNote(AbstractNote* note)
{
    currentNote = note;

    QString title = QString("QtSemanticNotes | %1").arg(note->getTitle());
    setWindowTitle(title);

    ui->tabWidgetNotes->setTabText(ui->tabWidgetNotes->currentIndex(), note->getTitle());

    ui->textBrowserNoteContent->setEnabled(true);
    ui->tableViewAliases->setEnabled(true);
    ui->tableViewNoteTags->setEnabled(true);
    ui->tableViewLinkedFrom->setEnabled(true);
    ui->tableViewLinksTo->setEnabled(true);
    ui->toolButtonAddAlias->setEnabled(true);
    ui->toolButtonAddTag->setEnabled(true);

    ui->textBrowserNoteContent->setText(note->getContent());

    QString idFilter = QString("note_id = %1").arg(note->getId());

    aliasesModel->setFilter(idFilter);
    aliasesModel->select();

    tagsModel->setFilter(idFilter);
    tagsModel->select();

    linkedFromModel->setFilter(idFilter);
    linkedFromModel->select();

    linksToModel->setFilter(idFilter);
    linksToModel->select();
}

void MainWindow::on_toolButtonAddAlias_clicked()
{
    bool ok;
    QString alias = QInputDialog::getText(this, tr("Add Alias"),
                                         tr("Alias name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !alias.isEmpty()) {
        QDateTime now = QDateTime::currentDateTime();
        QString now_s = now.toString(Qt::ISODateWithMs);

        QSqlRecord record = aliasesModel->record();
        record.setGenerated("id", false);
        record.setValue("alias", alias);
        record.setValue("note_id", currentNote->getId());
        record.setValue("created_at", now_s);
        record.setValue("updated_at", now_s);
        aliasesModel->insertRecord(-1, record);
    }
}

void MainWindow::on_toolButtonRemoveAlias_clicked()
{
    QModelIndexList indexes = ui->tableViewAliases->selectionModel()->selectedIndexes();
    for(QModelIndex& index : indexes) {
        aliasesModel->removeRows(index.row(), 1);
    }
    aliasesModel->select();
}

void MainWindow::on_toolButtonAddTag_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Tag"),
                                         tr("Tag name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !name.isEmpty()) {
        QDateTime now = QDateTime::currentDateTime();
        QString now_s = now.toString(Qt::ISODateWithMs);

        uint id;

        QSqlQuery tagIdQuery;
        tagIdQuery.prepare("SELECT id "
                           "FROM tags "
                           "WHERE name = :name");
        tagIdQuery.bindValue(":name", name);
        Database::safeExecPreparedQuery(tagIdQuery);

        if (tagIdQuery.next()) {
            id = tagIdQuery.value(0).toUInt();
        } else {
            QSqlQuery insertTagQuery;
            insertTagQuery.prepare("INSERT INTO tags (name, created_at, updated_at) "
                      "VALUES (:name, :created_at, :updated_at)");
            insertTagQuery.bindValue(":name", name);
            insertTagQuery.bindValue(":created_at", now_s);
            insertTagQuery.bindValue(":updated_at", now_s);
            Database::safeExecPreparedQuery(insertTagQuery);

            QSqlQuery rowIdQuery;
            rowIdQuery.prepare("SELECT last_insert_rowid()");
            Database::safeExecPreparedQuery(rowIdQuery);
            rowIdQuery.next();
            id = rowIdQuery.value(0).toUInt();
        }

        QSqlQuery insertNoteTagsQuery;
        insertNoteTagsQuery.prepare("INSERT INTO note_tags (note_id, tag_id, created_at, updated_at) "
                  "VALUES (:note_id, :tag_id, :created_at, :updated_at)");
        insertNoteTagsQuery.bindValue(":note_id", currentNote->getId());
        insertNoteTagsQuery.bindValue(":tag_id", id);
        insertNoteTagsQuery.bindValue(":created_at", now_s);
        insertNoteTagsQuery.bindValue(":updated_at", now_s);
        Database::safeExecPreparedQuery(insertNoteTagsQuery);

        tagsModel->select();
    }
}

void MainWindow::on_toolButtonRemoveTag_clicked()
{
    QModelIndexList indexes = ui->tableViewNoteTags->selectionModel()->selectedIndexes();
    for(QModelIndex& index : indexes) {
        tagsModel->removeRows(index.row(), 1);
    }
    tagsModel->select();
}

void MainWindow::on_treeViewNotes_customContextMenuRequested(const QPoint& point)
{
    QModelIndex selectedIndex = ui->treeViewNotes->indexAt(point);

    if (!selectedIndex.isValid())
        return;

    NoteItem* selectedItem = noteTreeModel->itemFromIndex(selectedIndex);

    auto onOpenAction = [this, selectedItem](){
        AbstractNote* note = selectedItem->getAsAbstractNote();
        openNote(note);
    };

    auto onAddSubnote = [this, selectedItem, selectedIndex](){
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

    auto onRenameNote = [this, selectedItem, selectedIndex](){
        bool ok;
        QString title = QInputDialog::getText(this, tr("Rename note"),
                                             tr("New Title:"), QLineEdit::Normal,
                                             selectedItem->getAsAbstractNote()->getTitle(), &ok);
        if (ok && !title.isEmpty()) {
            noteTreeModel->renameNoteAtIndex(title, selectedIndex);
        }
    };

    auto onDeleteAction = [this, selectedIndex](){
        QMessageBox msgBox;
        msgBox.setText("Remove note and it's subnotes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int answer = msgBox.exec();
        if (answer == QMessageBox::Yes) {
            noteTreeModel->deleteNoteAtIndex(selectedIndex);
        }
    };

    notesContextMenu.addAction("Open", onOpenAction);
    notesContextMenu.addAction("Open in new tab", [](){});
    notesContextMenu.addAction("Add Subnote", onAddSubnote);
    notesContextMenu.addAction("Rename", onRenameNote);
    notesContextMenu.addSeparator();
    notesContextMenu.addAction("Delete", onDeleteAction);

    notesRootContextMenu.addAction("Open", onOpenAction);
    notesRootContextMenu.addAction("Open in new tab", [](){});
    notesRootContextMenu.addAction("Add Subnote", onAddSubnote);
    notesRootContextMenu.addAction("Rename", onRenameNote);

    auto globalPoint = ui->treeViewNotes->viewport()->mapToGlobal(point);

    if (selectedItem == rootItem) {
        notesRootContextMenu.exec(globalPoint);
    } else {
        notesContextMenu.exec(globalPoint);
    }
}

void MainWindow::on_treeViewNotes_clicked(const QModelIndex &index)
{
    auto item = noteTreeModel->itemFromIndex(index);
    auto note = item->getAsAbstractNote();
    openNote(note);
}
