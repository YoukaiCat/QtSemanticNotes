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

#ifndef TABSMANAGER_H
#define TABSMANAGER_H

#include "Tab.h"

#include <QTabWidget>

#include <unordered_map>
#include <memory>
using std::unique_ptr;
using std::unordered_map;

class TabsManager : public QObject
{
    Q_OBJECT
public:
    explicit TabsManager(QObject* parent = nullptr);
    void setup(QTabWidget* tabWidget,
               LinksManager* linksManager);
    void setupNavigation(QAction* backwardAction, QAction* forwardAction);

signals:
    void modified();
    void noteOpenedFromHistory(shared_ptr<Note> note);

public slots:
    void openNote(shared_ptr<Note> note);
    void saveNote();
    void closeNote();
    void removeNote(shared_ptr<Note> note);

    void switchMode();

    //void openInCurrentTab(shared_ptr<Note> note);
    //void closeTab(shared_ptr<Note> note);

    //void onCurrentChanged(int index);
    //void onTabCloseRequested(int index);

private:
    //Tab* findOrCreate(shared_ptr<Note> note);
    //Tab* addNewTab(shared_ptr<Note> note);

    unique_ptr<Tab> current;
//    optional<Tab*> current;
//    unordered_map<shared_ptr<Note>, Tab*> tabByNote;
//    unordered_map<int, Tab*> tabByIndex;
    QTabWidget* tabWidget;
    LinksManager* linksManager;
};

#endif // TABSMANAGER_H
