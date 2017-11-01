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

class NoteTreeItem
{
public:
    explicit NoteTreeItem(const QString& header);
    explicit NoteTreeItem(RootNote* note, NoteTreeItem* parent = 0);
    explicit NoteTreeItem(Note* note, NoteTreeItem* parent = 0);

    NoteTreeItem* child(int number);

    int childCount() const;
    int columnCount() const;

    QVariant data(int column) const;

    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);

    NoteTreeItem* parent();

    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);

    int childNumber() const;

    bool setData(int column, const QVariant& value);

    QString header;
    variant<RootNote*, Note*> note;
    QList<NoteTreeItem*> subnotes;
    NoteTreeItem* parentItem;
};

#endif // NOTETREEITEM_H
