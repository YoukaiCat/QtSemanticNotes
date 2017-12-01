/*************************************************************************
 ** Copyright (C) 2017 Vladislav Mileshkin
 **
 ** This file is part of QtSemanticNotes.
 **
 ** QtSemanticNotes is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** QtSemanticNotes is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with QtSemanticNotes. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

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
