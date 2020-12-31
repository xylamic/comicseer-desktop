/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <ComicFile.hpp>
#include <PdfComicFile.hpp>
#include <XUtilities.hpp>
#include <boost/smart_ptr.hpp>


#ifdef PDFSUPPORT
using namespace ComicSeer;
using namespace std;
using namespace boost;

TEST(PdfComicFile, ValidateInstalled)
{
    PdfComicFile::ValidateSupported();
}

TEST(PdfComicFile, ReadContent)
{
    ComicFile *file = ComicFile::CreateComicFile("Intelligent Systems.pdf");
    file->Initialize();

    list<string> imageList = file->ImageList();
    EXPECT_EQ(360, imageList.size());
    EXPECT_EQ("0001.png", *imageList.begin());
    EXPECT_EQ("0360.png", *(--imageList.end()));
}

TEST(PdfComicFile, Read1File)
{
    ComicFile *file = ComicFile::CreateComicFile("Intelligent Systems.pdf");
    file->Initialize();

    XByteArray data;
    file->GetFile("0001.png", data);

    XByteArray actData;
    XUtilities::ReadFile("pdf1.png", actData);

    EXPECT_TRUE(XUtilities::CompareByteArrays(data.Data(), data.Size(), actData.Data(), actData.Size()));
}
#endif
