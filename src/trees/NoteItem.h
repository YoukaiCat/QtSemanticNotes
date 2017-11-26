#ifndef NOTEITEM_H
#define NOTEITEM_H

#include "../entities/Note.h"

#include <QList>

using std::unique_ptr;

class NoteItem
{
public:
    explicit NoteItem(shared_ptr<Note> note);
    ~NoteItem();

    void addChild(NoteItem* item);
    void addChildAndUpdateNoteParent(NoteItem* item);

    int childNumber() const;
    int childCount() const;

    NoteItem* getChild(const int& index) const;
    NoteItem* getParent() const;

    void removeChild(const int& index);

    shared_ptr<Note> getValue() const;

private:
    shared_ptr<Note> value;
    QList<NoteItem*> children;
    NoteItem* parent;
};

#endif // NOTEITEM_H
