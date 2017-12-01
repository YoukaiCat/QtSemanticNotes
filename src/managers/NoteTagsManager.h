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

#ifndef NOTETAGSMANAGER_H
#define NOTETAGSMANAGER_H

#include "../entities/Note.h"
#include "../entities/Tag.h"

#include <QSqlRelationalTableModel>
#include <QTableView>

#include <memory>
using std::unique_ptr;
using std::shared_ptr;

class NoteTagsManager : public QObject
{
    Q_OBJECT
public:
    explicit NoteTagsManager(QObject* parent = nullptr);

signals:
    void noteTagAdded();
    void noteTagRemoved();
    void noteTagSelected();
    void noteTagNotSelected();

public slots:
    void setup(QTableView* view);

    void clearModel();
    void filterModel(shared_ptr<Note> note);
    void selectFromModel();

    void addNoteTag(shared_ptr<Note> note, const QString& name);
    void removeNoteTag(const QModelIndex& index);
    void removeSelectedNoteTags();

private slots:
    void onSelectionChange();

private:
    unique_ptr<QSqlRelationalTableModel> createModel();
    void setupView(QTableView* view, QSqlRelationalTableModel* model);
    void removeTagByIndex(const QModelIndex& index);

    unique_ptr<QSqlRelationalTableModel> model;
    QTableView* view;
};

#endif // NOTETAGSMANAGER_H
