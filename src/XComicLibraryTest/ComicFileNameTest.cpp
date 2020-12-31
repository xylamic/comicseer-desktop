/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <ComicFileName.hpp>
#include <XUtilities.hpp>

using namespace ComicSeer;
using namespace std;

void CompareComicFileNames(const char* original, const char* base, const char* pretty,
                           int volume, int part, double issue, const char* orignalClean = NULL)
{
    ComicFileName c(original);

    if (orignalClean == NULL)
        EXPECT_STREQ(original, c.FileName());
    else
        EXPECT_STREQ(orignalClean, c.FileName());

    EXPECT_STREQ(base, c.BaseName());
    EXPECT_STREQ(pretty, c.PrettyName());

    if (volume < 0)
        EXPECT_TRUE(NULL == c.VolumeNumber());
    else
        EXPECT_TRUE(volume == *c.VolumeNumber());

    if (part < 0)
        EXPECT_TRUE(NULL == c.PartNumber());
    else
        EXPECT_TRUE(part == *c.PartNumber());

    if (issue < 0)
        EXPECT_TRUE(NULL == c.IssueNumber());
    else
        EXPECT_TRUE(issue == *c.IssueNumber());
}

TEST(ComicFileName, Basic)
{
    CompareComicFileNames("Wonder Woman 002", "Wonder Woman", "Wonder Woman #2", -1, -1, 2);
    CompareComicFileNames("007 000010", "007", "007 #10", -1, -1, 10);
    CompareComicFileNames("Jungle Book 010", "Jungle Book", "Jungle Book #10", -1, -1, 10);
}

TEST(ComicFileName, Combo)
{
    CompareComicFileNames("Wonder Woman Vol2 002", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman V02 002", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman Vol03 002", "Wonder Woman", "Wonder Woman V3 #2", 3, -1, 2);
    CompareComicFileNames("Wonder Woman Volume 2 002", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman VOL2 002", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman vol 002 002", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman v 002 002", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman V 002 002", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman V 002 002 (of 23)", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
}

TEST(ComicFileName, Complex)
{
    CompareComicFileNames("Wonder Woman - Vol 02 - 002", "Wonder Woman", "Wonder Woman V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman V 002 V002 002", "Wonder Woman V 002", "Wonder Woman V 002 V2 #2", 2, -1, 2);
    CompareComicFileNames("Wonder Woman V1 Vol1 #010", "Wonder Woman V1", "Wonder Woman V1 V1 #10", 1, -1, 10);
    CompareComicFileNames("Wonder Woman Vol1 010 Part1", "Wonder Woman", "Wonder Woman V1 P1 #10", 1, 1, 10);
    CompareComicFileNames("Wonder Woman Vol1 010 Part1 ( of 25 ) ", "Wonder Woman", "Wonder Woman V1 P1 #10", 1, 1, 10, "Wonder Woman Vol1 010 Part1 ( of 25 )");
    CompareComicFileNames("TestP V01 323", "TestP", "TestP V1 #323", 1, -1, 323);
}

TEST(ComicFileName, RealExamples)
{
    CompareComicFileNames("David Mazzucchelli - Asterios Polyp.cbz", "David Mazzucchelli - Asterios Polyp", "David Mazzucchelli - Asterios Polyp", -1, -1, -1, "David Mazzucchelli - Asterios Polyp");
    CompareComicFileNames("Mind the Gap 001", "Mind the Gap", "Mind the Gap #1", -1, -1, 1);
    CompareComicFileNames("American Vampire - Survival of the Fittest 1.cbz", "American Vampire - Survival of the Fittest", "American Vampire - Survival of the Fittest #1", -1, -1, 1, "American Vampire - Survival of the Fittest 1");
    CompareComicFileNames("Sweet Tooth (2009-) 003.cbz", "Sweet Tooth (2009-)", "Sweet Tooth (2009-) #3", -1, -1, 3, "Sweet Tooth (2009-) 003");
    CompareComicFileNames("Night of 1000 Wolves 2.cbz", "Night of 1000 Wolves", "Night of 1000 Wolves #2", -1, -1, 2, "Night of 1000 Wolves 2");
}

TEST(ComicFileName, Multilanguage)
{
    CompareComicFileNames("動物マン 001", "動物マン", "動物マン #1", -1, -1, 1);
}

TEST(ComicFileName, Paths)
{
    CompareComicFileNames("C:\\abc\\sjke\\Wonder Woman 002.cbr", "Wonder Woman", "Wonder Woman #2", -1, -1, 2, "Wonder Woman 002");
    CompareComicFileNames("./lkjjf/$@@fjskf/007 000010.cbz", "007", "007 #10", -1, -1, 10, "007 000010");
    CompareComicFileNames(".\\lkjsjej/cjsle\\Jungle Book 010", "Jungle Book", "Jungle Book #10", -1, -1, 10, "Jungle Book 010");
    CompareComicFileNames("Jungle Book 010.cb7", "Jungle Book", "Jungle Book #10", -1, -1, 10, "Jungle Book 010");
}

TEST(ComicFileName, SpecialCases)
{
    CompareComicFileNames("Wonder Woman (2011-) 023.1.cbz", "Wonder Woman (2011-)", "Wonder Woman (2011-) #23.1", -1, -1, 23.1, "Wonder Woman (2011-) 023.1");
    CompareComicFileNames("Wonder Woman  023.1 (2011-) .cbz", "Wonder Woman", "Wonder Woman #23.1", -1, -1, 23.1, "Wonder Woman  023.1 (2011-)");
    CompareComicFileNames("2000 AD (1977) #1873 (2014).cbr", "2000 AD (1977)", "2000 AD (1977) #1873", -1, -1, 1873, "2000 AD (1977) #1873 (2014)");
}
