/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <ComicInfo.hpp>
#include <XUtilities.hpp>
#include <XException.hpp>
#include <ComicFile.hpp>

using namespace ComicSeer;
using namespace std;

TEST(ComicInfo, EmptyFile)
{
    XByteArray content;

    EXPECT_THROW(ComicInfo ci(content), XmlFileReadException);
}

TEST(ComicInfo, InvalidFile)
{
    XByteArray content;
    content.Append("h><ello", 7);
    EXPECT_THROW(ComicInfo ci(content), XmlFileReadException);
}

TEST(ComicInfo, BasicFile)
{
    XByteArray data;
    XUtilities::ReadFile("ComicInfo.xml", data);

    ComicInfo ci(data);
    EXPECT_STREQ("The Visitation", ci.Title().c_str());
}

TEST(ComicInfo, NonenglishFile)
{
    XByteArray data;
    XUtilities::ReadFile("ComicInfoNonEnglish.xml", data);

    ComicInfo ci(data);
    EXPECT_STREQ("こんにちは", ci.AlternateSeries().c_str());
}

void ValidateComicInfo(const ComicInfo &ci, bool nonEnglish, bool complete = false)
{
    EXPECT_STREQ("The Visitation", ci.Title().c_str());
    EXPECT_STREQ("Wonder Woman", ci.Series().c_str());
    EXPECT_STREQ(nonEnglish ? "こんにちは" : "", ci.AlternateSeries().c_str());
    EXPECT_STREQ("1", ci.Number().c_str());
    EXPECT_EQ(complete ? 2 : -1, ci.Count());
    EXPECT_EQ(2011, ci.Volume());
    EXPECT_STREQ(complete ? "Recherche d'articles..." : "", ci.AlternateNumber().c_str());
    EXPECT_EQ(complete ? 43 : -1, ci.AlternateCount());
    EXPECT_STREQ("The Gods walk among us. To them, our lives are playthings. Only one woman would dare to protect "
                 "humanity from the wrath of such strange and powerful forces. But is she one of us – or one of them?\n"
                 "The issue opens with a mythical figure of some sort with three women overlooking a city from a "
                 "skyscraper. After conversing with them he possesses them. The story cuts to a rural farm where a "
                 "female character bearing a cape made of peacock brings forth some centaur like creatures. They have "
                 "been summoned to kill a nearby woman, but at the last moment she is confronted by a man with a key. "
                 "Upon grasping the key, the woman is teleported to London to the bedroom of Diana. Here she explains "
                 "what has happened to her, and Wonder Woman escorts her back to her house where she engages in combat "
                 "with the creatures. it is revealed that the man with the key was in fact the Greek God Hermes and "
                 "he was protecting the woman as she is pregnant with Zeus' child. As the story closes, the actions "
                 "of the story's narrator become more clear.", ci.Summary().c_str());
    EXPECT_STREQ("Scraped metadata from ComicVine [CVDB293307] on 2012.03.22 23:25:36.", ci.Notes().c_str());
    EXPECT_EQ(2011, ci.Year());
    EXPECT_EQ(11, ci.Month());
    EXPECT_STREQ("Brian Azzarello", ci.Writer().c_str());
    EXPECT_STREQ("Cliff Chiang", ci.Penciller().c_str());
    EXPECT_STREQ("Cliff Chiang", ci.Inker().c_str());
    EXPECT_STREQ("Matthew Wilson", ci.Colorist().c_str());
    EXPECT_STREQ("Jared K. Fletcher", ci.Letterer().c_str());
    EXPECT_STREQ(complete ? "The Artist" : "", ci.CoverArtist().c_str());
    EXPECT_STREQ("Chris Conroy, Matt Idelson", ci.Editor().c_str());
    EXPECT_STREQ("DC Comics", ci.Publisher().c_str());
    EXPECT_STREQ(complete ? "imprint" : "", ci.Imprint().c_str());
    EXPECT_STREQ(complete ? "Fun" : "", ci.Genre().c_str());
    EXPECT_STREQ("http://www.comicvine.com/wonder-woman-the-visitation/37-293307/", ci.Web().c_str());
    EXPECT_EQ(26, ci.PageCount());
    EXPECT_STREQ(complete ? "Variable" : "", ci.LanguageISO().c_str());
    EXPECT_STREQ(complete ? "Normal" : "", ci.Format().c_str());
    EXPECT_EQ(complete ? (int)ComicInfo::YES : (int)ComicInfo::UNDEFINED, (int)ci.IsBlackAndWhite());
    EXPECT_EQ(complete ? (int)ComicInfo::NO : (int)ComicInfo::UNDEFINED, (int)ci.IsManga());
    EXPECT_STREQ("Pandora, Hermes, Apollo, Zola, Hera, Wonder Woman", ci.Characters().c_str());
    EXPECT_STREQ("Amazons of Themyscira", ci.Teams().c_str());
    EXPECT_STREQ("Virginia , Singapore, London", ci.Locations().c_str());
}

TEST(ComicInfo, CompleteFileTest1)
{
    XByteArray data;
    XUtilities::ReadFile("ComicInfoNonEnglish.xml", data);

    ComicInfo ci(data);
    ValidateComicInfo(data, true);
}

TEST(ComicInfo, CompleteFileTest2)
{
    ComicFile *file = ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbz");
    file->Initialize();

    EXPECT_TRUE(file->HasComicInformationFile());

    XByteArray data;
    file->GetComicInformation(data);

    ComicInfo ci(data);
    ValidateComicInfo(ci, false);

    ComicFile::DisposeComicFile(file);
}

TEST(ComicInfo, CompleteFileTest3)
{
    ComicFile *file = ComicFile::CreateComicFile("Wonder Woman Vol04 001.cbr");
    file->Initialize();

    EXPECT_TRUE(file->HasComicInformationFile());

    XByteArray data;
    file->GetComicInformation(data);

    ComicInfo ci(data);
    ValidateComicInfo(ci, false);

    ComicFile::DisposeComicFile(file);
}

TEST(ComicInfo, CompleteFileTest4)
{
    XByteArray data;
    XUtilities::ReadFile("ComicInfoComplete.xml", data);

    ComicInfo ci(data);
    ValidateComicInfo(data, true, true);
}
