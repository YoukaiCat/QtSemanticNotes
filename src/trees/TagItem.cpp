#include "TagItem.h"

#include <QMapIterator>

TagItem::TagItem()
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
        if (wordToChildren.contains(word)) {
            wordToChildren[head]->insert(words);
        } else {
            TagItem* item = new TagItem(word, this);
            wordToChildren.insert(head, item);
            children.append(item);
            item->insert(words);
        }
    }
}

QString TagItem::getFullTag() const
{

}

int TagItem::childNumber() const
{
    int i = 0;
    QMapIterator<QString, TagItem*> iter(parent->children);
    while (iter.hasNext()) {
        iter.next();
        i++;
    }
    return i;
}

int TagItem::childCount() const
{
    return children.count();
}

void TagItem::removeChild(const int& index)
{
    int i = 0;
    QMapIterator<QString, TagItem*> iter(children);
    while (iter.hasNext()) {
        iter.next();
        if(i == index) {
            children.remove(iter.key());
            break;
        } else {
            i++;
        }
    }
}

TagItem* TagItem::getChild(const int& index) const
{
    int i = 0;
    QMapIterator<QString, TagItem*> iter(children);
    while (iter.hasNext()) {
        iter.next();
        if(i == index) {
            return iter.value();
        } else {
            i++;
        }
    }
    throw "";
}

TagItem* TagItem::getParent() const
{
    return parent;
}
