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

#include "Tab.h"

#include <QDebug>

using std::make_unique;

Tab::Tab(QObject* parent)
    : History(parent),
      browser(make_unique<QTextBrowser>())
{
    setupBrowser();
    connect(&editDoc, &QTextDocument::contentsChanged,
            this, &Tab::modified);
}

Tab::Tab(shared_ptr<Note> note, QObject* parent)
    : History(parent),
      note(note),
      browser(make_unique<QTextBrowser>())
{
    setupBrowser();
    connect(&editDoc, &QTextDocument::contentsChanged,
            this, &Tab::modified);
}

Tab::~Tab()
{
    maybeSaveNote();
}

QString Tab::getLabel()
{
    return note->getTitle();
}

QWidget* Tab::getWidget() const
{
    return browser.get();
}

shared_ptr<Note> Tab::getNote() const
{
    return note;
}

void Tab::maybeSaveNote()
{
    if (note.get() != nullptr &&
            !note->isDeleted() &&
            editDoc.isModified()) {
        saveNote();
    }
}

void Tab::openNote(const shared_ptr<Note>& value)
{
    maybeSaveNote();
    note = value;
    editDoc.setPlainText(note->getContent());
    chooseModeToEnable();
    browser->setEnabled(true);
    History::noteOpened(value);
}

void Tab::saveNote()
{
    note->setContent(editDoc.toPlainText());
    note->update();
    saveLinks();
}

void Tab::closeNote()
{
    browser->setEnabled(false);
}

void Tab::noteRemoved(shared_ptr<Note> note)
{
    History::noteRemoved(note);
}

void Tab::switchMode()
{
    if (browser->isReadOnly()) {
        enableEditMode();
    } else {
        enableViewMode();
    }
}

void Tab::enableViewMode()
{
    viewDoc.setHtml(manager->makeLinks(editDoc.toHtml()));
    browser->setDocument(&viewDoc);
    browser->setReadOnly(true);
}

void Tab::enableEditMode()
{
    //Hack to fix a bug (underlined text style is retained
    //after opening empty text document by a link)
    browser->setDocument(nullptr);
    browser->setDocument(&editDoc);
    browser->setReadOnly(false);
}

void Tab::chooseModeToEnable()
{
    if (note->getContent().size() == 0) {
        enableEditMode();
    } else {
        enableViewMode();
    }
    editDoc.setModified(false);
}

void Tab::setupBrowser()
{
    browser->setFrameShape(QFrame::NoFrame);
    browser->setLineWidth(0);
    browser->setUndoRedoEnabled(true);
    browser->setReadOnly(true);
    browser->setOpenExternalLinks(true);
    browser->setEnabled(false);
}

void Tab::saveLinks()
{
    manager->updateLinks(note);
}

QTextBrowser* Tab::getBrowser() const
{
    return browser.get();
}

LinksManager* Tab::getLinksManager() const
{
    return manager;
}

void Tab::setLinksManager(LinksManager* value)
{
    manager = value;
}
