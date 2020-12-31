/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <ComicFile.hpp>
#include <CbrComicFile.hpp>
#include <XUtilities.hpp>
#include <boost/smart_ptr.hpp>

using namespace ComicSeer;
using namespace std;
using namespace boost;

TEST(CbrLevelFile, ImageListNumberNotLoaded)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));

    ASSERT_EQ(uint32_t(0), comicFile->NumberOfPages());
}

TEST(CbrLevelFile, TitleImageNotLoaded)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));

	XByteArray data;
	ASSERT_THROW(comicFile->GetTitleImage(data), FileException);
}

TEST(CbrLevelFile, ImageListNumber)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));
	comicFile->Initialize();

	ASSERT_EQ(26, comicFile->NumberOfPages());
}

TEST(CbrLevelFile, ImageList)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));
	comicFile->Initialize();

	list<string> entries = comicFile->ImageList();

#ifdef _WIN32
    EXPECT_STREQ(entries.begin()->c_str(),			"Wonder Woman Vol04 001\\Level1\\01.jpg");
    EXPECT_STREQ((++entries.begin())->c_str(),		"Wonder Woman Vol04 001\\Level1\\02.jpg");
    EXPECT_STREQ((--(--entries.end()))->c_str(),	"Wonder Woman Vol04 001\\Level2\\25.jpg");
    EXPECT_STREQ((--entries.end())->c_str(),		"Wonder Woman Vol04 001\\Level2\\26.jpg");
#else
    EXPECT_STREQ(entries.begin()->c_str(),			"Wonder Woman Vol04 001/Level1/01.jpg");
    EXPECT_STREQ((++entries.begin())->c_str(),		"Wonder Woman Vol04 001/Level1/02.jpg");
    EXPECT_STREQ((--(--entries.end()))->c_str(),	"Wonder Woman Vol04 001/Level2/25.jpg");
    EXPECT_STREQ((--entries.end())->c_str(),		"Wonder Woman Vol04 001/Level2/26.jpg");
#endif
}

TEST(CbrLevelFile, NonImageList)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));
	comicFile->Initialize();

	list<string> entries = comicFile->NonImageList();
    ASSERT_EQ(size_t(4), entries.size());
#ifdef _WIN32
    EXPECT_STREQ((--entries.end())->c_str(),			"Wonder Woman Vol04 001\\Level2\\lint.xml");
#else
    EXPECT_STREQ((--entries.end())->c_str(),			"Wonder Woman Vol04 001/Level2/lint.xml");
#endif
}

TEST(CbrLevelFile, TitleImage)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));
	comicFile->Initialize();

	XByteArray data1;
	comicFile->GetTitleImage(data1);

	XByteArray data2;
	XUtilities::ReadFile("01.jpg", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbrLevelFile, AltImage)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));
	comicFile->Initialize();

	XByteArray data1;
#ifdef _WIN32
    comicFile->GetFile("Wonder Woman Vol04 001\\Level1\\08.jpg", data1);
#else
    comicFile->GetFile("Wonder Woman Vol04 001/Level1/08.jpg", data1);
#endif

	XByteArray data2;
	XUtilities::ReadFile("08.jpg", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbrLevelFile, AltFile)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));
	comicFile->Initialize();

	XByteArray data1;
#ifdef _WIN32
    comicFile->GetFile("Wonder Woman Vol04 001\\Level2\\ComicInfo.xml", data1);
#else
    comicFile->GetFile("Wonder Woman Vol04 001/Level2/ComicInfo.xml", data1);
#endif

	XByteArray data2;
	XUtilities::ReadFile("ComicInfo.xml", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbrLevelFile, ComicInfo)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001 - Multilevel.cbr"));
	comicFile->Initialize();

	XByteArray data1;
	ASSERT_TRUE(comicFile->GetComicInformation(data1));

	XByteArray data2;
	XUtilities::ReadFile("ComicInfo.xml", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}
