#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../models/NoteTreeModel.h"

#include <QMainWindow>
#include <QMenu>

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

    void on_treeViewNotes_customContextMenuRequested(const QPoint & point);

private:
    Ui::MainWindow* ui;

    unique_ptr<RootNote> rootNote;
    vector<unique_ptr<Note>> notes;

    NoteTreeItem* headerItem;
    QHash<Id, NoteTreeItem*> idToTreeItem;
    NoteTreeItem* rootItem;

    unique_ptr<NoteTreeModel> noteTreeModel;

    QMenu notesContextMenu;
    QMenu notesRootContextMenu;
    NoteTreeItem* selectedItem;
};

#endif // MAINWINDOW_H
