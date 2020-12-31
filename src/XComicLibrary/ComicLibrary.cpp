/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicLibrary.hpp"
#include <XException.hpp>
#include <ComicUtilities.hpp>
#include <XUtilities.hpp>
#include <boost/algorithm/string.hpp>

using namespace ComicSeer;
using namespace std;

ComicLibrary::ComicLibrary(const char *path)
{
    this->_path = path;
}

void ComicLibrary::ParseFolder(ComicFolder& folder)
{
    // get the items in the directory
    vector<XUtilities::XFileDesc> entries;
    XUtilities::GetFilesInDirectory(folder.path.c_str(), entries);

    // iterate through the items
    for(auto it = entries.begin(); it != entries.end(); it++)
    {
        XUtilities::XFileDesc& desc = *it;

        if (desc.isFile)
        {
            string ext = XUtilities::GetFileExtension(desc.name.c_str());
            if (ComicFile::ClassifyFileExtension(ext.c_str()) != ComicFile::Unknown)
            {
                boost::shared_ptr<ComicLibraryItem> item(new ComicLibraryItem(desc));

                folder.Comics.push_back(item);
                this->_flatComicList.push_back(item);
                this->_recentComicList.push_back(item);
            }
        }
        else
        {
            ComicFolder cfolder;
            cfolder.name = desc.name;
            cfolder.path = desc.fullname;

            // parse the sub-folder
            this->ParseFolder(cfolder);

            folder.Folders.push_back(cfolder);
        }
    }

    std::sort(begin(folder.Folders), end(folder.Folders), [](const ComicFolder& f1, const ComicFolder& f2)
    {
        return XUtilities::Utf8CaseInsensitiveCompare(f1.name, f2.name) < 0;
    });

    std::sort(folder.Comics.begin(), folder.Comics.end(), [](const boost::shared_ptr<ComicLibraryItem>& f1, const boost::shared_ptr<ComicLibraryItem>& f2)
    {
        return (*f1) < (*f2);
    });

    std::sort(this->_flatComicList.begin(), this->_flatComicList.end(), [](const boost::shared_ptr<ComicLibraryItem>& f1, const boost::shared_ptr<ComicLibraryItem>& f2)
    {
        return (*f1) < (*f2);
    });

    this->_itemNumber += folder.Comics.size();
}

void ComicLibrary::RefreshLibrary()
{
    this->_itemNumber = 0;
    this->_flatComicList.clear();
    this->_folder.Folders.clear();
    this->_folder.Comics.clear();
    this->_folder.name = "ComicRoot";
    this->_folder.path = this->_path;

    this->ParseFolder(this->_folder);

    // sort comics by name/issue/etc
    std::sort(this->_flatComicList.begin(), this->_flatComicList.end(), [](const boost::shared_ptr<ComicLibraryItem>& f1, const boost::shared_ptr<ComicLibraryItem>& f2)
    {
        return (*f1) < (*f2);
    });

    // sort comics by modification time
    std::sort(this->_recentComicList.begin(), this->_recentComicList.end(), [](const boost::shared_ptr<ComicLibraryItem>&f1, const boost::shared_ptr<ComicLibraryItem>& f2)
    {
        return f1->ModificationDate() > f2->ModificationDate();
    });
}

long ComicLibrary::GetComicNumber() const
{
    return this->_itemNumber;
}

const char *ComicLibrary::Path() const
{
    return this->_path.c_str();
}

const ComicLibrary::ComicFolder &ComicLibrary::GetLibraryComics() const
{
    return this->_folder;
}

const std::vector<boost::shared_ptr<ComicLibraryItem> >& ComicLibrary::GetCompleteComicList() const
{
    return this->_flatComicList;
}

const vector<ComicLibrary::ComicFolder> ComicLibrary::GetGroupedComicList() const
{
    vector<boost::shared_ptr<ComicLibraryItem> > comics = this->GetCompleteComicList();

    vector<ComicFolder> groupedList;

    string prevNameLower;
    for (vector<boost::shared_ptr<ComicLibraryItem> >::const_iterator it = comics.begin(); it != comics.end(); it++)
    {
        string lower((*it)->BaseName());
        boost::algorithm::to_lower(lower);

        // compare the current to the previous
        if (strcmp(prevNameLower.c_str(), lower.c_str()) != 0)
        {
            ComicFolder folder;
            folder.name = (*it)->BaseName();
            groupedList.push_back(folder);

            prevNameLower = lower;
        }

        groupedList[groupedList.size() - 1].Comics.push_back(*it);
    }

    return groupedList;
}

const std::vector<boost::shared_ptr<ComicLibraryItem> >& ComicLibrary::GetModifiedTimeComicList() const
{
    return this->_recentComicList;
}
