#ifndef NOTEITEM_H
#define NOTEITEM_H

#include "../entities/RootNote.h"
#include "../entities/Note.h"

#include <QList>
#include <QVariant>

#include <vector>
#include <variant>
using std::vector;
using std::variant;
using std::monostate;

class NoteItem
{
public:
    explicit NoteItem(NoteItem* parent = nullptr);
    explicit NoteItem(RootNote* note, NoteItem* parent = nullptr);
    explicit NoteItem(Note* note, NoteItem* parent = nullptr);
    ~NoteItem();

    void addChild(NoteItem* item);
    void addChildAndUpdateNoteParent(NoteItem* item);

    NoteItem* getChild(const int& index) const;
    NoteItem* getParent() const;

    int childNumber() const;
    int childCount() const;

    void removeChild(const int& index);

    AbstractNote* getAsAbstractNote() const;
    RootNote* getAsRoot() const;
    Note* getAsNote() const;

private:
    variant<monostate, RootNote*, Note*> value;
    QList<NoteItem*> children;
    NoteItem* parent;
};

#endif // NOTEITEM_H
