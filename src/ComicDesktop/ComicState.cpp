/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicState.h"
#include "ComicExceptions.hpp"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include "ComicUtils.h"
#include <QFileInfo>
#include <utility>

using namespace ComicSeer;
using namespace std;

ComicState* ComicState::_instance = NULL;

const char* IniSectionLibrary = "XLibraries";
const char* IniSectionAll = "XComicState";
const char* IniSectionConfig = "XConfiguration";

const char* IniKeyLibraryName = "Name";
const char* IniKeyLibraryPath = "Path";
const char* IniDefaultImageState = "DefaultState";
const char* IniDefaultImageView = "DefaultView";
const char* IniScaledZoom = "ScaledZoom";
const char* IniShowFullPath = "ShowFullPath";
const char* IniTwoPageMoveNumber = "TwoPageMove";
const char* IniBackcolor = "Backcolor";
const char* IniTwoPageAutodetectWide = "TwoPageDetectWide";
const char* IniThumbnailSize = "ThumbnailSize";
const char* IniReopenLastSession = "ReopenSession";
const char* IniShowPageNumber = "ShowPageNumber";
const char* IniPageNumberLocation = "PageNumberLocation";
const char* IniPageNumberColor = "PageNumberColor";
const char *IniComicTagPath = "ComicTaggerPath";
const char *IniAllowMagnification = "AllowMagnification";

ComicState::ComicState()
{
    this->Reinitialize();
}

std::map<std::string, std::string> ComicState::LibraryDirectories() const
{
    map<string, string> directories;
    for (auto it = this->_libraries.begin(); it != this->_libraries.end(); it++)
    {
        ComicLibrary *library = it->second.get();
        directories[it->first] = library->Path();
    }

    return directories;
}

ComicState* ComicState::State()
{
	if (_instance == NULL)
	{
		_instance = new ComicState();
	}

	return _instance;
}

void ComicState::AddLibraryDirectory(const char* name, const char* path)
{
    string n(name);
    string p(path);
	
    for (auto it = this->_libraries.begin(); it != this->_libraries.end(); it++)
	{
        if (boost::algorithm::iequals(name, it->first) || boost::algorithm::iequals(path, it->second->Path()))
		{
			throw ComicLibraryAlreadyExistsException("The library already exists.");
		}
	}

    this->_libraries[name] = boost::make_shared<ComicLibrary>(path);
}

void ComicState::RemoveLibraryDirectory(const char* name)
{
	this->_libraries.erase(string(name));
}

void ComicState::StoreState(XIni& ini) const
{
	// get the number of libraries
	std::stringstream libnumstr;
	libnumstr << this->_libraries.size();

	// clear the current libraries, if any
    ini.RemoveAllKeys(IniSectionLibrary);

	// set the total number of libraries
    ini.SetKeyValueForSection(IniSectionAll, IniSectionLibrary, libnumstr.str().c_str());

	// write each library
	int count = 0;
    for (auto it = this->_libraries.begin(); it != this->_libraries.end(); it++)
	{
		stringstream ssname;
        ssname << IniKeyLibraryName;
		ssname << count;

        ini.SetKeyValueForSection(IniSectionLibrary, ssname.str().c_str(), it->first.c_str());

		stringstream sspath;
        sspath << IniKeyLibraryPath;
		sspath << count;

        ini.SetKeyValueForSection(IniSectionLibrary, sspath.str().c_str(), it->second->Path());

		count++;
	}

    // write the default values
    ini.SetKeyValueForSection(IniSectionAll, IniDefaultImageState, static_cast<long>(this->_defaultState));
    ini.SetKeyValueForSection(IniSectionAll, IniDefaultImageView, static_cast<long>(this->_defaultView));
    ini.SetKeyValueForSection(IniSectionAll, IniTwoPageMoveNumber, this->_twoPageMoveNumber);
    ini.SetKeyValueForSection(IniSectionAll, IniBackcolor, this->_colorString.c_str());
    ini.SetKeyValueForSection(IniSectionAll, IniTwoPageAutodetectWide, this->_twoPageAutodetectWide);
    ini.SetKeyValueForSection(IniSectionAll, IniThumbnailSize, this->_thumbnailSize);
    ini.SetKeyValueForSection(IniSectionAll, IniReopenLastSession, this->_reopenLastSession);
    ini.SetKeyValueForSection(IniSectionAll, IniShowPageNumber, this->_showPageNumbers);
    ini.SetKeyValueForSection(IniSectionAll, IniPageNumberLocation, this->_pageNumberLocation);
    ini.SetKeyValueForSection(IniSectionAll, IniPageNumberColor, this->_pageNumberColorString.c_str());
    ini.SetKeyValueForSection(IniSectionAll, IniScaledZoom, this->_scaledZoom);
    ini.SetKeyValueForSection(IniSectionAll, IniShowFullPath, this->_showFilePath);
    ini.SetKeyValueForSection(IniSectionAll, IniComicTagPath, this->_comicTaggerPath.c_str());
    ini.SetKeyValueForSection(IniSectionAll, IniAllowMagnification, this->_allowMagnification);
}

