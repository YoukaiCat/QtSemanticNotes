#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../managers/NotesManager.h"
#include "../managers/TagsManager.h"
#include "../managers/AliasesManager.h"
#include "../managers/NoteTagsManager.h"
#include "../managers/LinksManager.h"
#include "../managers/SearchManager.h"
#include "../managers/TabsManager.h"

#include <QMainWindow>
#include <QMenu>
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

    //Tray
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

    void enableTagActions();
    void disableTagActions();
    void enableNoteActions();
    void disableNoteActions();
    void enableNodeEditWidgets();
    void disableNodeEditWidgets();
    void setWindowTitle(shared_ptr<Note> note);

    void addNote();
    void addSubnoteAtIndex(const QModelIndex& parentIndex);
    void addSubnote();
    void renameNoteAtIndex(const QModelIndex& index);
    void renameNote();
    void removeNoteAtIndex(const QModelIndex& index);
    void removeNote();
    void findNotesByWords();
    void showNoteIsModified();
    void showNoteIsNotModified();

    void removeTagAtIndex(const QModelIndex& index);
    void removeTag();

    void addNoteAlias();

    void enableRemoveAliasButton();
    void disableRemoveAliasButton();

    void addNoteTag();

    void enableRemoveNoteTagButton();
    void disableRemoveNoteTagButton();

    void switchTabToSearchResults();
    void switchTabToNote();

    void setTabTitle(shared_ptr<Note> note);

    void on_treeViewNotes_customContextMenuRequested(const QPoint& point);
    void on_treeViewTags_customContextMenuRequested(const QPoint& point);

    bool eventFilter(QObject *object, QEvent *event);

    void on_actionAboutQtSemanticNotes_triggered();
    void on_actionAboutQt_triggered();

private:
    Ui::MainWindow* ui;

    NotesManager notesManager;
    TagsManager tagsManager;
    AliasesManager aliasesManager;
    NoteTagsManager noteTagsManager;
    LinksManager linksManager;
    SearchManager searchManager;
    TabsManager tabsManager;

    QSystemTrayIcon tray;
    QMenu trayMenu;
    QAction* minimizeAction;
    QAction* restoreAction;
    QAction* quitAction;
};

#endif // MAINWINDOW_H
