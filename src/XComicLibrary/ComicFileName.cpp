/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicFileName.hpp"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace ComicSeer;
using namespace boost;
using namespace std;

const std::string ComicFileName::_regex = "(?<base>(.+?))\\s*-?\\s*"
        "(\\s(?:v|V|VO|vo|Vo|vol|Vol|VOL|volume|Volume|VOLUME|volume)#?\\s?#?(?<volume>\\d+))?\\s*-?\\s*"
        "(\\s(?:i|I|Iss|ISS|iss|issue|Issue|ISSUE|#)?\\s?(?<issue>\\d+\\.?\\d*))?\\s*-?\\s*"
        "(\\s(?:p|P|pt|Pt|Part|part|PART)#?\\s?#?(?<part>\\d+))?\\s*-?\\s*"
        "(\\([^()]+\\))?\\s*";

bool FindLastMatch(boost::regex& regex, string& searchText, string& number)
{
    static const int regex_removal = 1;
    static const int regex_digits = 2;

    boost::smatch matched;
    if(boost::regex_match(searchText, matched, regex))
    {
        number = matched.str(regex_digits);
        searchText.erase(matched.position(regex_removal), matched.length(regex_removal));

        return true;
    }

    return false;
}

ComicSeer::ComicFileName::ComicFileName(const char *filename)
{
    this->_filename = filename;

    // clean file name if needed
    size_t lastSlashIndex = this->_filename.find_last_of('/');
    if (lastSlashIndex != this->_filename.npos)
    {
        this->_filename = this->_filename.substr(lastSlashIndex + 1);
    }
    lastSlashIndex = this->_filename.find_last_of('\\');
    if (lastSlashIndex != this->_filename.npos)
    {
        this->_filename = this->_filename.substr(lastSlashIndex + 1);
    }
    size_t lastPeriodIndex = this->_filename.find_last_of('.');
    if (lastPeriodIndex != this->_filename.npos)
    {
        this->_filename = this->_filename.substr(0, lastPeriodIndex);
    }

    boost::algorithm::trim(this->_filename);
    this->_prettyname = this->_filename;

    static boost::regex name_match_regex(_regex);

    string issue, volume, part;
    boost::smatch matched;

    if (boost::regex_match(this->_prettyname, matched, name_match_regex))
    {
        this->_basename = boost::algorithm::trim_copy(string(matched["base"]));
        volume = boost::algorithm::trim_copy(string(matched["volume"]));
        issue = boost::algorithm::trim_copy(string(matched["issue"]));
        part = boost::algorithm::trim_copy(string(matched["part"]));

        // reset the pretty name
        this->_prettyname = this->_basename;

        // write the volume
        if (!volume.empty())
        {
            stringstream ss;
            this->_volNumber.reset(new int);
            *this->_volNumber = ::atoi(volume.c_str());
            ss << " V" << *this->_volNumber;
            this->_prettyname.append(ss.str());
        }

        // write the part
        if (!part.empty())
        {
            stringstream ss;
            this->_partNumber.reset(new int);
            *this->_partNumber = ::atoi(part.c_str());
            ss << " P" << *this->_partNumber;
            this->_prettyname.append(ss.str());
        }

        // write the issue
        if (!issue.empty())
        {
            stringstream ss;
            this->_issueNumber.reset(new double);
            *this->_issueNumber = ::atof(issue.c_str());
            ss << " #" << *this->_issueNumber;
            this->_prettyname.append(ss.str());
        }
    }
    else
    {
        this->_basename = this->_prettyname;
    }
}

ComicFileName::ComicFileName(const ComicFileName &other)
{
    this->operator =(other);
}

ComicFileName &ComicFileName::operator=(const ComicFileName &other)
{
    _filename = other._filename;
    _basename = other._basename;
    _prettyname = other._prettyname;

    if (other._volNumber.get() != NULL)
    {
        _volNumber.reset(new int);
        *_volNumber = *other._volNumber;
    }

    if (other._partNumber.get() != NULL)
    {
        _partNumber.reset(new int);
        *_partNumber = *other._partNumber;
    }

    if (other._issueNumber.get() != NULL)
    {
        _issueNumber.reset(new double);
        *_issueNumber = *other._issueNumber;
    }

    return *this;
}

int ptr_compare(double *i1, double *i2)
{
    if (i1 == nullptr && i2 == nullptr)
    {
        return 0;
    }
    else if (i1 == nullptr && i2 != nullptr)
    {
        return -1;
    }
    else if (i1 != nullptr && i2 == nullptr)
    {
        return 1;
    }
    else
    {
        if (*i1 < *i2)
        {
            return -1;
        }
        else if (*i1 > *i2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

int ptr_compare(int *i1, int *i2)
{
    if (i1 == nullptr && i2 == nullptr)
    {
        return 0;
    }
    else if (i1 == nullptr && i2 != nullptr)
    {
        return -1;
    }
    else if (i1 != nullptr && i2 == nullptr)
    {
        return 1;
    }
    else
    {
        if (*i1 < *i2)
        {
            return -1;
        }
        else if (*i1 > *i2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

bool ComicFileName::operator <(const ComicFileName &other) const
{
    {
        int val = XUtilities::Utf8CaseInsensitiveCompare(_basename,  other._basename);
        if (val != 0)
        {
            return val < 0;
        }

        val = ptr_compare(this->_volNumber.get(), other._volNumber.get());
        if (val != 0)
        {
            return val < 0;
        }

        val = ptr_compare(this->_issueNumber.get(), other._issueNumber.get());
        if (val != 0)
        {
            return val < 0;
        }

        val = ptr_compare(this->_partNumber.get(), other._partNumber.get());
        if (val != 0)
        {
            return val < 0;
        }

        return false;
    }
}

const char *ComicFileName::NameRegex()
{
    return _regex.c_str();
}
