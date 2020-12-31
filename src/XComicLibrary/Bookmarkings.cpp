/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Bookmarkings.hpp"
#include "pugixml.hpp"
#include <sstream>
#include "XException.hpp"
#include <algorithm>
#include <utility>

using namespace ComicSeer;
using namespace std;
using namespace pugi;

Bookmarkings::Bookmarkings()
{
}

void Bookmarkings::AddBookmark(const Bookmark &mark)
{
    this->_marks[mark.Identifier()] = boost::make_shared<Bookmark>(mark);
}

void Bookmarkings::RemoveBookmark(boost::uuids::uuid id)
{
    this->_marks.erase(id);
}


void Bookmarkings::Load(const char *path)
{
    // remove any existing marks
    this->_marks.clear();

    xml_document document;

    xml_parse_result result = document.load_file(path);
    if (!result)
    {
        stringstream ss;
        ss << result.description() << ", offset" << result.offset;
        throw XmlFileReadException(ss.str().c_str());
    }

    xml_node root = document.root();
    xml_node bmNode = root.child("Bookmarks");
    if (!bmNode)
    {
        throw XmlFileReadException("The document does not contain the expected root node.");
    }

    for (xml_node_iterator it = bmNode.children().begin(); it != bmNode.children().end(); it++)
    {
        if (strcmp(it->name(), "Bookmark") != 0)
        {
            continue;
        }

        boost::uuids::uuid id = XUtilities::EmptyUuid();
        string path, description, notes;
        int page = -1;
        time_t created = 0;
        for (xml_node_iterator prop = it->children().begin(); prop != it->children().end(); prop++)
        {
            if (strcmp(prop->name(), "Identifier") == 0)
                id = XUtilities::StringToBoostUuid(prop->text().as_string());
            else if (strcmp(prop->name(), "Path") == 0)
                path = prop->text().as_string();
            else if (strcmp(prop->name(), "Page") == 0)
                page = prop->text().as_int(-1);
            else if (strcmp(prop->name(), "Description") == 0)
                description = prop->text().as_string();
            else if (strcmp(prop->name(), "Notes") == 0)
                notes = prop->text().as_string();
            else if (strcmp(prop->name(), "Created") == 0)
                created = prop->text().as_llong();
        }

        if (id == XUtilities::EmptyUuid() || path.empty() || page < 1 || created == 0)
        {
            continue;
        }

        Bookmark bm(id, path.c_str(), page, created, description.c_str(), notes.c_str());
        this->AddBookmark(bm);
    }
}

void Bookmarkings::Store(const char *path)
{
    xml_document document;

    xml_node decNode = document.append_child(node_declaration);
    decNode.append_attribute("version").set_value("1.0");
    decNode.append_attribute("encoding").set_value("UTF-8");

    xml_node rootNode = document.append_child(node_element);
    rootNode.set_name("Bookmarks");
    rootNode.append_attribute("xmlns").set_value("http://www.xylasoft.com/comicseer");
    rootNode.append_attribute("xmlns:xsi").set_value("http://www.w3.org/2001/XMLSchema-instance");
    rootNode.append_attribute("xsi:schemaLocation").set_value("http://www.xylasoft.com/comicseer ComicSeer.xsd");

    const vector<const Bookmark*> marks = this->Bookmarks();
    for_each(marks.begin(), marks.end(), [&](const Bookmark *mark)
    {
        xml_node markNode = rootNode.append_child(node_element);
        markNode.set_name("Bookmark");

        markNode.append_child("Identifier").text().set(XUtilities::BoostUuidToString(mark->Identifier()).c_str());
        markNode.append_child("Name").text().set(mark->Name().PrettyName());
        markNode.append_child("Path").text().set(mark->Path());
        markNode.append_child("Page").text().set(mark->Page());
        markNode.append_child("Created").text().set(static_cast<long long>(mark->Created()));
        markNode.append_child("Description").text().set(mark->Description());
        markNode.append_child("Notes").text().set(mark->Notes());
    });

    if (!document.save_file(path))
    {
        throw XmlFileWriteException("Failed to write bookmark file.");
    }
}

const Bookmark *Bookmarkings::BookmarkById(boost::uuids::uuid id) const
{
    auto it = this->_marks.find(id);
    if(it == this->_marks.end())
    {
        return NULL;
    }
    else
    {
        return it->second.get();
    }
}

void Bookmarkings::UpdateBookmarkDescriptionNotes(boost::uuids::uuid id, const char *desc, const char *notes)
{
    auto it = this->_marks.find(id);
    if (it == this->_marks.end())
    {
        return;
    }

    it->second->SetDescription(desc);
    it->second->SetNotes(notes);
}

void Bookmarkings::UpdateBookmarkPath(boost::uuids::uuid id, const char *path)
{
    auto it = this->_marks.find(id);
    if (it == this->_marks.end())
    {
        return;
    }

    Bookmark *bm = it->second.get();
    *bm = Bookmark(id, path, bm->Page(), bm->Created(), bm->Description(), bm->Notes());
}

const std::vector<const Bookmark *> Bookmarkings::Bookmarks() const
{
    vector<const Bookmark*> marks;

    for (map<boost::uuids::uuid, boost::shared_ptr<Bookmark>>::const_iterator it = this->_marks.begin(); it != this->_marks.end(); it++)
    {
        marks.push_back(it->second.get());
    }

    return marks;
}
