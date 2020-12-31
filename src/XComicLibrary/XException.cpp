/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "XException.hpp"
#include "XUtilities.hpp"

using namespace ComicSeer;

XException::XException(const char* message)
	: std::exception()
{
	this->m_message = message;
}

XException::XException(const XException& other)
{
	this->m_message = other.m_message;
}

XException::~XException() throw()
{
}

XException& XException::operator=(const XException& other)
{
	this->m_message = other.m_message;
	return *this;
}

const char* XException::what() const throw()
{
	return this->m_message.c_str();
}

LicenseException::LicenseException(const char* message)
	: XException(message)
{ }

IOException::IOException(const char* message)
	: XException(message)
{ }

FileException::FileException(const char* message)
	: IOException(message)
{ }

FileReadException::FileReadException(const char* message)
    : FileException(message)
{ }

XmlFileReadException::XmlFileReadException(const char* message)
    : FileReadException(message)
{ }

FileWriteException::FileWriteException(const char* message)
    : FileException(message)
{ }

XmlFileWriteException::XmlFileWriteException(const char* message)
    : FileWriteException(message)
{ }

EncryptionException::EncryptionException(const char* message)
	: IOException(message)
{ }

TimeoutException::TimeoutException(const char* message)
	: IOException(message)
{ }

NetworkException::NetworkException(const char* message)
	: XException(message)
{ }

ResolutionException::ResolutionException(const char* message)
	: NetworkException(message)
{ }

BindException::BindException(const char* message)
	: NetworkException(message)
{ }

CommunicationException::CommunicationException(const char* message)
	: NetworkException(message)
{ }

AddressException::AddressException(const char* message)
	: NetworkException(message)
{ }

DataException::DataException(const char* message)
	: XException(message)
{ }

EncodingException::EncodingException(const char* message)
	: DataException(message)
{ }

ArgumentException::ArgumentException(const char* message)
	: DataException(message)
{ }

FileFormatException::FileFormatException(const char* message)
	: FileException(message)
{ }

EnvironmentException::EnvironmentException(const char *message)
    : XException(message)
{ }

EnvironmentVariableException::EnvironmentVariableException(const char *message)
    : EnvironmentException(message)
{ }
