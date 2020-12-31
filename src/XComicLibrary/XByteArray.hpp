/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef XBYTEARRAY_HPP
#define XBYTEARRAY_HPP

#include "Xylasoft.hpp"

namespace ComicSeer
{
	/*!
	This provides the functionality of a byte buffer for dynamically altering a
	byte array.
	*/
	class XByteArray
	{
	public:
		/*!
		Constructs an empty byte array.
		*/
		XByteArray();

		/*!
		Create a byte array of the specified size, with the specified value.
		@param[in] size The size of the array.
		@param[in] value The value to insert into the array.
		*/
		XByteArray(long size, char value);

		/*!
		Creates a copy of the specified data to initialize the array.
		@param[in] data The binary data array.
		@param[in] dataSize The length of the data to copy.
		*/
		XByteArray(const char* data, long dataSize);

		/*!
		Appends the specified data to the end of the array.
		@param[in] data The binary array to copy in.
		*/
		void Append(const XByteArray& data);

		/*!
		Append the specified data to the end of the array.
		@param[in] data The binary dat array.
		@param[in] dataSize The length of the data to copy.
		*/
		void Append(const char* data, long dataSize);

		/*!
		Append a single byte to the end of the array.
		@param[in] c The byte to append.
		*/
		void Append(char c);

		/*!
		Clears the byte array back to length of zero.
		*/
		void Clear();

		/*!
		Resize the array to the specified size and inserts the value for any new positions.
		@param[in] size The size to resize to.
		*/
		void Resize(long size, char value = 0);

		/*!
		Get the internal array of const bytes so the data
		CANNOT be manipulated externally.
		@returns The const array.
		*/
		const char* ConstData() const;

		/*!
		Get the internal array of bytes so the data CAN
		be manipulated externally.
		@returns The array.
		*/
		char* Data();

		/*!
		Get the size of the array.
		@returns The size of the array.
		*/
		long Size() const;

		/*!
		Get the data at the specified index offset.
		@param[in] index The index offset.
		@returns The pointer at the specified offset.
		*/
		char* operator[](long index);

		/*!
		Get the data at the specified index offset.
		@param[in] index The index offset.
		@returns The pointer at the specified offset.
		*/
		const char* operator[](long index) const;

	private:
		std::string m_buffer;
	};
}

#endif
