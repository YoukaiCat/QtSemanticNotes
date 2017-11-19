#ifndef NOTEMANAGER_H
#define NOTEMANAGER_H

#include "../entities/RootNote.h"
#include "../models/NoteTreeModel.h"

#include <tuple>
using std::tuple;

class NoteManager : public QObject
{
    Q_OBJECT
public:
    explicit NoteManager(QObject* parent = nullptr);

signals:

public slots:

private:
    void createItemsAndAddToMap(const vector<shared_ptr<Note>> & notes,
                                QHash<Id, NoteItem*> & itemByNoteId);
    void setItemChilds(const vector<shared_ptr<Note>> & notes,
                       const QHash<Id, NoteItem*> & itemByNoteId);
    void setupRootChildren(QHash<Id, NoteItem*> & itemByNoteId);
    void setupTree();

    unique_ptr<NoteTreeModel> noteTreeModel;

};

#endif // NOTEMANAGER_H
