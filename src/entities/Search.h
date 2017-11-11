#ifndef SEARCH_H
#define SEARCH_H

#include "Note.h"
#include "Tag.h"

#include <QSqlQuery>

class Search
{
public:
    static QSqlQuery findNotesByTag(const QString& fulltag);
};

#endif // SEARCH_H
