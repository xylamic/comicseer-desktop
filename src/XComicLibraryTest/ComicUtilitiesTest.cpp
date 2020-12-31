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

using namespace ComicSeer;
using namespace std;

const char* comicListTestFile = "ComicList.txt";

TEST(ComicUtilities, ComicList)
{
    vector<string> l;
    l.push_back("c1");
    l.push_back("c2");
    l.push_back("c5");
    l.push_back("c4");

    vector<string> l2;
    l2.push_back("c2");
    l2.push_back("c5");
    l2.push_back("c2");

    map<string, vector<string> > m;
    m["comic2"] = l2;
    m["comic1"] = l;

    ComicUtilities::SaveComicList(m, comicListTestFile);

#ifdef _WIN32
    const char *valid = "comic1\r\n=======================\r\nc1\r\nc2\r\nc5\r\nc4\r\n\r\ncomic2\r\n=======================\r\nc2\r\nc5\r\nc2\r\n\r\n";
#else
    const char* valid = "comic1\n=======================\nc1\nc2\nc5\nc4\n\ncomic2\n=======================\nc2\nc5\nc2\n\n";
#endif

    XByteArray data;
    XUtilities::ReadFile(comicListTestFile, data);

    EXPECT_TRUE(XUtilities::CompareByteArrays(data.ConstData(), data.Size(), valid, strlen(valid)));
}

