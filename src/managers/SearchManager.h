#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include "../entities/Id.h"

#include <QSqlQueryModel>
#include <QTableView>

#include <memory>
using std::unique_ptr;

class SearchManager : public QObject
{
    Q_OBJECT
public:
    explicit SearchManager(QObject* parent = nullptr);

signals:
    void resultReady();
    void resultSelected(Id noteId);

public slots:
    void setup(QTableView* view);

    void findNotes(const QString& words);
    void findNotesByTag(const QString& fulltag);

private:
    void setupView(QTableView* view, QSqlQueryModel* model);
    void indexClicked(const QModelIndex& index);

    unique_ptr<QSqlQueryModel> model;
    QTableView* view;
};

#endif // SEARCHMANAGER_H
