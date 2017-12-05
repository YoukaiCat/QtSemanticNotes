/*************************************************************************
 ** Copyright (C) 2017 Vladislav Mileshkin
 **
 ** This file is part of QtSemanticNotes.
 **
 ** QtSemanticNotes is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** QtSemanticNotes is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with QtSemanticNotes. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../database/Database.h"

#include <QMessageBox>
#include <QInputDialog>

#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>

#include <QDebug>

#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
#if !(defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(Q_OS_OPENBSD) || defined(Q_OS_NETBSD) || defined(Q_OS_UNIX))
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << QString(":/icons/themes"));
    QIcon::setThemeName("breeze");
#endif

    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/AppIcon"));
    setupTrayIcon();
    setupDatabase();

    //TODO: implement toolbar actions
    ui->mainToolBar->hide();
    //TODO: show highlighted link in status bar
    ui->statusBar->hide();

    qApp->installEventFilter(this);

    linksManager.setup(ui->tableViewLinkedFrom, ui->tableViewLinksTo);
    tabsManager.setup(ui->tabWidgetNotes, &linksManager);
    tabsManager.setupNavigation(ui->actionBackward, ui->actionForward);
    notesManager.setup(ui->treeViewNotes, &tabsManager);
    tagsManager.setup(ui->treeViewTags);
    aliasesManager.setup(ui->tableViewAliases);
    noteTagsManager.setup(ui->tableViewNoteTags);
    searchManager.setup(ui->tableViewSearch);

    QDesktopServices::setUrlHandler("qtsemanticnotes", &notesManager, "openNote");

    linksManager.loadPossibleLinks();
    connect(&notesManager, &NotesManager::noteAdded,
            &linksManager, &LinksManager::updateBackLinks);
    connect(&notesManager, &NotesManager::noteRenamed,
            &linksManager, &LinksManager::updateBackLinks);
    connect(&notesManager, &NotesManager::noteRemoved,
            &linksManager, &LinksManager::updateBackLinks);
    connect(&aliasesManager, &AliasesManager::aliasAdded,
            &linksManager, &LinksManager::updateBackLinksById);
    connect(&aliasesManager, &AliasesManager::aliasRemoved,
            &linksManager, &LinksManager::updateBackLinksById);

    connect(&linksManager, &LinksManager::linksUpdated,
            &linksManager, &LinksManager::selectFromModel);
    connect(&linksManager, &LinksManager::backLinksUpdated,
            &linksManager, &LinksManager::selectFromModel);
    connect(&linksManager, &LinksManager::linkClicked,
            &notesManager, &NotesManager::selectNoteInTreeById);

    connect(&notesManager, &NotesManager::noteModified,
            this, &MainWindow::showNoteIsModified);
    connect(&notesManager, &NotesManager::noteSaved,
            this, &MainWindow::showNoteIsNotModified);
    connect(&notesManager, &NotesManager::noteOpened,
            this, &MainWindow::showNoteIsNotModified);
    connect(&notesManager, &NotesManager::noteClosed,
            this, &MainWindow::showNoteIsNotModified);
    connect(&notesManager, &NotesManager::noteRemoved,
            this, &MainWindow::showNoteIsNotModified);

    //Открыть заметку, если она выделена
    connect(&notesManager, &NotesManager::noteSelected,
            &notesManager, &NotesManager::openSelectedNote);

    //Закрыть текущую заметку, если ни одна заметка не выделена
    connect(&notesManager, &NotesManager::noteNotSelected,
            &notesManager, &NotesManager::closeNote);

    //Найти и выделить заметку в дереве, после добавления
    //(удобно, чтобы вручную не раскрывать поддерево родителя)
    connect(&notesManager, &NotesManager::noteAdded,
            &notesManager, &NotesManager::selectNoteInTree);

    //Обновить дерево тегов после удаления заметки (а значит и всех её тегов)
    connect(&notesManager, &NotesManager::noteRemoved,
            &tagsManager, &TagsManager::loadTags);

    //Переключиться на вкладку с textBrowser после открытия заметки
    connect(&notesManager, &NotesManager::noteOpened,
            this, &MainWindow::switchTabToNote);

    //Показать имя заметки в названии окна после открытия заметки
    connect(&notesManager, &NotesManager::noteOpened,
            this, &MainWindow::setWindowTitle);

    //Показать имя заметки в названии вкладки после открытия заметки
    connect(&notesManager, &NotesManager::noteOpened,
            this, &MainWindow::setTabTitle);

    //Сделать доступными действия редактирования после открытия заметки
    connect(&notesManager, &NotesManager::noteOpened,
            this, &MainWindow::enableNoteActions);
    connect(&notesManager, &NotesManager::noteOpened,
            this, &MainWindow::enableNodeEditWidgets);

    //Сделать недоступными действия редактирования после закрытия заметки
    connect(&notesManager, &NotesManager::noteClosed,
            this, &MainWindow::disableNoteActions);
    connect(&notesManager, &NotesManager::noteClosed,
            this, &MainWindow::disableNodeEditWidgets);

    //Tags
    //Запросить поиск заметок по тегу после двойного клика по нему
    connect(ui->treeViewTags, &QTreeView::doubleClicked,
            &tagsManager, &TagsManager::requestSearchByTag);

    //Выполнить поиск заметок по тегу после запроса
    connect(&tagsManager, &TagsManager::searchByTagRequested,
            &searchManager, &SearchManager::findNotesByTag);

    //Сделать доступными действия редактирования тэга если тэг выделен
    connect(&tagsManager, &TagsManager::tagSelected,
            this, &MainWindow::enableTagActions);

    //Сделать недоступными действия редактирования тэга если тэг не выделен
    connect(&tagsManager, &TagsManager::tagNotSelected,
            this, &MainWindow::disableTagActions);

    connect(&tagsManager, &TagsManager::tagRemoved,
            &noteTagsManager, &NoteTagsManager::selectFromModel);

    connect(&tagsManager, &TagsManager::tagRemoved,
            &tagsManager, &TagsManager::loadTags);

    //NoteTags
    //Показать тэги для открытой заметки
    connect(&notesManager, &NotesManager::noteOpened,
            &noteTagsManager, &NoteTagsManager::filterModel);

    //Не показывать тэги после того как заметка закрыта
    connect(&notesManager, &NotesManager::noteClosed,
            &noteTagsManager, &NoteTagsManager::clearModel);

    //Показать диалог добавления тэга для заметки, когда нажата кнопка добавления
    connect(ui->toolButtonAddTag, &QToolButton::clicked,
            this, &MainWindow::addNoteTag);

    //Удалить выделенный тэг заметки, когда нажата кнопка удаления
    connect(ui->toolButtonRemoveTag, &QToolButton::clicked,
            &noteTagsManager, &NoteTagsManager::removeSelectedNoteTags);

    //Сделать доступной кнопку удаления тэга заметки если тэг выделен
    connect(&noteTagsManager, &NoteTagsManager::noteTagSelected,
            this, &MainWindow::enableRemoveNoteTagButton);

    //Сделать недоступной кнопку удаления тэга заметки если тэг не выделен
    connect(&noteTagsManager, &NoteTagsManager::noteTagNotSelected,
            this, &MainWindow::disableRemoveNoteTagButton);

    //Сделать недоступной кнопку удаления тэга заметки если тэг удалён
    //(но почему-то всё ещё остаётся выделение в таблице)
    connect(&noteTagsManager, &NoteTagsManager::noteTagRemoved,
            this, &MainWindow::disableRemoveNoteTagButton);

    //Перестроить дерево тэгов после добавления тэга заметки
    connect(&noteTagsManager, &NoteTagsManager::noteTagAdded,
            &tagsManager, &TagsManager::loadTags);

    //Перестроить дерево тэгов после удаления тэга заметки
    connect(&noteTagsManager, &NoteTagsManager::noteTagRemoved,
            &tagsManager, &TagsManager::loadTags);

    //Aliases CRUD
    //Показать алиасы для открытой заметки
    connect(&notesManager, &NotesManager::noteOpened,
            &aliasesManager, &AliasesManager::filterModel);

    //Не показывать алиасы после того как заметка закрыта
    connect(&notesManager, &NotesManager::noteClosed,
            &aliasesManager, &AliasesManager::clearModel);

    //Показать диалог добавления алиаса для заметки, когда нажата кнопка добавления
    connect(ui->toolButtonAddAlias, &QToolButton::clicked,
            this, &MainWindow::addNoteAlias);

    //Удалить выделенный алиас заметки, когда нажата кнопка удаления
    connect(ui->toolButtonRemoveAlias, &QToolButton::clicked,
            &aliasesManager, &AliasesManager::removeSelectedAliases);

    //Сделать доступной кнопку удаления алиаса заметки если алиас выделен
    connect(&aliasesManager, &AliasesManager::aliasSelected,
            this, &MainWindow::enableRemoveAliasButton);

    //Сделать недоступной кнопку удаления алиаса заметки если алиас не выделен
    connect(&aliasesManager, &AliasesManager::aliasNotSelected,
            this, &MainWindow::disableRemoveAliasButton);

    //Сделать недоступной кнопку удаления алиаса заметки если алиас удалён
    //(но почему-то всё ещё остаётся выделение в таблице)
    connect(&aliasesManager, &AliasesManager::aliasRemoved,
            this, &MainWindow::disableRemoveAliasButton);

    //Show links
    //Показать линки для открытой заметки
    connect(&notesManager, &NotesManager::noteOpened,
            &linksManager, &LinksManager::filterModel);

    //Search
    //Найти и выделить заметку в дереве, когда она выбрана в результатах поиска
    connect(&searchManager, &SearchManager::resultSelected,
            &notesManager, &NotesManager::selectNoteInTreeById);

    //Переключиться на вкладку поиска, когда готов результат поиска
    connect(&searchManager, &SearchManager::resultReady,
            this, &MainWindow::switchTabToSearchResults);

    //Note actions
    connect(ui->actionAdd, &QAction::triggered,
            this, &MainWindow::addNote);

    connect(ui->actionAddSubnote, &QAction::triggered,
            this, &MainWindow::addSubnote);

    connect(ui->actionRename, &QAction::triggered,
            this, &MainWindow::renameNote);

    connect(ui->actionSave, &QAction::triggered,
            &notesManager, &NotesManager::saveNote);

    connect(ui->actionDeleteNote, &QAction::triggered,
            this, &MainWindow::removeNote);

    connect(ui->actionFindNote, &QAction::triggered,
            this, &MainWindow::findNotesByWords);

    //Tag actions
    connect(ui->actionFindByTag, &QAction::triggered,
            &tagsManager, &TagsManager::requestSearchBySelectedTag);

    connect(ui->actionDeleteTag, &QAction::triggered,
            &tagsManager, &TagsManager::removeSelectedTag);

    //View mode
    connect(ui->actionViewMode, &QAction::triggered,
            &notesManager, &NotesManager::switchMode);

    //App actions
    connect(ui->actionQuit, &QAction::triggered,
            quitAction, &QAction::trigger);
}

MainWindow::~MainWindow()
{
    delete ui;
}

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

//SVG tray icons not working in KDE
//See https://bugs.kde.org/show_bug.cgi?id=365131
//https://bugreports.qt.io/browse/QTBUG-53550
void MainWindow::setupTrayIcon()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    restoreAction = new QAction(tr("&Restore"), this);
    quitAction = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this);
    connect(minimizeAction, &QAction::triggered, [this](){ hide(); });
    connect(restoreAction, &QAction::triggered, [this](){ showNormal(); });
    connect(quitAction, &QAction::triggered, [](){ qApp->quit(); });

    trayMenu.addAction(minimizeAction);
    trayMenu.addAction(restoreAction);
    trayMenu.addSeparator();
    trayMenu.addAction(quitAction);

    tray.setIcon(QIcon(":/icons/AppIconRasterized"));
    tray.setContextMenu(&trayMenu);
    tray.show();
    connect(&tray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
}

void MainWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (tray.isVisible()) {
        hide();
        event->ignore();
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger) {
        Q_UNUSED(reason)
        isHidden() ? showNormal() : hide();
    }
}

void MainWindow::showCriticalErrorAndQuit(const QString& error)
{
    QMessageBox::critical(this, "QtSemanticNotes", error);
    abort();
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
    if (ok && !answer.simplified().isEmpty()) {
        return answer;
    } else {
        return {};
    }
}

void MainWindow::enableTagActions()
{
    ui->actionFindByTag->setEnabled(true);
    ui->actionDeleteTag->setEnabled(true);
}

void MainWindow::disableTagActions()
{
    ui->actionFindByTag->setEnabled(false);
    ui->actionDeleteTag->setEnabled(false);
}

void MainWindow::enableNoteActions()
{
    ui->actionAddSubnote->setEnabled(true);
    ui->actionRename->setEnabled(true);
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
    ui->tableViewAliases->setEnabled(false);
    ui->tableViewNoteTags->setEnabled(false);
    ui->tableViewLinkedFrom->setEnabled(false);
    ui->tableViewLinksTo->setEnabled(false);
    ui->toolButtonAddAlias->setEnabled(false);
    ui->toolButtonAddTag->setEnabled(false);
    ui->actionViewMode->setEnabled(false);
}

void MainWindow::setTabTitle(shared_ptr<Note> note)
{
    ui->tabWidgetNotes->setTabText(0, note->getTitle());
}

void MainWindow::setWindowTitle(shared_ptr<Note> note)
{
    QString windowTitle = QString("%1[*] — QtSemanticNotes").arg(note->getTitle());
    QMainWindow::setWindowTitle(windowTitle);
}

void MainWindow::addNote()
{
    auto title = askString(tr("Add Note"), tr("Title:"));
    if(title.has_value()) {
        if(Note::existWithTitleOrAlias(title.value())) {
            QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
            addNote();
        } else {
            notesManager.addNote(title.value());
        }
    }
}

void MainWindow::addSubnoteAtIndex(const QModelIndex& parentIndex)
{
    auto title = askString(tr("Add Subnote"), tr("Title:"));
    if(title.has_value()) {
        if(Note::existWithTitleOrAlias(title.value())) {
            QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
            addSubnoteAtIndex(parentIndex);
        } else {
            notesManager.addSubnote(title.value(), parentIndex);
        }
    }
}

void MainWindow::addSubnote()
{
    auto title = askString(tr("Add Subnote"), tr("Title:"));
    if(title.has_value()) {
        if(Note::existWithTitleOrAlias(title.value())) {
            QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
            addSubnote();
        } else {
            notesManager.addSubnoteToSelected(title.value());
        }
    }
}

void MainWindow::renameNoteAtIndex(const QModelIndex& index)
{
    QString oldTitle = notesManager.selectedNote()->getTitle();
    auto title = askString(tr("Rename note"), tr("Title:"), oldTitle);
    if(title.has_value()) {
        if(Note::existWithTitleOrAlias(title.value())) {
            QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
            renameNoteAtIndex(index);
        } else {
            notesManager.renameNote(title.value(), index);
        }
    }
}

void MainWindow::renameNote()
{
    QString oldTitle = notesManager.selectedNote()->getTitle();
    auto title = askString(tr("Rename note"), tr("Title:"), oldTitle);
    if(title.has_value()) {
        if(Note::existWithTitleOrAlias(title.value())) {
            QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
            renameNote();
        } else {
            notesManager.renameSelectedNote(title.value());
        }
    }
}

void MainWindow::removeNoteAtIndex(const QModelIndex& index)
{
    if(askBool(tr("Remove note and it's subnotes?"))) {
        notesManager.removeNote(index);
    }
}

void MainWindow::removeNote()
{
    if(askBool(tr("Remove note and it's subnotes?"))) {
        notesManager.removeSelectedNote();
    }
}

void MainWindow::findNotesByWords()
{
    auto words = askString(tr("Search in title, content and aliases"), tr("Words:"));
    if(words.has_value()) {
        searchManager.findNotes(words.value());
    }
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

void MainWindow::addNoteAlias()
{
    auto name = askString(tr("Add Alias"), tr("Alias name:"));
    if (name.has_value()) {
        if(Note::existWithTitleOrAlias(name.value())) {
            QMessageBox::warning(this, "QtSemanticNotes", tr("Note with that title or alias already exists"));
            addNoteAlias();
        } else {
            aliasesManager.addAlias(notesManager.selectedNote(), name.value());
        }
    }
}

void MainWindow::enableRemoveAliasButton()
{
    ui->toolButtonRemoveAlias->setEnabled(true);
}

void MainWindow::disableRemoveAliasButton()
{
    ui->toolButtonRemoveAlias->setEnabled(false);
}

void MainWindow::addNoteTag()
{
    auto name = askString(tr("Add Tag"), tr("Tag name:"));
    if (name.has_value()) {
        noteTagsManager.addNoteTag(notesManager.selectedNote(), name.value());
    }
}

void MainWindow::enableRemoveNoteTagButton()
{
    ui->toolButtonRemoveTag->setEnabled(true);
}

void MainWindow::disableRemoveNoteTagButton()
{
    ui->toolButtonRemoveTag->setEnabled(false);
}

void MainWindow::removeTag()
{
    if(askBool(tr("Remove tag and it's subtags?"))) {
        tagsManager.removeSelectedTag();
    }
}

void MainWindow::removeTagAtIndex(const QModelIndex& index)
{
    if(askBool(tr("Remove tag and it's subtags?"))) {
        tagsManager.removeTag(index);
    }
}

void MainWindow::switchTabToNote()
{
    ui->tabWidgetNotes->setCurrentIndex(0);
}

void MainWindow::switchTabToSearchResults()
{
    ui->tabWidgetNotes->setCurrentIndex(1);
}

void MainWindow::on_treeViewNotes_customContextMenuRequested(const QPoint& point)
{
    QModelIndex selectedIndex = ui->treeViewNotes->indexAt(point);
    auto globalPoint = ui->treeViewNotes->viewport()->mapToGlobal(point);

    QMenu notesContextMenu;
    if (!selectedIndex.isValid()) {
        notesContextMenu.addAction(QIcon::fromTheme("document-new"), tr("Add Note"), [this](){
            addNote();
        });
    } else {
        notesContextMenu.addAction(QIcon::fromTheme("document-new"), tr("Add Subnote"), [this, selectedIndex](){
            addSubnoteAtIndex(selectedIndex);
        });
        notesContextMenu.addAction(QIcon::fromTheme("document-edit"), tr("Rename"), [this, selectedIndex](){
            renameNoteAtIndex(selectedIndex);
        });
        notesContextMenu.addSeparator();
        notesContextMenu.addAction(QIcon::fromTheme("document-close"), tr("Delete"), [this, selectedIndex](){
            removeNoteAtIndex(selectedIndex);
        });
    }
    notesContextMenu.exec(globalPoint);
}

void MainWindow::on_treeViewTags_customContextMenuRequested(const QPoint& point)
{
    QModelIndex selectedIndex = ui->treeViewTags->indexAt(point);
    if (selectedIndex.isValid()) {
        QMenu tagsContextMenu;
        tagsContextMenu.addAction(QIcon::fromTheme("edit-find"), tr("Find By Tag"), [this, selectedIndex](){
            tagsManager.requestSearchByTag(selectedIndex);
        });
        tagsContextMenu.addSeparator();
        tagsContextMenu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), [this, selectedIndex](){
            removeTagAtIndex(selectedIndex);
        });
        auto globalPoint = ui->treeViewTags->viewport()->mapToGlobal(point);
        tagsContextMenu.exec(globalPoint);
    }
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object)
    //if (object == target && event->type() == QEvent::KeyPress) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->buttons() == Qt::BackButton) {
            if (ui->actionBackward->isEnabled()) {
                ui->actionBackward->trigger();
            }
            return true;
        } else if (mouseEvent->buttons() == Qt::ForwardButton) {
            if (ui->actionForward->isEnabled()) {
                ui->actionForward->trigger();
            }
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
void MainWindow::on_actionAboutQtSemanticNotes_triggered()
{
#ifdef VERSION
    QMessageBox::about(this, "About QtSemanticNotes", tr("QtSemanticNotes version: %1\n\n").arg(TOSTRING(VERSION)) +
                       tr("Copyright © 2017 Vladislav Mileshkin\n\n") +
                       tr("Licensed under the terms of the\nGNU General Public License Version 3"));
#endif
}

void MainWindow::on_actionAboutQt_triggered()
{
    qApp->aboutQt();
}
