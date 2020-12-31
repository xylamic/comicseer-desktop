/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicLibraryItem.hpp"
#include <XUtilities.hpp>

using namespace ComicSeer;

ComicLibraryItem::ComicLibraryItem(const char *path)
    : ComicFileName(path)
{
    this->_filepath = path;
    this->_fileext = XUtilities::GetFileExtension(path);
    this->_modDateTime = 0;
}

ComicLibraryItem::ComicLibraryItem(const XUtilities::XFileDesc &fileDesc)
    : ComicFileName(fileDesc.fullname.c_str())
{
    this->_filepath = fileDesc.fullname;
    this->_fileext = XUtilities::GetFileExtension(fileDesc.fullname.c_str());
    this->_modDateTime = fileDesc.modTime;
}
