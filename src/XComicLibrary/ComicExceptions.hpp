/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef COMICEXCEPTIONS_HPP
#define COMICEXCEPTIONS_HPP

#include <Xylasoft.hpp>
#include <XException.hpp>

namespace ComicSeer
{
	/*!
	This exception represents that error that occurred with rar file access.
	*/
	class FileFormatRarException : public FileFormatException
	{
	public:
		/*!
		Construct the exception.
		@param[in] message The exception message.
		@param[in] code The winrar exception.
		*/
		FileFormatRarException(const char* message, int code);

		/*!
		Get the RAR exception code.
		@returns The RAR exception code.
		*/
		int Code() const;

	private:
		int _code;
	};

    /*!
    @brief This exception is used when a requested file format is requested, but unsupported.
    */
    class ComicFileFormatUnsupportedException : public FileFormatException
    {
    public:
        /*!
        Construct the exception.
        @param[in] message The exception message.
       */
        ComicFileFormatUnsupportedException(const char* message) : FileFormatException(message) { }
    };

	/*!
	This exceptions occurs when a library already exists.
	*/
	class ComicLibraryAlreadyExistsException : public DataException
	{
	public:
		/*!
		Construct the exception.
		@param[in] message The exception message.
		*/
		ComicLibraryAlreadyExistsException(const char* message) : DataException(message) { }
	};

    /*!
    This exceptions occurs when Ghostscript could not be loaded.
    */
    class GhostscriptLoadException : public DataException
    {
    public:
        /*!
        Construct the exception.
        @param[in] message The exception message.
       */
        GhostscriptLoadException(const char* message) : DataException(message) { }
    };

    /*!
    This exceptions occurs when Ghostscript could not be instantiated.
    */
    class GhostscriptInitException : public DataException
    {
    public:
        /*!
        Construct the exception.
        @param[in] message The exception message.
       */
        GhostscriptInitException(const char* message) : DataException(message) { }
    };

    /*!
    This exceptions occurs when Ghostscript could not read a file.
    */
    class GhostscriptReadException : public DataException
    {
    public:
        /*!
        Construct the exception.
        @param[in] message The exception message.
       */
        GhostscriptReadException(const char* message) : DataException(message) { }
    };
}

#endif
