#ifndef NOTEITEM_H
#define NOTEITEM_H

#include "../entities/Note.h"

#include <QList>
#include <QVariant>

class NoteItem
{
public:
    explicit NoteItem(Note* note, NoteItem* parent = nullptr);
    ~NoteItem();

    void addChild(NoteItem* item);
    void addChildAndUpdateNoteParent(NoteItem* item);

    NoteItem* getChild(const int& index) const;
    NoteItem* getParent() const;

    int childNumber() const;
    int childCount() const;

    void removeChild(const int& index);

    Note* getValue() const;

private:
    Note* value;
    QList<NoteItem*> children;
    NoteItem* parent;
};

#endif // NOTEITEM_H
