/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef XEXCEPTION_HPP
#define XEXCEPTION_HPP

#include "Xylasoft.hpp"
#include <exception>

namespace ComicSeer
{
    /*! This base exception for use in the software. */
	class XException : public std::exception
	{
	public:
		/*!
		Construct a base exception.
		@param[in] message The text of the message.
		*/
		XException(const char* message);

		/*!
		Copy a base exception.
		@param[in] other The other to copy.
		*/
		XException(const XException& other);

        /*!
        Virtual destructor.
       */
        virtual ~XException() throw();

		/*!
		Copy a base exception.
		@param[in] other The other to copy.
		@returns This instance.
		*/
		XException& operator=(const XException& other);

		/*!
		Get the text of the exception.
		@returns The char string.
		*/
		const char* what() const throw();

	private:
		std::string m_message;
	};

	/*! A licensing exception. */
	class LicenseException : public XException
	{
	public:
		LicenseException(const char* message);
	};

	/*! An IO exception. */
	class IOException : public XException
	{
	public:
		IOException(const char* message);
	};

	/*! A file exception */
	class FileException : public IOException
	{
	public:
		FileException(const char* message);
	};

    /*! A file format exception */
    class FileFormatException : public FileException
    {
    public:
		FileFormatException(const char* message);
    };

    /*! A file format exception */
    class FileReadException : public FileException
    {
    public:
		FileReadException(const char* message);
    };

    /*! A file format exception */
    class XmlFileReadException : public FileReadException
    {
    public:
        XmlFileReadException(const char* message);
    };

    /*! A file format exception */
    class FileWriteException : public FileException
    {
    public:
        FileWriteException(const char* message);
    };

    /*! A file format exception */
    class XmlFileWriteException : public FileWriteException
    {
    public:
        XmlFileWriteException(const char* message);
    };

	/*! A encryption exception */
	class EncryptionException : public IOException
	{
	public:
		EncryptionException(const char* message);
	};

	/*! A timeout exception */
	class TimeoutException : public IOException
	{
	public:
		TimeoutException(const char* message);
	};

	/*! Networking exception. Problem with external system. */
	class NetworkException : public XException
	{
	public:
		NetworkException(const char* message);
	};

	/*! A network resolution exception. */
	class ResolutionException : public NetworkException
	{
	public:
		ResolutionException(const char* message);
	};

	/*! A network bind exception. */
	class BindException : public NetworkException
	{
	public:
		BindException(const char* message);
	};

	/*! A network communication exception. */
	class CommunicationException : public NetworkException
	{
	public:
		CommunicationException(const char* message);
	};

	/*! A network address parse exception. */
	class AddressException : public NetworkException
	{
	public:
		AddressException(const char* message);
	};

	/*! An error that occurred because a data problem. */
	class DataException : public XException
	{
	public:
		DataException(const char* message);
	};

	/*! A data encoding exception. */
	class EncodingException : public DataException
	{
	public:
		EncodingException(const char* message);
	};

	/*! A exception caused by an invalid argument. */
	class ArgumentException : public DataException
	{
	public:
		ArgumentException(const char* message);
	};

    /*! A exception caused by an environment problem. */
    class EnvironmentException : public XException
    {
    public:
        EnvironmentException(const char* message);
    };

    /*! A exception caused by a missing/invalid environment variable. */
    class EnvironmentVariableException : public EnvironmentException
    {
    public:
        EnvironmentVariableException(const char* message);
    };
}

#endif // XEXCEPTION_H
