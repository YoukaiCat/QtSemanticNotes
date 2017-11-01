#ifndef NOTETREEITEM_H
#define NOTETREEITEM_H

#include "../entities/RootNote.h"
#include "../entities/Note.h"

#include <QList>
#include <QVariant>

#include <vector>
#include <variant>
using std::vector;
using std::variant;
using std::monostate;

class NoteTreeItem
{
public:
    explicit NoteTreeItem(NoteTreeItem* parent = 0);
    explicit NoteTreeItem(RootNote* note, NoteTreeItem* parent = 0);
    explicit NoteTreeItem(Note* note, NoteTreeItem* parent = 0);

    void addSubnote(NoteTreeItem* note);

//    bool insertChildren(int position, int count, int columns);
//    bool removeChildren(int position, int count);

    int childNumber() const;

    variant<monostate, RootNote*, Note*> note;
    QList<NoteTreeItem*> subnotes;
    NoteTreeItem* parentItem;
};

#endif // NOTETREEITEM_H
