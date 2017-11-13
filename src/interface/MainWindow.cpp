#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../database/Database.h"

#include "../entities/Note.h"
#include "../entities/RootNote.h"
#include "../entities/Tag.h"
#include "../entities/Search.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>

#include <QMessageBox>
#include <QInputDialog>

#include <QItemSelectionModel>

#include <QRegularExpression>

#include <QDir>
#include <QStandardPaths>

#include <QDebug>

#include <memory>
using std::make_unique;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupDatabase();
    setupNotesTree();
    setupTagsTree();
    setupNoteModels();
    setupSearchModel();

    loadPossibleLinks();

    ui->textBrowserNoteContent->setDocument(&content);

    connect(&content, &QTextDocument::contentsChanged,
            this, &MainWindow::onContentModified);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//~~~ Setup db and views ~~~~~~~~~~~~~

QString MainWindow::getDatabasePath()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if(path.isEmpty()) {
        showCriticalErrorAndQuit(tr("Data location is not writable"));
    }
    QDir dir;
    if(!dir.mkpath(path)) {
        showCriticalErrorAndQuit(tr("Can't create app data directory"));
    }
    path.append("/QtSemanticNotes.sqlite");
    return path;
}

void MainWindow::setupDatabase()
{
    try {
        Database::openDatabase(getDatabasePath());
    } catch (OpenDBException&) {
        showCriticalErrorAndQuit(tr("Can't open database"));
    } catch (QueryException&) {
        showCriticalErrorAndQuit(tr("Can't open database"));
    }
}

void MainWindow::setupNotesTreeRoot()
{
    rootNote = RootNote::getRootNote();
    noteRootItem = new NoteItem(rootNote.get()); //Model is the owner
    noteRealRootItem = new NoteItem(rootNote.get(), noteRootItem);
    idToItem.insert(rootNote->getId(), noteRealRootItem);
}

void MainWindow::createItemsForNotes()
{
    for(auto& note : notes) {
        NoteItem* item = new NoteItem(note.get());
        idToItem.insert(note->getId(), item);
    }
}

void MainWindow::setItemChilds()
{
    for(auto& note : notes) {
        NoteItem* item = idToItem[note->getId()];
        NoteItem* parent = idToItem[note->getParentId()];
        parent->addChild(item);
    }
}

void MainWindow::setupNotesTreeChildren()
{
    notes = Note::getAll();
    createItemsForNotes();
    setItemChilds();
}

void MainWindow::fixRoot()
{
    QModelIndex rootIndex = noteTreeModel->index(0, 0, QModelIndex());
    ui->treeViewNotes->setRootIndex(rootIndex);
}

void MainWindow::setupNotesTreeModel()
{
    noteTreeModel = make_unique<NoteTreeModel>(noteRootItem);
    ui->treeViewNotes->setModel(noteTreeModel.get());
    fixRoot();
    connect(ui->treeViewNotes->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::openSelectedNote);
}

void MainWindow::setupNotesTree()
{
    setupNotesTreeRoot();
    setupNotesTreeChildren();
    setupNotesTreeModel();
}

void MainWindow::setupTagsTreeRoot()
{
    tagRootItem = new TagItem(); //Model is the owner
}

void MainWindow::setupTagsTreeChildren()
{
    vector<unique_ptr<Tag>> tags = Tag::getAll();
    for(auto& tag : tags) {
        QString name = tag->getName();
        //TODO Preferences for tag part separator
        QStringList words = name.split(".");
        tagRootItem->insert(words);
    }
}

void MainWindow::setupTagsTreeModel()
{
    tagTreeModel = make_unique<TagTreeModel>(tagRootItem);
    ui->treeViewTags->setModel(tagTreeModel.get());
}

void MainWindow::setupTagsTree()
{
    setupTagsTreeRoot();
    setupTagsTreeChildren();
    setupTagsTreeModel();
}

