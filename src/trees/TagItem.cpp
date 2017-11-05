#include "TagItem.h"

#include <QMapIterator>

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
QString TagItem::getFullTag(QStringList& words) const
{
    words.append(word);
    if (parent) {
        parent->getFullTag(words);
    } else {
        std::reverse(words.begin(), words.end());
        return words.join(".");
    }
}
#pragma GCC diagnostic pop

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
