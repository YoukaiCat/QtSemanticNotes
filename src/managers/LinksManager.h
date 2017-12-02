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

#ifndef LINKSMANAGER_H
#define LINKSMANAGER_H

#include "../entities/Note.h"

#include <QSqlRelationalTableModel>
#include <QTableView>

#include <memory>
using std::unique_ptr;
using std::shared_ptr;

class LinksManager : public QObject
{
    Q_OBJECT
public:
    explicit LinksManager(QObject* parent = nullptr);
    QString makeLinks(QString rightPart);
    void loadPossibleLinks();

signals:
    void possibleLinksLoaded();
    void linksUpdated();
    void backLinksUpdated();
    void linkClicked(Id noteId);

public slots:
    void setup(QTableView* linkedFromView,
               QTableView* hasLinksToView);

    void clearModel();
    void filterModel(shared_ptr<Note> note);
    void selectFromModel();

    void updateLinks(shared_ptr<Note> note);
    void updateBackLinks(shared_ptr<Note> note);
    void updateBackLinksByTitleAndId(const QString& title, Id noteId);

private slots:
    void onLinkedFromDoubleClicked(const QModelIndex& index);
    void onLinksToDoubleClicked(const QModelIndex& index);

private:
    unique_ptr<QSqlRelationalTableModel> createLinkedFromModel();
    void setupViewLinkedFrom(QTableView* view, QSqlRelationalTableModel* model);
    unique_ptr<QSqlRelationalTableModel> createHasLinksToModel();
    void setupViewHasLinksTo(QTableView* view, QSqlRelationalTableModel* model);

    unique_ptr<QSqlRelationalTableModel> linkedFromModel;
    unique_ptr<QSqlRelationalTableModel> hasLinksToModel;
    QTableView* linkedFromView;
    QTableView* hasLinksToView;

    QList<Id> findLinks(const QString& text);

    QPair<QHash<QString, QString>, QString> possibleLinks;
};

#endif // LINKSMANAGER_H