void MainWindow::createAliasesModel()
{
    aliasesModel = make_unique<QSqlTableModel>();
    aliasesModel->setTable("aliases");
    aliasesModel->setEditStrategy(QSqlTableModel::OnFieldChange);
}

void MainWindow::setupAliases()
{
    createAliasesModel();
    ui->tableViewAliases->setModel(aliasesModel.get());
    ui->tableViewAliases->hideColumn(0);
    ui->tableViewAliases->hideColumn(2);
    ui->tableViewAliases->hideColumn(3);
    connect(ui->tableViewAliases->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::toggleRemoveAliasButtonEnabled);
}

void MainWindow::createTagsModel()
{
    tagsModel = make_unique<QSqlRelationalTableModel>();
    tagsModel->setTable("note_tags");
    tagsModel->setRelation(2, QSqlRelation("tags", "id", "name"));
    tagsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
}

void MainWindow::setupTags()
{
    createTagsModel();
    ui->tableViewNoteTags->setModel(tagsModel.get());
    ui->tableViewNoteTags->hideColumn(0);
    ui->tableViewNoteTags->hideColumn(1);
    ui->tableViewNoteTags->hideColumn(3);
    connect(ui->tableViewNoteTags->selectionModel(),&QItemSelectionModel::selectionChanged,
            this, &MainWindow::toggleRemoveTagButtonEnabled);
}

void MainWindow::createLinksFromModel()
{
    linkedFromModel = make_unique<QSqlRelationalTableModel>();
    linkedFromModel->setTable("links");
    linkedFromModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    linkedFromModel->setRelation(1, QSqlRelation("notes", "id", "title"));
}

void MainWindow::setupLinksFrom()
{
    createLinksFromModel();
    ui->tableViewLinkedFrom->setModel(linkedFromModel.get());
    ui->tableViewLinkedFrom->hideColumn(0);
    ui->tableViewLinkedFrom->hideColumn(2);
    ui->tableViewLinkedFrom->hideColumn(3);
}

void MainWindow::createLinksToModel()
{
    linksToModel = make_unique<QSqlRelationalTableModel>();
    linksToModel->setTable("links");
    linksToModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    linksToModel->setRelation(2, QSqlRelation("notes", "id", "title"));
}

void MainWindow::setupLinksTo()
{
    createLinksToModel();
    ui->tableViewLinksTo->setModel(linksToModel.get());
    ui->tableViewLinksTo->hideColumn(0);
    ui->tableViewLinksTo->hideColumn(1);
    ui->tableViewLinksTo->hideColumn(3);
}

void MainWindow::setupNoteModels()
{
    setupAliases();
    setupTags();
    setupLinksFrom();
    setupLinksTo();
}

void MainWindow::setupSearchModel()
{
    searchModel = make_unique<QSqlQueryModel>();
    ui->tableViewSearch->setModel(searchModel.get());
}

//~~~~~~~~~~~~~~~~~~~~

//Helper

void MainWindow::showCriticalErrorAndQuit(const QString& error)
{
    QMessageBox::critical(this, "QtSemanticNotes", error);
    qApp->quit();
}

