/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef FILEFORMATMINIZIPEXCEPTION_HPP
#define FILEFORMATMINIZIPEXCEPTION_HPP

#include <Xylasoft.hpp>
#include <XException.hpp>

namespace ComicSeer
{
	/*!
	Exception for specifying of minizip/zlib exceptions.
	*/
	class FileFormatMinizipException : FileFormatException
	{
	public:
		/*!
		Construct the exception.
		@param[in] message The exception message.
		@param[in] code The minizip/zlib exception.
		*/
		FileFormatMinizipException(const char* message, int code);

		/*!
		Get the minizip/zlib exception code.
		@returns The exception code.
		*/
		int Code() const;

	private:
		int _code;
	};
}

#endif
