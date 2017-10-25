#ifndef NOTE_H
#define NOTE_H

#include "NoteEntity.h"
#include "SQLitePersistentEntity.h"

class Note : public NoteEntity, SQLitePersistentEntity
{
public:
    Note(QObject* parent = nullptr);
};

#endif // NOTE_H
