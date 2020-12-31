/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <Bookmarkings.hpp>
#include <XUtilities.hpp>
#include <boost/uuid/uuid.hpp>
#include "XException.hpp"

using namespace ComicSeer;
using namespace std;
using namespace boost::uuids;

void CompareMarks(const Bookmark &expect, const Bookmark &actual)
{
    EXPECT_EQ(expect.Identifier(), actual.Identifier());
    EXPECT_STREQ(expect.Name().PrettyName(), actual.Name().PrettyName());
    EXPECT_STREQ(expect.Description(), actual.Description());
    EXPECT_STREQ(expect.Notes(), actual.Notes());
    EXPECT_EQ(expect.Page(), actual.Page());
    EXPECT_STREQ(expect.Path(), actual.Path());
    EXPECT_EQ(expect.Created(), actual.Created());
}

void FindCompareMarks(const Bookmark &expect, const vector<const Bookmark*> &bms)
{
    auto it = find_if(bms.begin(), bms.end(), [&expect](const Bookmark *mark)
    {
        return mark->Identifier() == expect.Identifier();
    });

    EXPECT_NE(bms.end(), it);

    CompareMarks(expect, **it);
}

TEST(Bookmarkings, MarkCheckEmpty)
{
    Bookmark mark("C:\\myfile 001.cbz", 3);
    EXPECT_NE(XUtilities::EmptyUuid(), mark.Identifier());
    EXPECT_STREQ("myfile #1", mark.Name().PrettyName());
    EXPECT_STREQ("", mark.Description());
    EXPECT_STREQ("", mark.Notes());
    EXPECT_EQ(3, mark.Page());
    EXPECT_STREQ("C:\\myfile 001.cbz", mark.Path());
    EXPECT_LT(::time(0) - 60, mark.Created());
    EXPECT_GE(::time(0), mark.Created());
}

TEST(Bookmarkings, MarkCheck)
{
    Bookmark mark("C:\\myfile 001.cbz", 3, "my desc", "my notes");
    EXPECT_NE(XUtilities::EmptyUuid(), mark.Identifier());
    EXPECT_STREQ("myfile #1", mark.Name().PrettyName());
    EXPECT_STREQ("my desc", mark.Description());
    EXPECT_STREQ("my notes", mark.Notes());
    EXPECT_EQ(3, mark.Page());
    EXPECT_STREQ("C:\\myfile 001.cbz", mark.Path());
    EXPECT_LT(::time(0) - 60, mark.Created());
    EXPECT_GE(::time(0), mark.Created());
}

TEST(Bookmarkings, AddTwo)
{
    Bookmarkings bm;

    Bookmark mark1("C:\\myfile 001.cbz", 66, "my d");
    Bookmark mark2("C:\\mfile 001.cbz", 3);

    bm.AddBookmark(mark1);
    bm.AddBookmark(mark2);

    const vector<const Bookmark*> bms = bm.Bookmarks();
    EXPECT_EQ(2, bms.size());

    const Bookmark *mark = bm.BookmarkById(mark1.Identifier());
    EXPECT_FALSE(NULL == mark);
    EXPECT_STREQ(XUtilities::BoostUuidToString(mark1.Identifier()).c_str(),
                 XUtilities::BoostUuidToString(mark->Identifier()).c_str());
    EXPECT_STREQ("myfile #1", mark->Name().PrettyName());
    EXPECT_STREQ("my d", mark->Description());
    EXPECT_STREQ("", mark->Notes());
    EXPECT_EQ(66, mark->Page());
    EXPECT_STREQ("C:\\myfile 001.cbz", mark->Path());
    EXPECT_LT(::time(0) - 60, mark->Created());
    EXPECT_GE(::time(0), mark->Created());

    mark = bm.BookmarkById(mark2.Identifier());
    EXPECT_FALSE(NULL == mark);
    EXPECT_STREQ(XUtilities::BoostUuidToString(mark2.Identifier()).c_str(),
                 XUtilities::BoostUuidToString(mark->Identifier()).c_str());
    EXPECT_STREQ("mfile #1", mark->Name().PrettyName());
    EXPECT_STREQ("", mark->Description());
    EXPECT_STREQ("", mark->Notes());
    EXPECT_EQ(3, mark->Page());
    EXPECT_STREQ("C:\\mfile 001.cbz", mark->Path());
    EXPECT_LT(::time(0) - 60, mark->Created());
    EXPECT_GE(::time(0), mark->Created());
}

