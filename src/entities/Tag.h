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
 ** along with QtSemanticNotes. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

#ifndef TAG_H
#define TAG_H

#include "Id.h"
#include "Note.h"

#include <QString>
#include <QDateTime>

#include <vector>
#include <optional>
#include <memory>
using std::vector;
using std::optional;
using std::unique_ptr;
using std::move;

class Tag
{
public:
    static shared_ptr<Tag> create(const QString& name);
    static shared_ptr<Tag> getOrCreate(const QString& name);
    static vector<shared_ptr<Tag>> getAll();
    static optional<shared_ptr<Tag>> getById(const Id& id);
    static optional<shared_ptr<Tag>> getByName(const QString& name);

    Id getId() const;

    QString getName() const;
    void setName(const QString& name);

    QDateTime getCreatedAt() const;

    QString toString() const;

    void remove();

    Tag(const Tag&) = delete;
    Tag & operator=(const Tag&) = delete;
    Tag(Tag&& other) noexcept = delete;
    Tag & operator=(Tag&& other) noexcept = delete;

    static void addNoteTag(const Note* note, const Tag* tag);

    static void deleteTagAndSubtags(const QString& fulltag);

private:
    Tag(const Id& id,
        const QString& name,
        const QDateTime& createdAt);

    Id id;
    QString name;
    QDateTime createdAt;
};

#endif // TAG_H
