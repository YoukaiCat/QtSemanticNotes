/*************************************************************************
 ** Copyright (C) 2017 Vladislav Mileshkin
 **
 ** This file is part of QtSemanticNotes.
 **
 ** QtSemanticNotes is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** QtSemanticNotes is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with QtSemanticNotesr. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#ifndef ROOTNOTE_H
#define ROOTNOTE_H

#include "Note.h"

class RootNote : public Note
{
public:
    static shared_ptr<RootNote> getRootNote();

    virtual Id getParentId() const override;
    virtual void setParentId(const Id& value) override;

    virtual QString toString() const override;

    void update() override;
    void remove() override;

    RootNote(const RootNote&) = delete;
    RootNote & operator=(const RootNote&) = delete;
    RootNote(RootNote&& other) noexcept = delete;
    RootNote & operator=(RootNote&& other) noexcept = delete;

private:
    RootNote(const Id& id,
             const QString& title,
             const QDateTime& createdAt,
             const QDateTime& updatedAt);
};

#endif // ROOTNOTE_H