TEST(Bookmarkings, StoreLoad5)
{
    Bookmarkings bm;

    Bookmark mark1("C:\\myfile 001.cbz", 66, "my d");
    Bookmark mark2("C:\\mfile 001.cbz", 3);
    Bookmark mark3("C:\\myile 011.cbz", 1, "m");
    Bookmark mark4("C:\\myfle 101.cbz", 23, "");
    Bookmark mark5("C:\\myfil 111.cbz", 42, "my d", "notes");

    bm.AddBookmark(mark1);
    bm.AddBookmark(mark2);
    bm.AddBookmark(mark3);
    bm.AddBookmark(mark4);
    bm.AddBookmark(mark5);

    const vector<const Bookmark*> bms1 = bm.Bookmarks();
    EXPECT_EQ(5, bms1.size());

    XUtilities::DelFile("bookmarks.xml");
    bm.Store("bookmarks.xml");

    bm.Load("bookmarks.xml");
    const vector<const Bookmark*> bms2 = bm.Bookmarks();
    EXPECT_EQ(5, bms2.size());
    FindCompareMarks(mark1, bms2);
    FindCompareMarks(mark2, bms2);
    FindCompareMarks(mark3, bms2);
    FindCompareMarks(mark4, bms2);
    FindCompareMarks(mark5, bms2);
}

TEST(Bookmarkings, StoreLoadRemoveStoreLoad)
{
    Bookmarkings bm;

    Bookmark mark1("C:\\myfile 001.cbz", 66, "my d");
    Bookmark mark2("C:\\mfile 001.cbz", 3);
    Bookmark mark3("C:\\myile 011.cbz", 1, "m");
    Bookmark mark4("C:\\myfle 101.cbz", 23, "");
    Bookmark mark5("C:\\myfil 111.cbz", 42, "my d", "notes");

    bm.AddBookmark(mark1);
    bm.AddBookmark(mark2);
    bm.AddBookmark(mark3);
    bm.AddBookmark(mark4);
    bm.AddBookmark(mark5);

    const vector<const Bookmark*> bms1 = bm.Bookmarks();
    EXPECT_EQ(5, bms1.size());

    bm.Store("bookmarks.xml");

    bm.Load("bookmarks.xml");
    const vector<const Bookmark*> bms2 = bm.Bookmarks();
    EXPECT_EQ(5, bms2.size());
    FindCompareMarks(mark1, bms2);
    FindCompareMarks(mark2, bms2);
    FindCompareMarks(mark3, bms2);
    FindCompareMarks(mark4, bms2);
    FindCompareMarks(mark5, bms2);

    bm.RemoveBookmark(mark3.Identifier());
    bm.RemoveBookmark(mark1.Identifier());
    bm.RemoveBookmark(mark5.Identifier());

    bm.Store("bookmarks.xml");

    Bookmarkings bm2;
    bm2.Load("bookmarks.xml");

    const vector<const Bookmark*> bms3 = bm.Bookmarks();
    EXPECT_EQ(2, bms3.size());
    FindCompareMarks(mark2, bms3);
    FindCompareMarks(mark4, bms3);
}

TEST(Bookmarkings, ReadFailNone)
{
    XUtilities::DelFile("bookmarks.xml");

    Bookmarkings bm;
    EXPECT_THROW(bm.Load("bookmark.xml"), XmlFileReadException);
}

TEST(Bookmarkings, ReadFailEmpty)
{
    fstream file;
    XUtilities::Openfstream(file, "bookmarks.xml", ios_base::out | ios_base::trunc);
    file.write("", 4);
    file.close();

    Bookmarkings bm;
    EXPECT_THROW(bm.Load("bookmark.xml"), XmlFileReadException);
}

TEST(Bookmarkings, ReadFailCorrupt)
{
    fstream file;
    XUtilities::Openfstream(file, "bookmarks.xml", ios_base::out | ios_base::trunc);
    file.write("<xml", 4);
    file.close();

    Bookmarkings bm;
    EXPECT_THROW(bm.Load("bookmark.xml"), XmlFileReadException);
}
