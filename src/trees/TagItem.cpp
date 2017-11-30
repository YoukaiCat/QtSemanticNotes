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

#include "TagItem.h"

TagItem::TagItem()
    : parent(nullptr)
{}

TagItem::TagItem(const QString& word, TagItem* parent)
    : word(word),
      parent(parent)
{}

TagItem::~TagItem()
{
    qDeleteAll(children);
}

void TagItem::insert(QStringList& words)
{
    if (!words.empty()) {
        QString head = words.takeFirst();
        if (wordToChildren.contains(head)) {
            wordToChildren[head]->insert(words);
        } else {
            TagItem* item = new TagItem(head, this);
            children.append(item);
            wordToChildren.insert(head, item);
            item->insert(words);
        }
    }
}

QString TagItem::getFullTag(QStringList& words) const
{
    if (!word.isEmpty())
        words.append(word);
    if (parent != nullptr) {
        return parent->getFullTag(words);
    } else {
        std::reverse(words.begin(), words.end());
        return words.join('.');
    }
}

QString TagItem::getWord() const
{
    return word;
}

int TagItem::childNumber() const
{
    if (parent) {
        return parent->children.indexOf(const_cast<TagItem*>(this));
    } else {
        return 0;
    }
}

int TagItem::childCount() const
{
    return children.count();
}

void TagItem::removeChild(const int& index)
{
    children.removeAt(index);
}

TagItem* TagItem::getChild(const int& index) const
{
    return children.at(index);
}

TagItem* TagItem::getParent() const
{
    return parent;
}
