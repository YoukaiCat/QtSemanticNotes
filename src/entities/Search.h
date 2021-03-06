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

#ifndef SEARCH_H
#define SEARCH_H

#include "Note.h"
#include "Tag.h"

#include <QSqlQuery>

class Search
{
public:
    static QSqlQuery findNotesByWords(const QString& words);
    static QSqlQuery findNotesByContent(const QString& words);
    static QSqlQuery findNotesByTag(const QString& fulltag);
};

#endif // SEARCH_H