bool MainWindow::askBool(const QString& question)
{
    QMessageBox msgBox;
    msgBox.setText(question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int answer = msgBox.exec();
    return answer == QMessageBox::Yes;
}

optional<QString> MainWindow::askString(const QString& title,
                                        const QString& label,
                                        const QString& defaultText)
{
    bool ok;
    QString answer = QInputDialog::getText(this, title, label,
                                           QLineEdit::Normal, defaultText, &ok);
    if (ok && !answer.isEmpty()) {
        return answer;
    } else {
        return {};
    }
}

Note* MainWindow::noteFromIndex(const QModelIndex& index)
{
    return noteTreeModel->itemFromIndex(index)->getValue();
}

QString MainWindow::tagFromIndex(const QModelIndex& index)
{
    QStringList words;
    return tagTreeModel->itemFromIndex(index)->getFullTag(words);
}

//~~~~~~~~~~~~~~~~~~~~

//Notes

void MainWindow::enableNoteActions()
{
    ui->actionAddSubnote->setEnabled(true);
    ui->actionRename->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionDeleteNote->setEnabled(true);
}

void MainWindow::disableNoteActions()
{
    ui->actionAddSubnote->setEnabled(false);
    ui->actionRename->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionDeleteNote->setEnabled(false);
}

void MainWindow::enableNodeEditWidgets()
{
    ui->textBrowserNoteContent->setEnabled(true);
    ui->tableViewAliases->setEnabled(true);
    ui->tableViewNoteTags->setEnabled(true);
    ui->tableViewLinkedFrom->setEnabled(true);
    ui->tableViewLinksTo->setEnabled(true);
    ui->toolButtonAddAlias->setEnabled(true);
    ui->toolButtonAddTag->setEnabled(true);
    ui->actionViewMode->setEnabled(true);
}

void MainWindow::disableNodeEditWidgets()
{
    ui->textBrowserNoteContent->setEnabled(false);
    ui->tableViewAliases->setEnabled(false);
    ui->tableViewNoteTags->setEnabled(false);
    ui->tableViewLinkedFrom->setEnabled(false);
    ui->tableViewLinksTo->setEnabled(false);
    ui->toolButtonAddAlias->setEnabled(false);
    ui->toolButtonAddTag->setEnabled(false);
    ui->actionViewMode->setEnabled(false);
}

void MainWindow::selectFromModels()
{
    aliasesModel->select();
    tagsModel->select();
    linkedFromModel->select();
    linksToModel->select();
}

void MainWindow::filterModelsForNote(const Id& id)
{
    QString idFilter = QString("note_id = %1").arg(id);
    aliasesModel->setFilter(idFilter);
    tagsModel->setFilter(idFilter);
    linkedFromModel->setFilter(idFilter);
    linksToModel->setFilter(idFilter);
    selectFromModels();
}

void MainWindow::setWidgetTitles(const QString& title)
{
    QString windowTitle = QString("%1[*] — QtSemanticNotes").arg(title);
    setWindowTitle(windowTitle);
    ui->tabWidgetNotes->setTabText(0, title);
}

void MainWindow::selectNoteInTree()
{
    auto index = noteTreeModel->findIndexByNoteId(currentNote->getId(), ui->treeViewNotes->rootIndex());
    Q_ASSERT(index.has_value());
    ui->treeViewNotes->selectionModel()->setCurrentIndex(index.value(), QItemSelectionModel::SelectCurrent);
}

void MainWindow::openNote(Note* note)
{
    currentNote = note;
    content.setPlainText(note->getContent());
    chooseModeToEnable(note);
    setWidgetTitles(note->getTitle());
    enableNoteActions();
    enableNodeEditWidgets();
    filterModelsForNote(note->getId());
    selectNoteInTree();
}

void MainWindow::openSelectedNote()
{
    QModelIndexList indexes = ui->treeViewNotes->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        Note* note = noteFromIndex(indexes.first());
        openNote(note);
    }
}

void MainWindow::closeNote()
{
    disableNoteActions();
    disableNodeEditWidgets();
    selectFromModels();
}

Note* MainWindow::createNote(const QString& title, const Id& parentId)
{
    auto note = Note::create(title, "", parentId);
    Note* noteptr = note.get();
    notes.push_back(std::move(note));
    return noteptr;
}

optional<Note*> MainWindow::createNoteIfNotExists(const QString& title, const Id& parentId)
{
    if(Note::existWithTitleOrAlias(title)) {
        QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
        return {};
    } else {
        return createNote(title, parentId);
    }
}

void MainWindow::addSubnote(const QModelIndex& parentIndex)
{
    auto title = askString("Add Subnote", "Title:");
    if(title.has_value()) {
        Id parentId = noteFromIndex(parentIndex)->getId();
        auto note = createNoteIfNotExists(title.value(), parentId);
        if(note.has_value()) {
            noteTreeModel->addSubnoteAtIndex(note.value(), parentIndex);
        }
    }
}

