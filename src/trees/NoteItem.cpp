#include "NoteItem.h"

NoteItem::NoteItem(shared_ptr<Note> value)
    : value(move(value))
{}

void NoteItem::addChild(unique_ptr<NoteItem> && item)
{
    children.append(move(item));
    item->parent = this;
}

void NoteItem::addChildAndUpdateNoteParent(unique_ptr<NoteItem> && item)
{
    addChild(move(item));
    uint id = this->getValue()->getId();
    auto note = item->getValue();
    note->setParentId(id);
    note->update();
}

int NoteItem::childNumber() const
{
    if (parent) {
        auto it = std::find_if(parent->children.begin(),
                               parent->children.end(),
                               [this](auto item){
            return item.get() == this;
        });
        return std::distance(parent->children.begin(), it);
    } else {
        return 0;
    }
}

int NoteItem::childCount() const
{
    return children.count();
}

//unique_ptr<NoteItem> NoteItem::takeAt(const int& index)
//{
//    return children.takeAt(index);
//}

void NoteItem::removeChild(const int& index)
{
    children.removeAt(index);
}

NoteItem* NoteItem::getChild(const int& index) const
{
    return children.at(index).get();
}

NoteItem* NoteItem::getParent() const
{
    return parent;
}

shared_ptr<Note> NoteItem::getValue() const
{
    return value;
}
