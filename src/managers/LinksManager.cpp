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

#include "LinksManager.h"

#include "../entities/Search.h"

#include <QRegularExpression>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>

using std::make_unique;

LinksManager::LinksManager(QObject *parent)
    : QObject(parent)
{}

void LinksManager::setup(QTableView* linkedFromView, QTableView* hasLinksToView)
{
    this->linkedFromView = linkedFromView;
    this->hasLinksToView = hasLinksToView;
    linkedFromModel = createLinkedFromModel();
    setupViewLinkedFrom(linkedFromView, linkedFromModel.get());
    hasLinksToModel = createHasLinksToModel();
    setupViewHasLinksTo(hasLinksToView, hasLinksToModel.get());

//    connect(linkedFromView->selectionModel(), &QItemSelectionModel::selectionChanged,
//            this, &LinksManager::onLinkedFromSelectionChange);
//    connect(linkedFromView->selectionModel(), &QItemSelectionModel::selectionChanged,
//            this, &LinksManager::onLinkedFromSelectionChange);
}

void LinksManager::clearModel()
{
    linkedFromModel->clear();
    hasLinksToModel->clear();
}

void LinksManager::selectFromModel()
{
    linkedFromModel->select();
    hasLinksToModel->select();
}

void LinksManager::filterModel(shared_ptr<Note> note)
{
    QString fromIdFilter = QString("from_note_id = '%1'").arg(note->getId().toString());
    QString toIdFilter = QString("to_note_id = '%1'").arg(note->getId().toString());
    linkedFromModel->setFilter(toIdFilter);
    hasLinksToModel->setFilter(fromIdFilter);
    selectFromModel();
}

void LinksManager::updateLinks(shared_ptr<Note> note)
{
    QList<Id> links = findLinks(note->getContent());
    Note::clearLinksFrom(note->getId());
    for(auto& id : links) {
        Note::addNoteLink(note->getId(), id);
    }
    emit linksUpdated();
}

void LinksManager::loadPossibleLinks()
{
    possibleLinks = Note::getPossibleLinks();
    emit possibleLinksLoaded();
}

void LinksManager::updateBackLinks(shared_ptr<Note> note)
{
    updateBackLinksByTitleAndId(note->getTitle(), note->getId());
}

void LinksManager::updateBackLinksByTitleAndId(const QString& title, Id noteId)
{
    //old links
    Note::clearLinksTo(noteId);
    //new links
    Q_UNUSED(noteId)
    loadPossibleLinks();
    QSqlQuery q = Search::findNotesByContent(title);
    while(q.next()) {
        Id hasLinkId = q.value(0).toString();
        QString content = q.value(1).toString();
        QList<Id> links = findLinks(content);
        Note::clearLinksFrom(hasLinkId);
        for(auto& id : links) {
            Note::addNoteLink(hasLinkId, id);
        }
    }
    emit backLinksUpdated();
}

//void LinksManager::onLinkedFromSelectionChange()
//{
//    QModelIndexList indexes = linkedFromView->selectionModel()->selectedIndexes();
//    if(!indexes.isEmpty()) {

//    }
//}

//void LinksManager::onLinksToSelectionChange()
//{
//    QModelIndexList indexes = linkedFromView->selectionModel()->selectedIndexes();
//    if(!indexes.isEmpty()) {

//    }
//}

unique_ptr<QSqlRelationalTableModel> LinksManager::createLinkedFromModel()
{
    auto linkedFromModel = make_unique<QSqlRelationalTableModel>();
    linkedFromModel->setTable("links");
    linkedFromModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    linkedFromModel->setRelation(1, QSqlRelation("notes", "id", "title"));
    return linkedFromModel;
}

void LinksManager::setupViewLinkedFrom(QTableView* view, QSqlRelationalTableModel* model)
{
    view->setModel(model);
    view->hideColumn(0);
    view->hideColumn(2);
    view->hideColumn(3);
}

unique_ptr<QSqlRelationalTableModel> LinksManager::createHasLinksToModel()
{
    auto hasLinksToModel = make_unique<QSqlRelationalTableModel>();
    hasLinksToModel->setTable("links");
    hasLinksToModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    hasLinksToModel->setRelation(2, QSqlRelation("notes", "id", "title"));
    return hasLinksToModel;
}

void LinksManager::setupViewHasLinksTo(QTableView* view, QSqlRelationalTableModel* model)
{
    view->setModel(model);
    view->hideColumn(0);
    view->hideColumn(1);
    view->hideColumn(3);
}

QString LinksManager::makeLinks(QString rightPart)
{
    QStringList parts;
    QString leftPart;
    QRegularExpression titlesRegex("(" + possibleLinks.second + ")", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = titlesRegex.match(rightPart);
    while(match.hasMatch()) {
        QString title = match.captured(0);
        QString fixedTitle = QLocale::system().toLower(title);
        fixedTitle = QRegularExpression::escape(fixedTitle);
        Q_ASSERT(possibleLinks.first.contains(fixedTitle));
        Id id = possibleLinks.first.value(fixedTitle);
        QString link = QString("<a href='qtsemanticnotes://0.0.0.0/?%1'>%2</a>").arg(id.toString()).arg(title);
        int position = match.capturedStart() + match.capturedLength();
        leftPart = rightPart.left(position);
        rightPart = rightPart.mid(position);
        leftPart.replace(match.capturedStart(), match.capturedLength(), link);
        parts.append(leftPart);
        match = titlesRegex.match(rightPart);
    }
    parts.append(rightPart);
    return parts.join("");
}

QList<Id> LinksManager::findLinks(const QString& text)
{
    QList<Id> linkedNotes;
    QRegularExpression titlesRegex("(" + possibleLinks.second + ")", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = titlesRegex.globalMatch(text);
    while(i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString title = match.captured(0);
        title = QLocale::system().toLower(title);
        title = QRegularExpression::escape(title);
        if(possibleLinks.first.contains(title)) {
            linkedNotes.append(possibleLinks.first.value(title));
        }
    }
    return linkedNotes;
}