void MainWindow::addNote()
{
    addSubnote(ui->treeViewNotes->rootIndex());
}

void MainWindow::updateNoteContent()
{
    QString text = content.toRawText();
    currentNote->setContent(text);
    currentNote->update();
}

void MainWindow::showNoteIsModified()
{
    setWindowModified(true);
    ui->actionSave->setEnabled(true);
}

void MainWindow::showNoteIsNotModified()
{
    setWindowModified(false);
    ui->actionSave->setEnabled(false);
}

void MainWindow::saveNote()
{
    updateNoteContent();
    showNoteIsNotModified();
}

void MainWindow::renameNoteIfNotExists(const QString& title, const QModelIndex& index)
{
    if(Note::existWithTitleOrAlias(title)) {
        QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
    } else {
        noteTreeModel->renameNoteAtIndex(title, index);
    }
}

void MainWindow::renameNote(const QModelIndex& index)
{
    QString oldTitle = noteFromIndex(index)->getTitle();
    auto title = askString("Rename note", "Title:", oldTitle);
    if(title.has_value()) {
        renameNoteIfNotExists(title.value(), index);
    }

}

void MainWindow::deleteNote(const QModelIndex& index)
{
    closeNote();
    if(askBool("Remove note and it's subnotes?")) {
        noteTreeModel->deleteNoteAtIndex(index);
    }
}

void MainWindow::switchTabToSearchResults()
{
    ui->tabWidgetNotes->setCurrentIndex(1);
}

void MainWindow::findNotes()
{
    auto words = askString("Search in title, content and aliases", "Words:");
    if(words.has_value()) {
        QSqlQuery query = Search::findNotesByWords(words.value());
        searchModel->setQuery(query);
        searchModel->setHeaderData(1, Qt::Horizontal, tr("Note Title"));
        ui->tableViewSearch->setColumnHidden(0, true);
        switchTabToSearchResults();
    }
}

//~~~~~~~~~~~~~~~~~~

//Tag

void MainWindow::deleteTag(const QModelIndex& index)
{
    if(askBool("Remove tag and it's subtags?")) {
        QString fulltag = tagFromIndex(index);
        Tag::deleteTagAndSubtags(fulltag);
        tagTreeModel->deleteTagAtIndex(index);
    }
}

void MainWindow::findNotesByTag(const QModelIndex& index)
{
    QString fulltag = tagFromIndex(index);
    QSqlQuery query = Search::findNotesByTag(fulltag);
    searchModel->setQuery(query);
    searchModel->setHeaderData(1, Qt::Horizontal, tr("Note Title"));
    ui->tableViewSearch->setColumnHidden(0, true);
    switchTabToSearchResults();
}

//~~~~~~~~~~~~~~~~~~

//NoteTags

void MainWindow::addNoteTag()
{
    auto name = askString(tr("Add Tag"), tr("Tag name:"));
    if (name.has_value()) {
        auto tag = Tag::getOrCreate(name.value());
        Tag::addNoteTags(currentNote, tag.get());
        tagsModel->select();
    }
}

void MainWindow::deleteNoteTag()
{
    QModelIndexList indexes = ui->tableViewNoteTags->selectionModel()->selectedIndexes();
    for(QModelIndex& index : indexes) {
        Id id = index.data(1).toUInt();
        Tag::deleteTagIfNotUsed(id);
        tagsModel->removeRows(index.row(), 1);
    }
    tagsModel->select();
}

void MainWindow::toggleRemoveTagButtonEnabled()
{
    if(ui->tableViewNoteTags->selectionModel()->selectedIndexes().size() > 0) {
        ui->toolButtonRemoveTag->setEnabled(true);
    } else {
        ui->toolButtonRemoveTag->setEnabled(false);
    }
}

//~~~~~~~~~~~~~~~~~~

//NoteAlias

void MainWindow::addAliasIfNotExists(const QString& alias)
{
    if(Note::existWithTitleOrAlias(alias)) {
        QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
    } else {
        Note::addNoteAlias(currentNote, alias);
    }
}

