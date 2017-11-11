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
