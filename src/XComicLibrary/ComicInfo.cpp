/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicInfo.hpp"
#include "pugixml.hpp"
#include "XException.hpp"
#include <sstream>
#include <cstring>

using namespace ComicSeer;
using namespace pugi;
using namespace std;

#define TITLE "Title"
#define SERIES "Series"
#define NUMBER "Number"
#define COUNT "Count"
#define VOLUME "Volume"
#define ALTSERIES "AlternateSeries"
#define ALTNUMBER "AlternateNumber"
#define ALTCOUNT "AlternateCount"
#define SUMMARY "Summary"
#define NOTES "Notes"
#define YEAR "Year"
#define MONTH "Month"
#define WRITER "Writer"
#define PENCILLER "Penciller"
#define INKER "Inker"
#define COLORIST "Colorist"
#define LETTERER "Letterer"
#define COVERARTIST "CoverArtist"
#define EDITOR "Editor"
#define PUBLISHER "Publisher"
#define IMPRINT "Imprint"
#define GENRE "Genre"
#define WEB "Web"
#define PAGECOUNT "PageCount"
#define LANGUAGEISO "LanguageISO"
#define FORMAT "Format"
#define BW "BlackAndWhite"
#define MANGA "Manga"
#define CHARACTERS "Characters"
#define TEAMS "Teams"
#define LOCATIONS "Locations"

ComicInfo::ComicInfo(const XByteArray &data)
{
    xml_document document;

    xml_parse_result result = document.load(data.ConstData());
    if (!result)
    {
        stringstream ss;
        ss << result.description() << ", offset" << result.offset;
        throw XmlFileReadException(ss.str().c_str());
    }

    xml_node root = document.root();
    xml_node ciNode = root.child("ComicInfo");
    if (!ciNode)
    {
        throw XmlFileReadException("The document does not contain the expected root node.");
    }

    // set all ints to a default number
    _count = -1;
    _volume = -1;
    _alternateCount = -1;
    _year = -1;
    _month = -1;
    _pageCount = -1;

    // set the defaults for enums
    _isManga = ComicInfo::UNDEFINED;
    _isBlackAndWhite = ComicInfo::UNDEFINED;

    for (xml_node_iterator it = ciNode.children().begin(); it != ciNode.end(); it++)
    {
        if (strcmp(TITLE, it->name()) == 0)
        {
            this->_title = it->child_value();
        }
        else if (strcmp(SERIES, it->name()) == 0)
        {
            this->_series = it->child_value();
        }
        else if (strcmp(NUMBER, it->name()) == 0)
        {
            this->_number = it->child_value();
        }
        else if (strcmp(COUNT, it->name()) == 0)
        {
            this->_count = atoi(it->child_value());
        }
        else if (strcmp(VOLUME, it->name()) == 0)
        {
            this->_volume = atoi(it->child_value());
        }
        else if (strcmp(ALTSERIES, it->name()) == 0)
        {
            this->_alternateSeries = it->child_value();
        }
        else if (strcmp(ALTNUMBER, it->name()) == 0)
        {
            this->_alternateNumber = it->child_value();
        }
        else if (strcmp(ALTCOUNT, it->name()) == 0)
        {
            this->_alternateCount = atoi(it->child_value());
        }
        else if (strcmp(SUMMARY, it->name()) == 0)
        {
            this->_summary = it->child_value();
        }
        else if (strcmp(NOTES, it->name()) == 0)
        {
            this->_notes = it->child_value();
        }
        else if (strcmp(YEAR, it->name()) == 0)
        {
            this->_year = atoi(it->child_value());
        }
        else if (strcmp(MONTH, it->name()) == 0)
        {
            this->_month = atoi(it->child_value());
        }
        else if (strcmp(WRITER, it->name()) == 0)
        {
            this->_writer = it->child_value();
        }
        else if (strcmp(PENCILLER, it->name()) == 0)
        {
            this->_penciller = it->child_value();
        }
        else if (strcmp(INKER, it->name()) == 0)
        {
            this->_inker = it->child_value();
        }
        else if (strcmp(COLORIST, it->name()) == 0)
        {
            this->_colorist = it->child_value();
        }
        else if (strcmp(LETTERER, it->name()) == 0)
        {
            this->_letterer = it->child_value();
        }
        else if (strcmp(COVERARTIST, it->name()) == 0)
        {
            this->_coverArtist = it->child_value();
        }
        else if (strcmp(EDITOR, it->name()) == 0)
        {
            this->_editor = it->child_value();
        }
        else if (strcmp(PUBLISHER, it->name()) == 0)
        {
            this->_publisher = it->child_value();
        }
        else if (strcmp(IMPRINT, it->name()) == 0)
        {
            this->_imprint = it->child_value();
        }
        else if (strcmp(GENRE, it->name()) == 0)
        {
            this->_genre = it->child_value();
        }
        else if (strcmp(WEB, it->name()) == 0)
        {
            this->_web = it->child_value();
        }
        else if (strcmp(PAGECOUNT, it->name()) == 0)
        {
            this->_pageCount = atoi(it->child_value());
        }
        else if (strcmp(LANGUAGEISO, it->name()) == 0)
        {
            this->_languageISO = it->child_value();
        }
        else if (strcmp(FORMAT, it->name()) == 0)
        {
            this->_format = it->child_value();
        }
        else if (strcmp(CHARACTERS, it->name()) == 0)
        {
            this->_characters = it->child_value();
        }
        else if (strcmp(TEAMS, it->name()) == 0)
        {
            this->_teams = it->child_value();
        }
        else if (strcmp(LOCATIONS, it->name()) == 0)
        {
            this->_locations = it->child_value();
        }
        else if (strcmp(BW, it->name()) == 0)
        {
            if (strcmp("Yes", it->child_value()) == 0)
            {
                this->_isBlackAndWhite = ComicInfo::YES;
            }
            else if (strcmp("No", it->child_value()) == 0)
            {
                this->_isBlackAndWhite = ComicInfo::NO;
            }
        }
        else if (strcmp(MANGA, it->name()) == 0)
        {
            if (strcmp("Yes", it->child_value()) == 0)
            {
                this->_isManga = ComicInfo::YES;
            }
            else if (strcmp("No", it->child_value()) == 0)
            {
                this->_isManga = ComicInfo::NO;
            }
        }
    }
}