void MainWindow::addNoteAlias()
{
    auto name = askString(tr("Add Alias"), tr("Alias name:"));
    if (name.has_value()) {
        addAliasIfNotExists(name.value());
    }
    aliasesModel->select();
}

void MainWindow::deleteNoteAlias()
{
    QModelIndexList indexes = ui->tableViewAliases->selectionModel()->selectedIndexes();
    for(QModelIndex& index : indexes) {
        aliasesModel->removeRows(index.row(), 1);
    }
    aliasesModel->select();
}

void MainWindow::toggleRemoveAliasButtonEnabled()
{
    if(ui->tableViewAliases->selectionModel()->selectedIndexes().size() > 0) {
        ui->toolButtonRemoveAlias->setEnabled(true);
    } else {
        ui->toolButtonRemoveAlias->setEnabled(false);
    }
}

//~~~~~~~~~~~~~~~~~~

//Links

void MainWindow::loadPossibleLinks()
{
    possibleLinks = Note::getPossibleLinks();
}

//"java virtual machine" (java virtual machine|virtual machine|machine|java)
QString MainWindow::makeLinks(QString rightPart)
{
    QStringList parts;
    QString leftPart;
    QRegularExpression titlesRegex("(" + possibleLinks.second + ")", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = titlesRegex.match(rightPart);
    while(match.hasMatch()) {
        QString link = QString("<a href='qtsemanticnotes://1'>%1</a>").arg(match.captured(0));
        int position = match.capturedStart() + match.capturedLength();
        leftPart = rightPart.left(position);
        rightPart = rightPart.mid(position);
        leftPart.replace(match.capturedStart(), match.capturedLength(), link);
        parts.append(leftPart);
        match = titlesRegex.match(rightPart);
    }
    parts.append(rightPart);
    return parts.join("");
}

QList<Id> MainWindow::findLinks(const QString& text)
{
    QList<Id> linkedNotes;
    QRegularExpression titlesRegex("(" + possibleLinks.second + ")", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = titlesRegex.globalMatch(text);
    while(i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(0);
        if(possibleLinks.first.contains(word)) {
            linkedNotes.append(possibleLinks.first.value(word));
        }
    }
    return linkedNotes;
}

void MainWindow::updateBackLinks(const Note* note)
{
    QSqlQuery q = Search::findNotesByContent(note->getTitle());
    while(q.next()) {
        Id id = q.value(0).toUInt();
        QString content = q.value(1).toString();
        QList<Id> links = findLinks(content);
        Note::clearLinks(id);
        for(auto& id : links) {
            Note::addNoteLink(id, note->getId());
        }
    }
}

//~~~~~~~~~~~~~~~~~~

//Editor

void MainWindow::enableViewMode()
{
    ui->textBrowserNoteContent->setDocument(nullptr);
    ui->textBrowserNoteContent->setHtml(makeLinks(content.toRawText()));
    ui->textBrowserNoteContent->setReadOnly(true);
}

void MainWindow::enableEditMode()
{
    ui->textBrowserNoteContent->setDocument(&content);
    ui->textBrowserNoteContent->setReadOnly(false);
}

void MainWindow::chooseModeToEnable(Note* note)
{
    if (note->getContent().size() == 0) {
        enableEditMode();
    } else {
        enableViewMode();
    }
}

void MainWindow::onContentModified()
{
    showNoteIsModified();
}

//~~~~~~~~~~~~~~~~~~

//Actions

void MainWindow::on_actionAdd_triggered()
{
    addNote();
}

void MainWindow::on_actionAddSubnote_triggered()
{
    QModelIndexList indexes = ui->treeViewNotes->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        addSubnote(indexes.first());
    }
}

//TODO автоматически находить в дереве и выделять открытую заметку
void MainWindow::on_actionRename_triggered()
{
    QModelIndexList indexes = ui->treeViewNotes->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        renameNote(indexes.first());
    }
}

