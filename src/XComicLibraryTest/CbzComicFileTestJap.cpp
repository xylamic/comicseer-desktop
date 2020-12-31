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

string GetJapFileCbr()
{
    return "Wonder Woman Vol04 001 - こんにちは.cbz";
}

TEST(CbzFileJap, ImageListNumberNotLoaded)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));

    ASSERT_EQ(uint32_t(0), comicFile->NumberOfPages());
}

TEST(CbzFileJap, TitleImageNotLoaded)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));

    XByteArray data;
    ASSERT_THROW(comicFile->GetTitleImage(data), FileException);
}

TEST(CbzFileJap, ImageListNumber)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));
    comicFile->Initialize();

    ASSERT_EQ(26, comicFile->NumberOfPages());
}

TEST(CbzFileJap, ImageList)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));
    comicFile->Initialize();

    list<string> entries = comicFile->ImageList();
    EXPECT_STREQ(entries.begin()->c_str(),			"01.jpg");
    EXPECT_STREQ((++entries.begin())->c_str(),		"02.jpg");
    EXPECT_STREQ((--(--entries.end()))->c_str(),    "25.jpg");
    EXPECT_STREQ((--entries.end())->c_str(),    "26.jpg");
}

TEST(CbzFileJap, NonImageList)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));
    comicFile->Initialize();

    list<string> entries = comicFile->NonImageList();
    ASSERT_EQ(size_t(1), entries.size());
    EXPECT_STREQ(entries.begin()->c_str(),			"lint.xml");
}

TEST(CbzFileJap, TitleImage)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));
    comicFile->Initialize();

    XByteArray data1;
    comicFile->GetTitleImage(data1);

    XByteArray data2;
    XUtilities::ReadFile("01.jpg", data2);

    ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbzFileJap, AltImage)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));
    comicFile->Initialize();

    XByteArray data1;
    comicFile->GetFile("08.jpg", data1);

    XByteArray data2;
    XUtilities::ReadFile("08.jpg", data2);

    ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbzFileJap, AltFile)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));
    comicFile->Initialize();

    XByteArray data1;
    comicFile->GetFile("ComicInfo.xml", data1);

    XByteArray data2;
    XUtilities::ReadFile("ComicInfo.xml", data2);

    ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

TEST(CbzFileJap, ComicInfo)
{
    scoped_ptr<ComicFile> comicFile(ComicFile::CreateComicFile(GetJapFileCbr().c_str()));
    comicFile->Initialize();

    XByteArray data1;
    ASSERT_TRUE(comicFile->GetComicInformation(data1));

    XByteArray data2;
    XUtilities::ReadFile("ComicInfo.xml", data2);

    ASSERT_TRUE(XUtilities::CompareByteArrays(data1.ConstData(), data1.Size(), data2.ConstData(), data2.Size()));
}

