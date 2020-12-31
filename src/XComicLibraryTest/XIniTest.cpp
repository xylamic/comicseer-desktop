/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "Xylasoft.hpp"
#include <gtest/gtest.h>
#include <XIni.hpp>
#include <XUtilities.hpp>
#include <XByteArray.hpp>

using namespace std;
using namespace ComicSeer;

const char* testfile = "こんにちは.ini";

const char* complexstr1 = "kjd8O(&U$((Uln lslkhfhskk?????IGJUDKJSKKk??(?skkdk(&(3090!@#$%^&*()";
const char* complexstr2 = "kjd8O(&U$((Uln lsl%r\r%%\n\r%n%rr%n%%%rkhfhこんにちはs\r\nkkdjKIGJUDKJこんにちはk??(?skkdk(&(3090!@#$%^&*()";

TEST(XIni, Basic)
{
    XIni ini;
    string val = ini.GetValueForKey("section", "key");
    ASSERT_TRUE(val.empty());

    ini.SetKeyValueForSection("section", "key", "value");
    ASSERT_STREQ(ini.GetValueForKey("section", "key").c_str(), "value");

    ini.Save(testfile);

    XIni ini2(testfile);
    ASSERT_STREQ(ini2.GetValueForKey("section", "key").c_str(), "value");
}

TEST(XIni, ComplexString)
{
    XIni ini;
    string val = ini.GetValueForKey("section", "key");
    ASSERT_TRUE(val.empty());

    ini.SetKeyValueForSection("section", "key", complexstr1);
    ASSERT_STREQ(ini.GetValueForKey("section", "key").c_str(), complexstr1);

    ini.Save(testfile);

    XIni ini2(testfile);
    ASSERT_STREQ(ini2.GetValueForKey("section", "key").c_str(), complexstr1);
}

TEST(XIni, ComplexString2)
{
    XIni ini;
    string val = ini.GetValueForKey("section", "key");
    ASSERT_TRUE(val.empty());

    ini.SetKeyValueForSection("section", "key", complexstr2);
    string newval = ini.GetValueForKey("section", "key");
    ASSERT_STREQ(ini.GetValueForKey("section", "key").c_str(), complexstr2);

    ini.Save(testfile);

    XIni ini2(testfile);
    ASSERT_STREQ(ini2.GetValueForKey("section", "key").c_str(), complexstr2);
}

TEST(XIni, Long1)
{
    XIni ini;
    long val;

    ASSERT_FALSE(ini.GetValueForKeyCast("section", "key", val));

    ini.SetKeyValueForSection("section", "key", -100);
    ASSERT_TRUE(ini.GetValueForKeyCast("section", "key", val));
    ASSERT_EQ(-100, val);

    ini.Save(testfile);

    XIni ini2(testfile);
    ASSERT_TRUE(ini2.GetValueForKeyCast("section", "key", val));
    ASSERT_EQ(-100, val);
}

TEST(XIni, Long2)
{
    XIni ini;
    long val;

    ASSERT_FALSE(ini.GetValueForKeyCast("section", "key", val));

    ini.SetKeyValueForSection("section", "key", 0L);
    ASSERT_TRUE(ini.GetValueForKeyCast("section", "key", val));
    ASSERT_EQ(0, val);

    ini.Save(testfile);

    XIni ini2(testfile);
    ASSERT_TRUE(ini2.GetValueForKeyCast("section", "key", val));
    ASSERT_EQ(0, val);
}


TEST(XIni, Long3)
{
    XIni ini;
    long val;

    ASSERT_FALSE(ini.GetValueForKeyCast("section", "key", val));

    ini.SetKeyValueForSection("section", "key", 100);
    ASSERT_TRUE(ini.GetValueForKeyCast("section", "key", val));
    ASSERT_EQ(100, val);

    ini.Save(testfile);

    XIni ini2(testfile);
    ASSERT_TRUE(ini2.GetValueForKeyCast("section", "key", val));
    ASSERT_EQ(100, val);
}

TEST(XIni, MultiLong)
{
    XIni ini;
    long val1, val2, val3, val4, val5;

    ASSERT_FALSE(ini.GetValueForKeyCast("section1", "key1", val1));
    ASSERT_FALSE(ini.GetValueForKeyCast("section1", "key2", val2));
    ASSERT_FALSE(ini.GetValueForKeyCast("section2", "key1", val3));
    ASSERT_FALSE(ini.GetValueForKeyCast("section1", "key3", val4));
    ASSERT_FALSE(ini.GetValueForKeyCast("section2", "key2", val5));

    ini.SetKeyValueForSection("section1", "key1", -100);
    ini.SetKeyValueForSection("section2", "key2", -99);
    ini.SetKeyValueForSection("section", "key3", -98);
    ini.SetKeyValueForSection("section1", "key2", -97);
    ini.SetKeyValueForSection("section2", "key3", -96);
    ini.SetKeyValueForSection("section", "key1", -95);
    ini.SetKeyValueForSection("section1", "key3", -1002350);
    ini.SetKeyValueForSection("section2", "key2", 100);
    ini.SetKeyValueForSection("section", "key1", 99);
    ini.SetKeyValueForSection("section1", "key1", 83);
    ini.SetKeyValueForSection("section2", "key2", 58);
    ini.SetKeyValueForSection("section", "key3", 29);
    ini.SetKeyValueForSection("section1", "key2", 92);
    ini.SetKeyValueForSection("section2", "key3", 95);
    ini.SetKeyValueForSection("section", "key1", 19384850);

    ASSERT_TRUE(ini.GetValueForKeyCast("section1", "key1", val1));
    ASSERT_TRUE(ini.GetValueForKeyCast("section1", "key2", val2));
    ASSERT_FALSE(ini.GetValueForKeyCast("section2", "key1", val3));
    ASSERT_TRUE(ini.GetValueForKeyCast("section1", "key3", val4));
    ASSERT_TRUE(ini.GetValueForKeyCast("section2", "key2", val5));

    ASSERT_EQ(83, val1);
    ASSERT_EQ(92, val2);
    ASSERT_EQ(-1002350, val4);
    ASSERT_EQ(58, val5);

    ini.Save(testfile);

    XIni ini2(testfile);

    ASSERT_TRUE(ini2.GetValueForKeyCast("section1", "key1", val1));
    ASSERT_TRUE(ini2.GetValueForKeyCast("section1", "key2", val2));
    ASSERT_FALSE(ini2.GetValueForKeyCast("section2", "key1", val3));
    ASSERT_TRUE(ini2.GetValueForKeyCast("section1", "key3", val4));
    ASSERT_TRUE(ini2.GetValueForKeyCast("section2", "key2", val5));

    ASSERT_EQ(83, val1);
    ASSERT_EQ(92, val2);
    ASSERT_EQ(-1002350, val4);
    ASSERT_EQ(58, val5);
}
