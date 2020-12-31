/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicFile.hpp"
#include "ComicUtilities.hpp"
#include "CbzComicFile.hpp"
#include "CbrComicFile.hpp"
#include "PdfComicFile.hpp"
#include "ImageComicFile.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <XUtilities.hpp>

using namespace ComicSeer;
using namespace std;

ComicFile* ComicFile::CreateComicFile(const char* filepath)
{
	// verify the path is not empty
    if (filepath == NULL || strlen(filepath) == 0)
    {
        throw ArgumentException("The file path cannot be empty.");
    }

	// trim the path
    string sfile = filepath;
    boost::algorithm::trim(sfile);

	// get the file extension
    string ext = XUtilities::GetFileExtension(sfile.c_str());

	// instantiate the appropriate class or throw an exception
	switch (ClassifyFileExtension(ext.c_str()))
	{
	case Zip:
		return new CbzComicFile(sfile.c_str());
	case Rar:
        return new CbrComicFile(sfile.c_str());
    case Image:
        return new ImageComicFile(sfile.c_str());
#ifdef PDFSUPPORT
    case Pdf:
        return new PdfComicFile(sfile.c_str());
#endif
	default:
		throw FileFormatException("The file extension was not recognized as a valid format.");
    }
}

void ComicFile::DisposeComicFile(ComicFile* comic)
{
    delete comic;
}

ComicFile::ComicFileType ComicFile::ClassifyFileExtension(const char* extension)
{
	string ext = extension;
	boost::algorithm::to_lower(ext);

	if (ext.compare("cbz") == 0 || ext.compare("zip") == 0)
    {
        return ComicFile::Zip;
    }
    else if (ext.compare("cbr") == 0 || ext.compare("rar") == 0)
    {
		return ComicFile::Rar;
    }
#ifdef PDFSUPPORT
    else if (ext.compare("pdf") == 0)
    {
        return ComicFile::Pdf;
    }
#endif
	else
	{
        // see if it is a straight image
        list<string> exts = ComicUtilities::SupportedImageExtensions();
        for (auto it = exts.begin(); it != exts.end(); it++)
        {
            if (strcmp(it->c_str(), ext.c_str()) == 0)
            {
                return ComicFile::Image;
            }
        }

		return ComicFile::Unknown;
	}
}

ComicFile::ComicFile(const char* filepath)
{
    this->_filepath = filepath;
}

ComicFile::~ComicFile()
{
}

void ComicFile::Initialize()
{
	// clear any existing information
	_imageEntries.clear();
	_altEntries.clear();
	_comicInfoPresent = "";

	// get the file list
	list<string> entries = this->GetFileList();

	// get the list of supported image extensions
	list<string> exts = ComicUtilities::SupportedImageExtensions();

	// iterate through the file list
	for (list<string>::const_iterator it = entries.begin(); it != entries.end(); it++)
	{
		// get the file extension
        string ext = XUtilities::GetFileExtension(it->c_str());

		// if the file matches a supported image extension
		list<string>::const_iterator itfind = find(exts.begin(), exts.end(), ext);
 		if (itfind != exts.end())
		{
			_imageEntries.push_back(*it);
		}
		// if this is the comic info file
		else if (boost::algorithm::to_lower_copy(*it).find("comicinfo.xml") != it->npos)
		{
			this->_comicInfoPresent = *it;
		}
		// else this is an alternate file
		else
		{
			_altEntries.push_back(*it);
		}
	}

	// sort the images and alt files
    this->_imageEntries.sort([](const string &s1, const string &s2)
    {
        return XUtilities::Utf8CaseInsensitiveCompare(s1, s2) < 0;
    });
    this->_altEntries.sort([](const string &s1, const string &s2)
    {
        return XUtilities::Utf8CaseInsensitiveCompare(s1, s2) < 0;
    });
}

uint32_t ComicFile::NumberOfPages() const
{
	return static_cast<uint32_t>(_imageEntries.size());
}

void ComicFile::GetTitleImage(XByteArray& data) const
{
	if (_imageEntries.size() <= 0)
	{
		throw FileException("The file is not loaded or contains no images.");
	}

	this->ReadFile(_imageEntries.begin()->c_str(), data);
}

bool ComicFile::GetComicInformation(XByteArray& content)
{
    if (!this->HasComicInformationFile())
	{
		return false;
	}

    content.Clear();

    this->ReadFile(this->_comicInfoPresent.c_str(), content);

    if (content.Size() <= 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

ComicInfo ComicFile::GetComicInformation()
{
    XByteArray data;
    if (!this->GetComicInformation(data))
    {
        throw FileReadException("The comic information file could not be read.");
    }

    ComicInfo ci(data);

    return ci;
}

void ComicFile::GetFile(const char* name, XByteArray& data) const
{
	this->ReadFile(name, data);
}

string ComicFile::FilePath() const
{
    return this->_filepath;
}

bool ComicFile::HasComicInformationFile() const
{
    if (this->_comicInfoPresent.empty())
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::list<std::string> ComicFile::ImageList() const
{
	return this->_imageEntries;
}

std::list<std::string> ComicFile::NonImageList() const
{
	return this->_altEntries;
}