void MainWindow::on_actionSave_triggered()
{
    saveNote();
}

void MainWindow::on_actionDeleteNote_triggered()
{
    QModelIndexList indexes = ui->treeViewNotes->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        deleteNote(indexes.first());
    }
}

void MainWindow::on_actionFindNote_triggered()
{
    findNotes();
}

void MainWindow::on_actionFindByTag_triggered()
{
    QModelIndexList indexes = ui->treeViewTags->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty()) {
        findNotesByTag(indexes.first());
    }
}

void MainWindow::on_actionPrevious_triggered()
{}

void MainWindow::on_actionNext_triggered()
{}

void MainWindow::on_actionViewMode_triggered()
{
    if (ui->textBrowserNoteContent->isReadOnly()) {
        enableEditMode();
    } else {
        enableViewMode();
    }
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
void MainWindow::on_actionAboutQtSemanticNotes_triggered()
{
#ifdef VERSION
    QMessageBox::about(this, "QtSemanticNotes", QString("Version: %1").arg(TOSTRING(VERSION)));
#endif
}

void MainWindow::on_actionAboutQt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_treeViewNotes_customContextMenuRequested(const QPoint& point)
{
    QModelIndex selectedIndex = ui->treeViewNotes->indexAt(point);
    auto globalPoint = ui->treeViewNotes->viewport()->mapToGlobal(point);

    QMenu notesContextMenu;
    if (!selectedIndex.isValid()) {
        notesContextMenu.addAction("Add Note", [this](){ addNote(); });
    } else {
        notesContextMenu.addAction("Open", [this, selectedIndex](){
            openNote(noteFromIndex(selectedIndex));
        });
        notesContextMenu.addAction("Add Subnote", [this, selectedIndex](){
            addSubnote(selectedIndex);
        });
        notesContextMenu.addAction("Rename", [this, selectedIndex](){
            renameNote(selectedIndex);
        });
        notesContextMenu.addSeparator();
        notesContextMenu.addAction("Delete", [this, selectedIndex](){
            deleteNote(selectedIndex);
        });
    }
    notesContextMenu.exec(globalPoint);
}

void MainWindow::on_treeViewTags_customContextMenuRequested(const QPoint& point)
{
    QModelIndex selectedIndex = ui->treeViewTags->indexAt(point);
    if (selectedIndex.isValid()) {
        QMenu tagsContextMenu;
        tagsContextMenu.addAction("Find By Tag", [this, selectedIndex](){
            findNotesByTag(selectedIndex);
        });
        tagsContextMenu.addSeparator();
        tagsContextMenu.addAction("Delete", [this, selectedIndex](){
            deleteTag(selectedIndex);
        });
        auto globalPoint = ui->treeViewTags->viewport()->mapToGlobal(point);
        tagsContextMenu.exec(globalPoint);
    }
}

void MainWindow::on_treeViewTags_doubleClicked(const QModelIndex& index)
{
    findNotesByTag(index);
}

//~~~~~~~~~~~~~~~~~~~~~

//Aliases buttons

void MainWindow::on_toolButtonAddAlias_clicked()
{
    addNoteAlias();
}

void MainWindow::on_toolButtonRemoveAlias_clicked()
{
    deleteNoteAlias();
}

//~~~~~~~~~~~~~~~~~~

//NoteTags buttons

void MainWindow::on_toolButtonAddTag_clicked()
{
    addNoteTag();
}

void MainWindow::on_toolButtonRemoveTag_clicked()
{
    deleteNoteTag();
}

//~~~~~~~~~~~~~~~~~

void MainWindow::switchTabToNote()
{
    ui->tabWidgetNotes->setCurrentIndex(0);
}

void MainWindow::on_tableViewSearch_clicked(const QModelIndex& index)
{
    QSqlRecord note = searchModel->record(index.row());
    uint id = note.value("id").toUInt();
    if (idToItem.contains(id)) {
        NoteItem* item = idToItem.value(id);
        openNote(item->getValue());
        switchTabToNote();
    }
}
