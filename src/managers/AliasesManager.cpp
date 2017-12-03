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

#include "AliasesManager.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>

using std::make_unique;

AliasesManager::AliasesManager(QObject* parent)
    : QObject(parent)
{}

void AliasesManager::setup(QTableView* view)
{
    this->view = view;
    model = createModel();
    setupView(view, model.get());

    connect(this, &AliasesManager::aliasAdded,
            model.get(), &QSqlTableModel::select);
    connect(this, &AliasesManager::aliasRemoved,
            model.get(), &QSqlTableModel::select);
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &AliasesManager::onSelectionChange);
}

void AliasesManager::clearModel()
{
    model->clear();
}

void AliasesManager::filterModel(shared_ptr<Note> note)
{
    QString idFilter = QString("note_id = '%1'").arg(note->getId().toString());
    model->setFilter(idFilter);
    selectFromModel();
}

void AliasesManager::selectFromModel()
{
    model->select();
}

void AliasesManager::addAlias(shared_ptr<Note> note, const QString& alias)
{
    Note::addNoteAlias(note.get(), alias);
    emit aliasAdded(note->getId());
}

Id AliasesManager::noteIdByIndex(const QModelIndex& index)
{
    return model->record(index.row()).value(2).toString();
}

void AliasesManager::removeAlias(const QModelIndex& index)
{
    auto noteId = noteIdByIndex(index);
    model->removeRows(index.row(), 1);
    emit aliasRemoved(noteId);
}

void AliasesManager::removeSelectedAliases()
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    for(QModelIndex& index : indexes) {
        removeAlias(index);
    }
}

void AliasesManager::onSelectionChange()
{
    if(view->selectionModel()->selectedIndexes().size() > 0) {
        emit aliasSelected();
    } else {
        emit aliasNotSelected();
    }
}

unique_ptr<QSqlTableModel> AliasesManager::createModel()
{
    auto model = make_unique<QSqlTableModel>();
    model->setTable("aliases");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    return model;
}

void AliasesManager::setupView(QTableView* view, QSqlTableModel* model)
{
    view->setModel(model);
    view->hideColumn(0);
    view->hideColumn(2);
    view->hideColumn(3);
}
