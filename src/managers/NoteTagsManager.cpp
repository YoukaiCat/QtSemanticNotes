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

#include "NoteTagsManager.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>

#include <QDebug>

using std::make_unique;

NoteTagsManager::NoteTagsManager(QObject* parent)
    : QObject(parent)
{}

void NoteTagsManager::setup(QTableView* view)
{
    this->view = view;
    model = createModel();
    setupView(view, model.get());

    connect(this, &NoteTagsManager::noteTagAdded,
            model.get(), &QSqlTableModel::select);
    connect(this, &NoteTagsManager::noteTagRemoved,
            model.get(), &QSqlTableModel::select);

    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &NoteTagsManager::onSelectionChange);
}

void NoteTagsManager::clearModel()
{
    model->clear();
}

void NoteTagsManager::selectFromModel()
{
    model->select();
}

void NoteTagsManager::filterModel(shared_ptr<Note> note)
{
    QString idFilter = QString("note_id = '%1'").arg(note->getId().toString());
    model->setFilter(idFilter);
    selectFromModel();
}

void NoteTagsManager::addNoteTag(shared_ptr<Note> note, const QString& name)
{
    auto tag = Tag::getOrCreate(name);
    Tag::addNoteTag(note.get(), tag.get());
    emit noteTagAdded();
}

void NoteTagsManager::removeTagByIndex(const QModelIndex& index)
{
    model->removeRows(index.row(), 1);
}

void NoteTagsManager::removeNoteTag(const QModelIndex& index)
{
    removeTagByIndex(index);
    emit noteTagRemoved();
}

void NoteTagsManager::removeSelectedNoteTags()
{
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    for(QModelIndex& index : indexes) {
        removeTagByIndex(index);
    }
    emit noteTagRemoved();
}

void NoteTagsManager::onSelectionChange()
{
    if(view->selectionModel()->selectedIndexes().size() > 0) {
        emit noteTagSelected();
    } else {
        emit noteTagNotSelected();
    }
}

unique_ptr<QSqlRelationalTableModel> NoteTagsManager::createModel()
{
    auto model = make_unique<QSqlRelationalTableModel>();
    model->setTable("note_tags");
    model->setRelation(2, QSqlRelation("tags", "id", "name"));
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    return model;
}

void NoteTagsManager::setupView(QTableView* view, QSqlRelationalTableModel* model)
{
    view->setModel(model);
    view->hideColumn(0);
    view->hideColumn(1);
    view->hideColumn(3);
    view->hideColumn(4);
}
