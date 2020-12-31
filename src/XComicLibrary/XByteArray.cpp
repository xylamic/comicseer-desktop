/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "XByteArray.hpp"
#include "XUtilities.hpp"
#include <cstring>
#include <sstream>

using namespace ComicSeer;

XByteArray::XByteArray()
{
}

XByteArray::XByteArray(long size, char value)
{
	this->m_buffer.resize(size);
	std::memset(&this->m_buffer[0], value, size);
}

XByteArray::XByteArray(const char* data, long dataSize)
{
	m_buffer.resize(dataSize);

	if (dataSize > 0)
	{
		std::memcpy(&this->m_buffer[0], data, dataSize);
	}
}

void XByteArray::Append(const XByteArray& data)
{
	this->Append(data.ConstData(), data.Size());
}

void XByteArray::Append(const char* data, long dataSize)
{
	if (dataSize > 0)
	{
		long currentSize = static_cast<long>(this->m_buffer.size());

		this->m_buffer.resize(currentSize + dataSize);
		std::memcpy(&this->m_buffer[currentSize], data, dataSize);
	}
}

void XByteArray::Append(char c)
{
	this->m_buffer.push_back(c);
}

void XByteArray::Clear()
{
	this->m_buffer.clear();
}

void XByteArray::Resize(long size, char value)
{
	long initialSize = this->Size();
	this->m_buffer.resize(size);

	if (this->Size() > initialSize)
	{
		std::memset(&this->m_buffer[initialSize], value, this->Size() - initialSize);
	}
}

const char* XByteArray::ConstData() const
{
	return &this->m_buffer[0];
}

char* XByteArray::Data()
{
	return &this->m_buffer[0];
}

long XByteArray::Size() const
{
	return static_cast<long>(this->m_buffer.size());
}

char* XByteArray::operator[](long index)
{
	return &this->m_buffer[index];
}

const char* XByteArray::operator[](long index) const
{
	return &this->m_buffer[index];
}