void ComicState::LoadState(XIni& ini)
{
	// get the number of libraries
    string numLibrariesStr = ini.GetValueForKey(IniSectionAll, IniSectionLibrary);
	if (numLibrariesStr.empty())
	{
		return;
	}

	// read the libraries
	int numLibraries = atoi(numLibrariesStr.c_str());
	for (int i = 0; i < numLibraries; i++)
	{
		stringstream ssname;
        ssname << IniKeyLibraryName;
		ssname << i;

        string name = ini.GetValueForKey(IniSectionLibrary, ssname.str().c_str());

		stringstream sspath;
        sspath << IniKeyLibraryPath;
		sspath << i;

        string path = ini.GetValueForKey(IniSectionLibrary, sspath.str().c_str());

		if (name.empty() || path.empty())
		{
			continue;
		}

		this->AddLibraryDirectory(name.c_str(), path.c_str());
	}

    // read default image state
    long iValue = static_cast<int>(this->_defaultState);
    if (ini.GetValueForKeyCast(IniSectionAll, IniDefaultImageState, iValue) && iValue >= 0 && iValue <= 3)
    {
        this->_defaultState = static_cast<ImageState>(iValue);
    }

    // read default image view
    iValue = static_cast<int>(this->_defaultView);
    if (ini.GetValueForKeyCast(IniSectionAll, IniDefaultImageView, iValue) && iValue >= 0 && iValue <= 2)
    {
        this->_defaultView = static_cast<ImageView>(iValue);
    }

    // read two-page move number
    iValue = this->_twoPageMoveNumber;
    if (ini.GetValueForKeyCast(IniSectionAll, IniTwoPageMoveNumber, iValue))
    {
        this->SetTwoPageMoveNumber(iValue);
    }

    // read two-page wide page autodetection
    iValue = this->_twoPageAutodetectWide;
    if (ini.GetValueForKeyCast(IniSectionAll, IniTwoPageAutodetectWide, iValue))
    {
        this->_twoPageAutodetectWide = iValue ? true : false;
    }

    // read the background color string
    string sValue = ini.GetValueForKey(IniSectionAll, IniBackcolor);
    if (!sValue.empty())
    {
        this->SetBackgroundColor(sValue.c_str());
    }

    // read the thumbnail size
    iValue = this->_thumbnailSize;
    if (ini.GetValueForKeyCast(IniSectionAll, IniThumbnailSize, iValue))
    {
        this->SetThumbnailSize(iValue);
    }

    // read reopen of last session
    iValue = this->_reopenLastSession;
    if (ini.GetValueForKeyCast(IniSectionAll, IniReopenLastSession, iValue))
    {
        this->_reopenLastSession = iValue ? true : false;
    }

    // read show full file path
    iValue = this->_showFilePath;
    if (ini.GetValueForKeyCast(IniSectionAll, IniShowFullPath, iValue))
    {
        this->_showFilePath = iValue ? true : false;
    }

    // read show to page numbers
    iValue = this->_showPageNumbers;
    if (ini.GetValueForKeyCast(IniSectionAll, IniShowPageNumber, iValue))
    {
        this->_showPageNumbers = iValue ? true : false;
    }

    // read page number location
    iValue = this->_pageNumberLocation;
    if (ini.GetValueForKeyCast(IniSectionAll, IniPageNumberLocation, iValue))
    {
        this->_pageNumberLocation = static_cast<ComicSeer::PageNumberLocation>(iValue);
    }

    // read page number color
    sValue = ini.GetValueForKey(IniSectionAll, IniPageNumberColor);
    if (!sValue.empty())
    {
        this->_pageNumberColorString = sValue;
    }

    // read scaled zoom
    double dValue = this->_scaledZoom;
    if (ini.GetValueForKeyCast(IniSectionAll, IniScaledZoom, dValue) && dValue >= 1.0 && dValue <= 4.0)
    {
        this->_scaledZoom = dValue;
    }

    // read comic tagger path
    sValue = ini.GetValueForKey(IniSectionAll, IniComicTagPath);
    if (!sValue.empty())
    {
        this->_comicTaggerPath = sValue;
    }

    // read allow magnification
    iValue = this->_allowMagnification;
    if (ini.GetValueForKeyCast(IniSectionAll, IniAllowMagnification, iValue))
    {
        this->_allowMagnification = iValue ? true : false;
    }
}

