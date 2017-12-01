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

#ifndef TAGITEM_H
#define TAGITEM_H

#include <QList>
#include <QMap>

class TagItem
{
public:
    explicit TagItem();
    explicit TagItem(const QString& word, TagItem* parent);
    ~TagItem();

    TagItem* getChild(const int& index) const;
    TagItem* getParent() const;

    int childNumber() const;
    int childCount() const;

    void removeChild(const int& index);

    QString getWord() const;

    void insert(QStringList& words);
    QString getFullTag(QStringList& words) const;

private:
    QString word;
    QMap<QString, TagItem*> wordToChildren;
    QList<TagItem*> children;
    TagItem* parent;
};

#endif // TAGITEM_H
