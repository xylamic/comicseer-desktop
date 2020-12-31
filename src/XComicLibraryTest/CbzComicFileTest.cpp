/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <ComicFile.hpp>
#include <CbzComicFile.hpp>
#include <XUtilities.hpp>
#include <boost/smart_ptr.hpp>

using namespace ComicSeer;
using namespace std;
using namespace boost;

TEST(CbzFile, BadFile)
{
	ASSERT_THROW(ComicFile::CreateComicFile("SingleImage.cbq"), FileFormatException);
}

TEST(CbzFile, CbzInstance)
{
	ComicFile* comicFile = ComicFile::CreateComicFile(".sjfigthisisgibberish/\\fs.f\\sld\\SingleImage.cbz");

	CbzComicFile* cbzComicFile = dynamic_cast<CbzComicFile*>(comicFile);
	ASSERT_TRUE(cbzComicFile != NULL);

	ComicFile::DisposeComicFile(cbzComicFile);
}

TEST(CbzFile, SingleImageNoComicInfo)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("SingleImage.cbz"));

	XByteArray content;
	ASSERT_FALSE(comicFile->GetComicInformation(content));
}

TEST(CbzFile, ImageListNumberNotLoaded)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));

    ASSERT_EQ(uint32_t(0), comicFile->NumberOfPages());
}

TEST(CbzFile, TitleImageNotLoaded)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));

	XByteArray data;
	ASSERT_THROW(comicFile->GetTitleImage(data), FileException);
}

TEST(CbzFile, ImageListNumber)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));
	comicFile->Initialize();

	ASSERT_EQ(26, comicFile->NumberOfPages());
}

TEST(CbzFile, ImageList)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));
	comicFile->Initialize();

	list<string> entries = comicFile->ImageList();
	EXPECT_STREQ(entries.begin()->c_str(),			"01.jpg");
	EXPECT_STREQ((++entries.begin())->c_str(),		"02.jpg");
	EXPECT_STREQ((--(--entries.end()))->c_str(),	"25.jpg");
	EXPECT_STREQ((--entries.end())->c_str(),		"26.jpg");
}

TEST(CbzFile, NonImageList)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));
	comicFile->Initialize();

	list<string> entries = comicFile->NonImageList();
    ASSERT_EQ(size_t(1), entries.size());
	EXPECT_STREQ(entries.begin()->c_str(),			"lint.xml");
}

TEST(CbzFile, TitleImage)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));
	comicFile->Initialize();

	XByteArray data1;
	comicFile->GetTitleImage(data1);

	XByteArray data2;
	XUtilities::ReadFile("01.jpg", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbzFile, AltImage)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));
	comicFile->Initialize();

	XByteArray data1;
	comicFile->GetFile("08.jpg", data1);

	XByteArray data2;
	XUtilities::ReadFile("08.jpg", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbzFile, AltFile)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));
	comicFile->Initialize();

	XByteArray data1;
	comicFile->GetFile("ComicInfo.xml", data1);

	XByteArray data2;
	XUtilities::ReadFile("ComicInfo.xml", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbzFile, ComicInfo)
{
	scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz"));
	comicFile->Initialize();

	XByteArray data1;
	ASSERT_TRUE(comicFile->GetComicInformation(data1));

	XByteArray data2;
	XUtilities::ReadFile("ComicInfo.xml", data2);

	ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}
