/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <XUtilities.hpp>
#include <ctime>
#include <cstdio>

using namespace std;
using namespace ComicSeer;

bool FindMatchingNameType(vector<XUtilities::XFileDesc>& list, const char* compareName, bool isFile)
{
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (it->name.compare(compareName) == 0 && it->isFile == isFile)
        {
            return true;
        }
    }

    return false;
}

TEST(XUtilities, LocalDirectory)
{
    vector<XUtilities::XFileDesc> entries;
    XUtilities::GetFilesInDirectory(".", entries);

    ASSERT_TRUE(FindMatchingNameType(entries, "main.cpp", true));
    ASSERT_TRUE(FindMatchingNameType(entries, "こんにちは.ini", true));
    ASSERT_FALSE(FindMatchingNameType(entries, "bob.cpp", true));
    ASSERT_TRUE(FindMatchingNameType(entries, "XComicLibraryTest.pro", true));
    ASSERT_FALSE(FindMatchingNameType(entries, "TestDir", true));

    ASSERT_TRUE(FindMatchingNameType(entries, "TestDir", false));
    ASSERT_FALSE(FindMatchingNameType(entries, ".", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "..", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "main.cpp", false));
    ASSERT_TRUE(FindMatchingNameType(entries, "TestDir", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "こんにちは", false));
}

TEST(XUtilities, LocalDirectoryRelative)
{
    vector<XUtilities::XFileDesc> entries;
    XUtilities::GetFilesInDirectory("./../XComicLibraryTest", entries);

    ASSERT_TRUE(FindMatchingNameType(entries, "main.cpp", true));
    ASSERT_TRUE(FindMatchingNameType(entries, "こんにちは.ini", true));
    ASSERT_FALSE(FindMatchingNameType(entries, "bob.cpp", true));
    ASSERT_TRUE(FindMatchingNameType(entries, "XComicLibraryTest.pro", true));
    ASSERT_FALSE(FindMatchingNameType(entries, "TestDir", true));

    ASSERT_TRUE(FindMatchingNameType(entries, "TestDir", false));
    ASSERT_FALSE(FindMatchingNameType(entries, ".", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "..", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "main.cpp", false));
    ASSERT_TRUE(FindMatchingNameType(entries, "TestDir", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "こんにちは", false));
}

TEST(XUtilities, LocalDirectoryRelative2)
{
    vector<XUtilities::XFileDesc> entries;

#ifdef _WIN32
    XUtilities::GetFilesInDirectory("..\\XComicLibraryTest", entries);
#else
    XUtilities::GetFilesInDirectory("../XComicLibraryTest", entries);
#endif

    ASSERT_TRUE(FindMatchingNameType(entries, "main.cpp", true));
    ASSERT_TRUE(FindMatchingNameType(entries, "こんにちは.ini", true));
    ASSERT_FALSE(FindMatchingNameType(entries, "bob.cpp", true));
    ASSERT_TRUE(FindMatchingNameType(entries, "XComicLibraryTest.pro", true));
    ASSERT_FALSE(FindMatchingNameType(entries, "TestDir", true));

    ASSERT_TRUE(FindMatchingNameType(entries, "TestDir", false));
    ASSERT_FALSE(FindMatchingNameType(entries, ".", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "..", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "main.cpp", false));
    ASSERT_TRUE(FindMatchingNameType(entries, "TestDir", false));
    ASSERT_FALSE(FindMatchingNameType(entries, "こんにちは", false));
}

TEST(XUtilities, LocalDirectorySubDir)
{
    vector<XUtilities::XFileDesc> entries;
    XUtilities::GetFilesInDirectory("TestDir", entries);

    ASSERT_EQ(size_t(2), entries.size());
    ASSERT_TRUE(FindMatchingNameType(entries, "こんにちは.ini", true));
    ASSERT_TRUE(FindMatchingNameType(entries, "XCoreLibraryTest.pro", true));
}

TEST(XUtilities, LocalDirectorySubDir2)
{
    vector<XUtilities::XFileDesc> entries;
    XUtilities::GetFilesInDirectory("TestDir/", entries);

    ASSERT_EQ(size_t(2), entries.size());
    ASSERT_TRUE(FindMatchingNameType(entries, "こんにちは.ini", true));
    ASSERT_TRUE(FindMatchingNameType(entries, "XCoreLibraryTest.pro", true));
}

TEST(XUtilities, LocalDirectoryModTime)
{
    vector<XUtilities::XFileDesc> entries;
    XUtilities::GetFilesInDirectory(".", entries);

    time_t currTime;
    time(&currTime);

    struct tm *timeinfo;
    timeinfo = localtime(&currTime);
    timeinfo->tm_year = timeinfo->tm_year - 3;

    time_t otime = mktime(timeinfo);

    for (auto it = entries.begin(); it != entries.end(); it++)
    {
        ASSERT_LT(otime, it->modTime);
    }
}

TEST(XUtilities, GetOS)
{
    string n, m, r, v;

    XUtilities::GetOperatingSystem(r, v);

}

TEST(XUtilities, EmptyUuid)
{
    boost::uuids::uuid u = { 0 };
    std::string s = XUtilities::BoostUuidToString(u);
    EXPECT_STREQ("00000000-0000-0000-0000-000000000000", s.c_str());
}

TEST(XUtilities, 4Uuids)
{
    boost::uuids::uuid u1 = { 0 };
    boost::uuids::uuid u2 = XUtilities::NewUuid();
    boost::uuids::uuid u3 = XUtilities::NewUuid();
    boost::uuids::uuid u4 = XUtilities::NewUuid();

    EXPECT_NE(u1, u2);
    EXPECT_NE(u1, u3);
    EXPECT_NE(u1, u4);
    EXPECT_NE(u2, u3);
    EXPECT_NE(u2, u4);
    EXPECT_NE(u3, u4);
}

TEST(XUtilities, 2UuidsCompare)
{
    boost::uuids::uuid u1 = XUtilities::StringToBoostUuid("00002300-F000-0AB0-0000-000002309800");
    boost::uuids::uuid u2 = XUtilities::StringToBoostUuid("00002300-F000-0AB0-0000-000002309800");

    EXPECT_EQ(u1, u2);
}
