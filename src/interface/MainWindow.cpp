#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDirModel>
#include <QHash>

#include "../database/Database.h"
#include "../database/Database.h"

#include <QSqlQuery>
#include <QSqlRecord>

#include "../entities/Note.h"

#include <QDebug>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Database db;
    db.openDatabase("/home/parsee/Projects/Active/QtSemanticNotes/test.sqlite");

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

    qDebug() << SQLNote::getAll().count();

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
