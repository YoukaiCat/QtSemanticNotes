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

#ifndef TAB_H
#define TAB_H

#include "History.h"
#include "LinksManager.h"

#include <QTabWidget>
#include <QTextBrowser>

class Tab : public History
{
    Q_OBJECT
public:
    explicit Tab(QObject* parent = nullptr);
    explicit Tab(shared_ptr<Note> note, QObject* parent = nullptr);
    ~Tab();

    QString getLabel();
    QWidget* getWidget() const;

    shared_ptr<Note> getNote() const;

    LinksManager* getLinksManager() const;
    void setLinksManager(LinksManager* linksManager);

    QTextBrowser* getBrowser() const;

signals:
    void modified();

public slots:
    void openNote(const shared_ptr<Note>& note);
    void saveNote();
    void closeNote();
    void noteRemoved(shared_ptr<Note> note);

    void switchMode();

private:
    void enableViewMode();
    void enableEditMode();
    void chooseModeToEnable();
    void setupBrowser();
    void saveLinks();

    shared_ptr<Note> note;
    QTextDocument editDoc;
    QTextDocument viewDoc;
    unique_ptr<QTextBrowser> browser;
    LinksManager* manager;
    void maybeSaveNote();
    void setCursorPosition(int cursorPosition);
};

#endif // TAB_H