ImageState ComicState::DefaultImageState() const
{
    return this->_defaultState;
}

void ComicState::SetDefaultImageState(ImageState state)
{
    this->_defaultState = state;
}

ImageView ComicState::DefaultImageView() const
{
    return this->_defaultView;
}

void ComicState::SetDefaultImageView(ImageView view)
{
    this->_defaultView = view;
}

double ComicState::ScaledZoomFactor() const
{
    return this->_scaledZoom;
}

void ComicState::SetScaledZoom(double value)
{
    if (value >= 1.0 && value <= 4.0)
    {
        this->_scaledZoom = value;
    }
}

bool ComicState::ShowFullFilePath() const
{
    return this->_showFilePath;
}

void ComicState::SetShowFullFilePath(bool show)
{
    if (this->_showFilePath != show)
    {
        this->_showFilePath = show;
        this->ShowFullPathUpdated(show);
    }
}

int ComicState::TwoPageMoveNumber() const
{
    return this->_twoPageMoveNumber;
}

void ComicState::SetTwoPageMoveNumber(int number)
{
    if (number < 1 || number > 2)
    {
        return;
    }

    this->_twoPageMoveNumber = number;
}

void ComicState::SetBackgroundColor(const char* color)
{
    if (strlen(color) == 7 && color[0] == '#' && strcmp(this->_colorString.c_str(), color) != 0)
    {
        this->_colorString = color;
        this->BackgroundColorUpdated(color);
    }
}

std::string ComicState::BackgroundColor() const
{
    return this->_colorString;
}

void ComicState::SetThumbnailSize(int size)
{
    if (size >= 75 && size <= 600)
    {
        this->_thumbnailSize = size;
        this->ThumbnailSizeUpdated(size);
    }
}

bool ComicState::ShowPageNumbers() const
{
    return this->_showPageNumbers;
}

ComicSeer::PageNumberLocation ComicState::PageNumberLocation() const
{
    return this->_pageNumberLocation;
}

string ComicState::PageNumberColor() const
{
    return this->_pageNumberColorString;
}

