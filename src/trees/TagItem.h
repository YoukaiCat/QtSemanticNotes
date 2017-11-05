#ifndef TAGITEM_H
#define TAGITEM_H

#include "../entities/Tag.h"

#include <QList>
#include <QVariant>

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

    void insert(QStringList& words);
    QString getFullTag() const;

private:
    QString word;
    QMap<QString, TagItem*> wordToChildren;
    QList<TagItem*> children;
    TagItem* parent;
};

#endif // TAGITEM_H
