#include "SearchManager.h"

#include "../entities/Search.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>

using std::make_unique;

SearchManager::SearchManager(QObject* parent)
    : QObject(parent)
{}

void SearchManager::setup(QTableView* view)
{
    this->view = view;
    model = make_unique<QSqlQueryModel>();
    setupView(view, model.get());

    connect(view, &QTableView::clicked,
            this, &SearchManager::indexClicked);
}

void SearchManager::findNotes(const QString& words)
{
    QSqlQuery query = Search::findNotesByWords(words);
    model->setQuery(query);
    model->setHeaderData(1, Qt::Horizontal, tr("Note Title"));
    view->setColumnHidden(0, true);
    emit resultReady();
}

void SearchManager::findNotesByTag(const QString& fulltag)
{
    QSqlQuery query = Search::findNotesByTag(fulltag);
    model->setQuery(query);
    model->setHeaderData(1, Qt::Horizontal, tr("Note Title"));
    view->setColumnHidden(0, true);
    emit resultReady();
}

void SearchManager::setupView(QTableView* view, QSqlQueryModel* model)
{
    view->setModel(model);
}

void SearchManager::indexClicked(const QModelIndex& index)
{
    QSqlRecord note = model->record(index.row());
    uint id = note.value("id").toUInt();
    emit resultSelected(id);
}