string ComicInfo::Title() const
{
    return this->_title;
}

string ComicInfo::Series() const
{
    return this->_series;
}

string ComicInfo::Number() const
{
    return this->_number;
}

int ComicInfo::Count() const
{
    return this->_count;
}

int ComicInfo::Volume() const
{
    return this->_volume;
}

string ComicInfo::AlternateSeries() const
{
    return this->_alternateSeries;
}

string ComicInfo::AlternateNumber() const
{
    return this->_alternateNumber;
}

int ComicInfo::AlternateCount() const
{
    return this->_alternateCount;
}

string ComicInfo::Summary() const
{
    return this->_summary;
}

string ComicInfo::Notes() const
{
    return this->_notes;
}

int ComicInfo::Year() const
{
    return this->_year;
}

int ComicInfo::Month() const
{
    return this->_month;
}

std::string ComicInfo::Writer() const
{
    return this->_writer;
}

std::string ComicInfo::Penciller() const
{
    return this->_penciller;
}

std::string ComicInfo::Inker() const
{
    return this->_inker;
}

std::string ComicInfo::Colorist() const
{
    return this->_colorist;
}

std::string ComicInfo::Letterer() const
{
    return this->_letterer;
}

std::string ComicInfo::CoverArtist() const
{
    return this->_coverArtist;
}

std::string ComicInfo::Editor() const
{
    return this->_editor;
}

std::string ComicInfo::Publisher() const
{
    return this->_publisher;
}

std::string ComicInfo::Imprint() const
{
    return this->_imprint;
}

std::string ComicInfo::Genre() const
{
    return this->_genre;
}

std::string ComicInfo::Web() const
{
    return this->_web;
}

int ComicInfo::PageCount() const
{
    return this->_pageCount;
}

std::string ComicInfo::LanguageISO() const
{
    return this->_languageISO;
}

std::string ComicInfo::Format() const
{
    return this->_format;
}

std::string ComicInfo::Characters() const
{
    return this->_characters;
}

std::string ComicInfo::Teams() const
{
    return this->_teams;
}

std::string ComicInfo::Locations() const
{
    return this->_locations;
}

ComicInfo::YesNo ComicInfo::IsBlackAndWhite() const
{
    return this->_isBlackAndWhite;
}

ComicInfo::YesNo ComicInfo::IsManga() const
{
    return this->_isManga;
}
