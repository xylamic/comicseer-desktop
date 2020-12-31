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

TEST(CbrFile, CbrInstance)
{
	ComicFile* comicFile = ComicFile::CreateComicFile(".sjfigthisisgibberish/\\fs.f\\sld\\SingleImage.cbr");

	CbrComicFile* cbrComicFile = dynamic_cast<CbrComicFile*>(comicFile);
	ASSERT_TRUE(cbrComicFile != NULL);

	ComicFile::DisposeComicFile(comicFile);
}

TEST(CbrFile, ImageListNumberNotLoaded)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));

    ASSERT_EQ(uint32_t(0), comicFile->NumberOfPages());
}

TEST(CbrFile, TitleImageNotLoaded)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));

	XByteArray data;
	ASSERT_THROW(comicFile->GetTitleImage(data), FileException);
}

TEST(CbrFile, ImageListNumber)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));
	comicFile->Initialize();

	ASSERT_EQ(26, comicFile->NumberOfPages());
}

TEST(CbrFile, ImageList)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));
	comicFile->Initialize();

	list<string> entries = comicFile->ImageList();
	EXPECT_STREQ(entries.begin()->c_str(),			"01.jpg");
	EXPECT_STREQ((++entries.begin())->c_str(),		"02.jpg");
	EXPECT_STREQ((--(--entries.end()))->c_str(),	"25.jpg");
	EXPECT_STREQ((--entries.end())->c_str(),		"26.jpg");
}

TEST(CbrFile, NonImageList)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));
	comicFile->Initialize();

	list<string> entries = comicFile->NonImageList();
    ASSERT_EQ(size_t(1), entries.size());
	EXPECT_STREQ(entries.begin()->c_str(),			"lint.xml");
}

TEST(CbrFile, TitleImage)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));
	comicFile->Initialize();

	XByteArray data1;
	comicFile->GetTitleImage(data1);

	XByteArray data2;
	XUtilities::ReadFile("01.jpg", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbrFile, AltImage)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));
	comicFile->Initialize();

	XByteArray data1;
	comicFile->GetFile("08.jpg", data1);

	XByteArray data2;
	XUtilities::ReadFile("08.jpg", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbrFile, AltFile)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));
	comicFile->Initialize();

	XByteArray data1;
	comicFile->GetFile("ComicInfo.xml", data1);

	XByteArray data2;
	XUtilities::ReadFile("ComicInfo.xml", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbrFile, ComicInfo)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr"));
	comicFile->Initialize();

	XByteArray data1;
	ASSERT_TRUE(comicFile->GetComicInformation(data1));

	XByteArray data2;
	XUtilities::ReadFile("ComicInfo.xml", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}
