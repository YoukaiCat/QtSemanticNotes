#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDirModel>
#include <QHash>

#include "../database/Database.h"
#include "../database/SQLiteDatabase.h"

#include <QSqlQuery>
#include <QSqlRecord>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SQliteDatabase db;
    db.openDatabase(":memory:");

    QHash<int, TreeItem*> NoteIdToNoteTreeItem;

    QSqlQuery q("select * from notes join relations");
    QSqlRecord rec = q.record();

    TreeItem * rootItem = new TreeItem("Title");
//    while (q.next()) {
//        //NoteIdToNoteTreeItem[]
//        if (NoteIdToNoteTreeItem.contains(q.value(1).toInt())) {
//            NoteIdToNoteTreeItem(q.value(1).toInt());
//        } else {

//        }
//    }

    noteTreeModel = new NoteTreeModel(rootItem);
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
