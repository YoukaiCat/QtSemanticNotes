#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../models/NoteTreeModel.h"

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

    NoteTreeModel* noteTreeModel;
};

#endif // MAINWINDOW_H
