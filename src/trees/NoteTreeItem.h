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
    ~NoteTreeItem();

    void addSubnote(NoteTreeItem* note);

    void addSubnoteAndUpdateParent(NoteTreeItem* note);

    int subnoteNumber() const;

    NoteTreeItem* getSubnote(const int& index) const;

//    QString getNoteTitle() const;

//    QString getNoteContent() const;

    AbstractNote* getNote();
    RootNote* getAsRoot();
    Note* getAsNote();

    variant<monostate, RootNote*, Note*> note;
    QList<NoteTreeItem*> subnotes;
    NoteTreeItem* parentItem;
};

#endif // NOTETREEITEM_H
