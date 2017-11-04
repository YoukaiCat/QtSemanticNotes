#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../models/NoteTreeModel.h"

#include <QMainWindow>
#include <QMenu>

#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    void on_actionAboutQt_triggered();

    void openNote(AbstractNote* note);

    void on_toolButtonAddAlias_clicked();
    void on_toolButtonRemoveAlias_clicked();

    void on_toolButtonAddTag_clicked();
    void on_toolButtonRemoveTag_clicked();

    void on_treeViewNotes_customContextMenuRequested(const QPoint & point);
    void on_treeViewNotes_clicked(const QModelIndex &index);

private:
    Ui::MainWindow* ui;

    unique_ptr<RootNote> rootNote;
    vector<unique_ptr<Note>> notes;

    NoteItem* headerItem;
    QHash<Id, NoteItem*> idToItem;
    NoteItem* rootItem;

    unique_ptr<NoteTreeModel> noteTreeModel;

    QMenu notesContextMenu;
    QMenu notesRootContextMenu;
    QModelIndex selectedIndex;
    NoteItem* selectedItem;

    AbstractNote* currentNote;

    unique_ptr<QSqlQueryModel> searchModel;
    unique_ptr<QSqlTableModel> aliasesModel;
    unique_ptr<QSqlRelationalTableModel> tagsModel;
    unique_ptr<QSqlRelationalTableModel> linkedFromModel;
    unique_ptr<QSqlRelationalTableModel> linksToModel;
};

#endif // MAINWINDOW_H
