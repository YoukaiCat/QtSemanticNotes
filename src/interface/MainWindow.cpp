#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QHash>

#include <QSqlQuery>
#include <QSqlRecord>

#include <QMessageBox>

#include <QDebug>

#include "../database/Database.h"

#include "../entities/AbstractNote.h"
#include "../entities/Note.h"
#include "../entities/RootNote.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    try {
        Database db;
        db.openDatabase("/home/parsee/Projects/Active/QtSemanticNotes/test.sqlite");
        //db.openDatabase(":memory:");

        QHash<int, TreeItem*> IdToTreeItem;

        RootNote rootNote = RootNote::getRoot().value();

        TreeItem * rootItem = new TreeItem(rootNote.getTitle());

        qDebug() << Note::getAll().count();

        noteTreeModel = new NoteTreeModel(rootItem);
        ui->treeViewNotes->setModel(noteTreeModel);
    } catch (OpenDBException e) {
        QMessageBox::critical(this,
            "Error occured while opening or creating database",
            e.getMessage());
    } catch (QueryException e) {
        QMessageBox::critical(this,
            "SQL Query error occured",
            e.getMessage());
    } catch (RootUnsupportedActionException e) {
        QMessageBox::critical(this,
            "Internal error",
            e.getMessage());
    }
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
