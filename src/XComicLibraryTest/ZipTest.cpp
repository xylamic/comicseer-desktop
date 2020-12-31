/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <ComicFile.hpp>
#include <XUtilities.hpp>
#include <XZip.hpp>
#include <fstream>
#include <XException.hpp>
#include <FileFormatMinizipException.h>

using namespace ComicSeer;
using namespace std;

TEST(Zip, NumberOfFiles)
{
	XZip zip("Wonder Woman Vol04 001.cbz");
    EXPECT_EQ(uint64_t(28), zip.NumberOfEntries());
}

TEST(Zip, FileList)
{
	XZip zip("Wonder Woman Vol04 001.cbz");

	EXPECT_EQ(28, zip.NumberOfEntries());

	list<string> entries;
	zip.FileEntries(entries);

	EXPECT_EQ(28, entries.size());
	EXPECT_STREQ("01.jpg", entries.begin()->c_str());
	EXPECT_STREQ("02.jpg", (++entries.begin())->c_str());
	EXPECT_STREQ("ComicInfo.xml", (--(--entries.end()))->c_str());
	EXPECT_STREQ("lint.xml", (--entries.end())->c_str());
}

TEST(Zip, FileContent1)
{
	XZip zip("Wonder Woman Vol04 001.cbz");

	XByteArray data;
	zip.ReadFile("ComicInfo.xml", data);

	XByteArray odata;
	XUtilities::ReadFile("ComicInfo.xml", odata);

	EXPECT_TRUE(XUtilities::CompareByteArrays(data.ConstData(), data.Size(), odata.ConstData(), odata.Size()));
}

TEST(Zip, FileContent2)
{
	XZip zip("Wonder Woman Vol04 001.cbz");

	XByteArray data;
	zip.ReadFile("01.jpg", data);

	XByteArray odata;
	XUtilities::ReadFile("01.jpg", odata);

	EXPECT_TRUE(XUtilities::CompareByteArrays(data.ConstData(), data.Size(), odata.ConstData(), odata.Size()));
}

TEST(Zip, FileNotFound)
{
	XZip zip("Wonder Woman Vol04 Excep.cbz");

	XByteArray data;
	ASSERT_THROW(zip.ReadFile("ComicInfo.xml", data), FileException);
}

TEST(Zip, FileBad)
{
	XZip zip("ComicInfo.xml");

	XByteArray data;
	ASSERT_THROW(zip.ReadFile("ComicInfo.xml", data), FileException);
}
