#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "../models/TagTreeModel.h"

#include "../entities/RootNote.h"

#include <QMainWindow>
#include <QMenu>

#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>

#include <QTextDocument>

#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    //DB
    QString getDatabasePath();
    void setupDatabase();
    //Notes tree
    void setupNotesTreeRoot();
    void createItemsForNotes();
    void setItemChilds();
    void setupNotesTreeChildren();
    void fixRoot();
    void setupNotesTreeModel();
    void setupNotesTree();
    //Tags tree
    void setupTagsTreeRoot();
    void setupTagsTreeChildren();
    void setupTagsTreeModel();
    void setupTagsTree();
    //Note aliases, tags and links
    void createAliasesModel();
    void setupAliases();
    void createTagsModel();
    void setupTags();
    void createLinksFromModel();
    void setupLinksFrom();
    void createLinksToModel();
    void setupLinksTo();
    void setupNoteModels();
    //Search model
    void setupSearchModel();

    void setupTrayIcon();
    void setVisible(bool visible);
    void closeEvent(QCloseEvent *event);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    //Helper
    void showCriticalErrorAndQuit(const QString& error);
    bool askBool(const QString& question);
    optional<QString> askString(const QString& title,
                                const QString& label,
                                const QString& defaultText = "");
    Note* noteFromIndex(const QModelIndex& index);
    QString tagFromIndex(const QModelIndex& index);

    //Notes
    void enableNoteActions();
    void disableNoteActions();
    void enableNodeEditWidgets();
    void disableNodeEditWidgets();
    void selectFromModels();
    void filterModelsForNote(const Id& id);
    void setWidgetTitles(const QString& title);
    void openNote(Note* note);
    void openSelectedNote();
    void closeNote();
    Note* createNote(const QString& title, const Id& parentId);
    optional<Note*> createNoteIfNotExists(const QString& title, const Id& parentId);
    void addSubnote(const QModelIndex& parentIndex);
    void addNote();
    void updateNoteContent();
    void showNoteIsModified();
    void showNoteIsNotModified();
    void saveNote();
    void renameNoteIfNotExists(const QString& title, const QModelIndex& index);
    void renameNote(const QModelIndex& index);
    void deleteNote(const QModelIndex& index);
    void findNotes();

    //Tags
    void deleteTag(const QModelIndex& index);
    void findNotesByTag(const QModelIndex& index);

    //NoteTags
    void addNoteTag();
    void deleteNoteTag();
    void toggleRemoveTagButtonEnabled();

    //NoteAlias
    void addAliasIfNotExists(const QString& alias);
    void addNoteAlias();
    void deleteNoteAlias();
    void toggleRemoveAliasButtonEnabled();

    //Links
    void loadPossibleLinks();
    QString makeLinks(QString text);
    QList<Id> findLinks(const QString& text);
    void updateBackLinks(const Note* note);

    //Editor
    void enableViewMode();
    void enableEditMode();
    void chooseModeToEnable(Note* note);
    void onContentModified();

    //Slots for UI signals

    //Actions
    void on_actionAdd_triggered();
    void on_actionAddSubnote_triggered();
    void on_actionRename_triggered();
    void on_actionSave_triggered();
    void on_actionDeleteNote_triggered();
    void on_actionFindNote_triggered();

    void on_actionFindByTag_triggered();

    void on_actionPrevious_triggered();
    void on_actionNext_triggered();

    void on_actionViewMode_triggered();

    void on_actionAboutQtSemanticNotes_triggered();
    void on_actionAboutQt_triggered();

    void on_treeViewNotes_customContextMenuRequested(const QPoint& point);

    void on_treeViewTags_customContextMenuRequested(const QPoint& point);
    void on_treeViewTags_doubleClicked(const QModelIndex& index);

    //Aliases buttons
    void on_toolButtonAddAlias_clicked();
    void on_toolButtonRemoveAlias_clicked();

    //NoteTags buttons
    void on_toolButtonAddTag_clicked();
    void on_toolButtonRemoveTag_clicked();

    void on_tableViewSearch_clicked(const QModelIndex& index);

    void selectNoteInTree();

    void switchTabToSearchResults();

    void switchTabToNote();

signals:
    void noteAdded(const Note* note, const QModelIndex& index);
    void noteRemoved(const Note* note, const QModelIndex& index);
    void noteOpened(const Note* note, const QModelIndex& index);
    void noteClosed(const Note* note, const QModelIndex& index);
    void noteEdited(const Note* note, const QModelIndex& index);
    void noteSaved(const Note* note, const QModelIndex& index);
    void noteRenamed(const Note* note, const QModelIndex& index);

    void tagDeleted(const QString& fulltag);

    void noteAliasAdded(const Note* note, const QString& alias);
    void noteAliasRemoved(const Note* note, const QString& alias);

    void noteTagAdded(const Note* note, const QString& tag);
    void noteTagRemoved(const Note* note, const QString& tag);

private:
    Ui::MainWindow* ui;

//    unique_ptr<RootNote> rootNote;
//    vector<unique_ptr<Note>> notes;
//    QHash<Id, NoteItem*> idToItem;

//    NoteItem* noteRootItem;
//    NoteItem* noteRealRootItem;
//    TagItem* tagRootItem;

    QPair<QHash<QString,Id>,QString> possibleLinks;

//    Note* currentNote;

    QTextDocument content;


    unique_ptr<TagTreeModel> tagTreeModel;

    unique_ptr<QSqlQueryModel> searchModel;
    unique_ptr<QSqlTableModel> aliasesModel;
    unique_ptr<QSqlRelationalTableModel> tagsModel;
    unique_ptr<QSqlRelationalTableModel> linkedFromModel;
    unique_ptr<QSqlRelationalTableModel> linksToModel;

    QSystemTrayIcon tray;
    QMenu trayMenu;
    QAction* minimizeAction;
    QAction* restoreAction;
    QAction* quitAction;
};

#endif // MAINWINDOW_H
