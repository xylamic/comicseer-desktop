/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ImageComicFile.hpp"
#include <XUtilities.hpp>
#include <XException.hpp>
#include "ComicUtilities.hpp"
#include <boost/algorithm/string.hpp>

using namespace ComicSeer;
using namespace std;

ImageComicFile::ImageComicFile(const char *filepath)
    : ComicFile(filepath)
{
}

ComicFile::ComicFileType ImageComicFile::GetFileType() const
{
    return ComicFile::Image;
}

std::list<string> ImageComicFile::GetFileList() const
{
    string ext = boost::algorithm::to_lower_copy(XUtilities::GetFileExtension(this->FilePath().c_str()));

    // get the list of supported image extensions
    list<string> exts = ComicUtilities::SupportedImageExtensions();

    bool found = false;
    for (auto it = exts.begin(); it != exts.end(); it++)
    {
        if (strcmp(it->c_str(), ext.c_str()) == 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        throw FileFormatException("The image file format is unsupported.");
    }

    string filename = "Image.";
    filename.append(ext);

    std::list<string> files;
    files.push_back(filename);

    return files;
}

void ImageComicFile::ReadFile(const char *, XByteArray &data) const
{
    XUtilities::ReadFile(this->FilePath().c_str(), data);
}
