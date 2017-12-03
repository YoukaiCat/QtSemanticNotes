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

#ifndef ALIASESMANAGER_H
#define ALIASESMANAGER_H

#include "../entities/Note.h"

#include <QSqlTableModel>
#include <QTableView>

#include <memory>
using std::unique_ptr;
using std::shared_ptr;

class AliasesManager : public QObject
{
    Q_OBJECT
public:
    explicit AliasesManager(QObject* parent = nullptr);

signals:
    void aliasAdded(Id noteId);
    void aliasRemoved(Id noteId);
    void aliasSelected();
    void aliasNotSelected();

public slots:
    void setup(QTableView* view);

    void clearModel();
    void filterModel(shared_ptr<Note> note);
    void selectFromModel();

    void addAlias(shared_ptr<Note> note, const QString& alias);
    void removeAlias(const QModelIndex& index);
    void removeSelectedAliases();

private slots:
    void onSelectionChange();

private:
    unique_ptr<QSqlTableModel> createModel();
    void setupView(QTableView* view, QSqlTableModel* model);
    QString aliasByIndex(const QModelIndex& index);
    Id noteIdByIndex(const QModelIndex& index);

    unique_ptr<QSqlTableModel> model;
    QTableView* view;
};

#endif // ALIASESMANAGER_H
