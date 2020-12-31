/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicUtilities.hpp"
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <XUtilities.hpp>
#include <XException.hpp>
#include <cstdio>
#include <sstream>
#include <boost/regex.hpp>
#include <boost/regex/v4/match_results.hpp>
#include <boost/regex/v4/regex.hpp>
#include <iostream>

using namespace ComicSeer;
using namespace std;

std::list<std::string> ComicUtilities::SupportedImageExtensions()
{
	list<string> exts;

	exts.push_back("png");
	exts.push_back("jpg");
	exts.push_back("jpeg");
	exts.push_back("bmp");
	exts.push_back("gif");
    exts.push_back("tif");
    exts.push_back("tiff");

	return exts;
}

string ComicUtilities::StorageDirectory()
{
	return "";
}

string ComicUtilities::TemporaryDirectory()
{
	return "";
}

double ComicUtilities::ScaleKeepAspectRatio(int imageWidth, int imageHeight, int maxWidth, int maxHeight)
{
	double widthScale = maxWidth / (double)imageWidth;	double heightScale = maxHeight / (double)imageHeight;
	
    double scale = min(widthScale, heightScale);

    return scale;
}

void ComicUtilities::SaveComicList(const std::map<std::string, std::vector<std::string> >& names, const char* path)
{
    fstream file;
    XUtilities::Openfstream(file, path, ios_base::out | ios_base::trunc);

    static const char* separator = "=======================";

    if (!file.is_open())
    {
        throw FileException("The file could not be opened for write.");
    }

    for (auto itlib = begin(names); itlib != end(names); itlib++)
    {
        file.write(itlib->first.c_str(), itlib->first.size());
        file.write("\n", 1);
        file.write(separator, strlen(separator));
        file.write("\n", 1);

        vector<string> nameList = itlib->second;
        for (auto itfile = begin(nameList); itfile != end(nameList); itfile++)
        {
            file.write(itfile->c_str(), itfile->size());
            file.write("\n", 1);
        }

        file.write("\n", 1);
    }

    file.flush();
    file.close();
}
