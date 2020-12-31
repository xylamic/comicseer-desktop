/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <ComicUtilities.hpp>
#include <XUtilities.hpp>
#include <iostream>
#include <ComicLibrary.hpp>
#include <ComicLibraryItem.hpp>
#include <time.h>

using namespace ComicSeer;
using namespace std;

#ifdef _WIN32
    #define COMIC_LIBRARY "D:\\Dropbox\\Books\\CBZ"
#else
    #define COMIC_LIBRARY "/media/xylamic/data/Dropbox/Books/CBZ"
#endif

#define CURRENT_COMIC_NUMBER 399

void AssertFindItem(const ComicLibrary::ComicFolder& folder, const char* name)
{
    for(auto it = folder.Comics.begin(); it != folder.Comics.end(); it++)
    {
        if (strcmp((*it)->FileName(), name) == 0)
            return;
    }

    ASSERT_TRUE(false);
}

TEST(ComicLibrary, FileList)
{
    ComicLibrary c(".");
    c.RefreshLibrary();

    ComicLibrary::ComicFolder folder = c.GetLibraryComics();

    ASSERT_EQ(11, c.GetComicNumber());
    ASSERT_LE(size_t(1), folder.Folders.size());
    ASSERT_EQ(11, folder.Comics.size());

    ASSERT_STREQ("01", folder.Comics[0]->FileName());
    ASSERT_STREQ("08", folder.Comics[1]->FileName());
    ASSERT_STREQ("Wonder Woman Vol04 001 - こんにちは", folder.Comics[10]->FileName());

    vector<boost::shared_ptr<ComicLibraryItem> > lst = c.GetCompleteComicList();
    ASSERT_EQ(11, lst.size());
    ASSERT_STREQ("01", lst[0]->FileName());
    ASSERT_STREQ("08", lst[1]->FileName());
    ASSERT_STREQ("Wonder Woman Vol04 001 - こんにちは", lst[10]->FileName());
}

TEST(ComicLibrary, FileListLocal)
{
    ComicLibrary c(COMIC_LIBRARY);

    c.RefreshLibrary();

    ComicLibrary::ComicFolder folder = c.GetLibraryComics();
    vector<boost::shared_ptr<ComicLibraryItem> > items = c.GetCompleteComicList();

    ASSERT_EQ(CURRENT_COMIC_NUMBER, c.GetComicNumber());
    ASSERT_EQ(CURRENT_COMIC_NUMBER, items.size());
    ASSERT_EQ(14, folder.Folders.size());
    ASSERT_EQ(0, folder.Comics.size());

    ASSERT_STREQ("America's Best", c.GetLibraryComics().Folders[0].name.c_str());
    ASSERT_STREQ("Archaia", c.GetLibraryComics().Folders[1].name.c_str());
    ASSERT_STREQ("Zenescope", c.GetLibraryComics().Folders[c.GetLibraryComics().Folders.size() - 1].name.c_str());

    string s265 = items[CURRENT_COMIC_NUMBER - 2]->FileName();
    string s266 = items[CURRENT_COMIC_NUMBER - 1]->FileName();

    ASSERT_STREQ("30 Days of Night Vol1 01", items[0]->FileName());
    ASSERT_STREQ("Action Comics (2011-) 001", items[1]->FileName());
    ASSERT_STREQ("World of Warcraft 000", s265.c_str());
    ASSERT_STREQ("World of Warcraft 001", s266.c_str());
}

TEST(ComicLibrary, FileListLocalModTime)
{
    ComicLibrary c(COMIC_LIBRARY);

    c.RefreshLibrary();

    vector<boost::shared_ptr<ComicLibraryItem> > items = c.GetCompleteComicList();

    for (auto it = items.begin(); it != items.end(); it++)
    {
        const boost::shared_ptr<ComicLibraryItem>& item = *it;

        time_t t = item->ModificationDate();
        struct tm* tm = ::localtime(&t);
        ASSERT_GT(tm->tm_year + 1900, 2010);
        ASSERT_LT(tm->tm_year + 1900, 2020);
    }
}
