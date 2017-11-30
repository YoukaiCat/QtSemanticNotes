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
 ** along with QtSemanticNotesr. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

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
