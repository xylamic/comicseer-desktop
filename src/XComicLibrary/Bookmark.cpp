/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Bookmark.hpp"

using namespace ComicSeer;
using namespace boost::uuids;

Bookmark::Bookmark(uuid id, const char *path, int page, time_t created, const char *description, const char *notes)
    : _name(path)
{
    this->_id = id;
    this->_path = path;
    this->_page = page;
    this->_created = created;
    this->_description = description == NULL ? "" : description;
    this->_notes = notes == NULL ? "" : notes;
}

Bookmark::Bookmark(const char *path, int page, const char *description, const char *notes)
    : _name(path)
{
    this->_id = XUtilities::NewUuid();
    this->_path = path;
    this->_page = page;
    this->_created = time(0);
    this->_description = description == NULL ? "" : description;
    this->_notes = notes == NULL ? "" : notes;
}

Bookmark::Bookmark(const Bookmark &other)
    : _name(other._name)
{
    this->_id = other._id;
    this->_path = other._path;
    this->_page = other._page;
    this->_created = other._created;
    this->_description = other._description;
    this->_notes = other._notes;
}

boost::uuids::uuid Bookmark::Identifier() const
{
    return this->_id;
}

ComicFileName Bookmark::Name() const
{
    return this->_name;
}

const char *Bookmark::Path() const
{
    return this->_path.c_str();
}

int Bookmark::Page() const
{
    return this->_page;
}

time_t Bookmark::Created() const
{
    return this->_created;
}

const char *Bookmark::Description() const
{
    return this->_description.c_str();
}

void Bookmark::SetDescription(const char *description)
{
    this->_description = description;
}

const char *Bookmark::Notes() const
{
    return this->_notes.c_str();
}

void Bookmark::SetNotes(const char *notes)
{
    this->_notes = notes;
}
