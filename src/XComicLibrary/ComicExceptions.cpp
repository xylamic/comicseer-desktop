/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "ComicExceptions.hpp"

using namespace ComicSeer;

FileFormatRarException::FileFormatRarException(const char* message, int code)
	: FileFormatException(message)
{
	this->_code = code;
}

int FileFormatRarException::Code() const
{
	return this->_code;
}