void ComicState::SetPageNumberSettings(bool show, ComicSeer::PageNumberLocation location, const char *color)
{
    bool changed = false;

    if (show != this->_showPageNumbers)
    {
        this->_showPageNumbers = show;
        changed = true;
    }

    if (location != this->_pageNumberLocation)
    {
        this->_pageNumberLocation = location;
        changed = true;
    }

    if (strlen(color) == 7 && color[0] == '#' && strcmp(this->_pageNumberColorString.c_str(), color) != 0)
    {
        this->_pageNumberColorString = color;
        changed = true;
    }

    if (changed)
    {
        this->PageNumberingUpdated();
    }
}

double ComicState::MagnifierMagnification() const
{
    return 2.0;
}

string ComicState::ComicTaggerPath() const
{
    return this->_comicTaggerPath;
}

void ComicState::SetComicTaggerPath(const std::string &path)
{
    this->_comicTaggerPath = path;
}

int ComicState::ThumbnailSize() const
{
    return this->_thumbnailSize;
}

bool ComicState::TwoPageAutodetectWidePage() const
{
    return this->_twoPageAutodetectWide;
}

void ComicState::SetTwoPageAutodetectWidePage(bool autodetect)
{
    this->_twoPageAutodetectWide = autodetect;
}

bool ComicState::ReopenLastSession() const
{
    return this->_reopenLastSession;
}

void ComicState::SetReopenLastSession(bool reopen)
{
    this->_reopenLastSession = reopen;
}

void ComicState::SetAllowMagnification(bool allow)
{
    this->_allowMagnification = allow;
}

void ComicState::RefreshLibrary(const char *name)
{
    auto it = this->_libraries.find(name);

    assert(it != this->_libraries.end());
    it->second->RefreshLibrary();
}

const ComicLibrary *ComicState::Library(const char *name) const
{
    auto it = this->_libraries.find(name);
    assert(it != this->_libraries.end());
    return it->second.get();
}

int ComicState::NumberOfComicsInLibraries()
{
    int total = 0;
    for_each(begin(this->_libraries), end(this->_libraries), [&](pair<string, boost::shared_ptr<ComicLibrary> > kvp)
    {
        total += kvp.second->GetComicNumber();
    });
    return total;
}

bool ComicState::AllowMagnification() const
{
    return this->_allowMagnification;
}

void ComicState::Reinitialize(bool clearPaths)
{
    // clear lists
    if (clearPaths)
    {
        this->_libraries.clear();
        this->SetComicTaggerPath("");
    }

    // set the default values
    this->SetDefaultImageView(OnePage);
    this->SetDefaultImageState(Manual);
    this->SetScaledZoom(2.0);
    this->SetShowFullFilePath(false);
    this->SetTwoPageMoveNumber(2);
    this->SetBackgroundColor("#000000");
    this->SetTwoPageAutodetectWidePage(true);
    this->SetThumbnailSize(150);
    this->SetReopenLastSession(true);
    this->SetPageNumberSettings(false, ComicSeer::Bottom, "#006600");
    this->SetAllowMagnification(true);
}

Bookmarkings &ComicState::Bookmarks()
{
    if (this->_bookmarks.get() == NULL)
    {
        // create the bookmark path
        QString path = ComicUtils::FileConfigurationPath();
        path.append("Bookmarks.xml");

        // create the bookmarkings
        this->_bookmarks.reset(new Bookmarkings());

        // load what's possible
        try
        {
            QFileInfo file(path);
            if (file.exists())
            {
                this->_bookmarks->Load(path.toUtf8());
            }
        }
        catch (...) { }
    }

    return (*this->_bookmarks);
}

void ComicState::ReloadBookmarks()
{
    // clear the existing bookmarks
    this->_bookmarks.reset();

    // reload the bookmarks
    this->Bookmarks();
}

void ComicState::StoreBookmarks()
{
    // if the bookmarkings don't exist, its a bug
    assert(this->_bookmarks.get() != NULL);

    // create the bookmark path
    QString path = ComicUtils::FileConfigurationPath();
    path.append("Bookmarks.xml");

    this->_bookmarks->Store(path.toUtf8());
}
