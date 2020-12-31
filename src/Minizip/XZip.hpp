/*
Copyright 2012-2014 Adam Jordan (xylamic@gmail.com)

This file is part of Comic Seer.

Comic Seer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

Comic Seer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Comic Seer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XZIP_HPP
#define XZIP_HPP

#include <Xylasoft.hpp>
#include <XByteArray.hpp>
#include <list>
#include <cstdint>
#include <cstdio>
#include "unzip.h"

namespace ComicSeer
{
	class XZip
	{
	public:
		/*!
		Construct the zip file.
		@param[in] path The path to the zip file.
		*/
		XZip(const char* path);

		/*!
		Destructor.
		*/
		~XZip();

		/*!
		Get the number of entries in the file.
		@returns The number of entries.
		*/
		uint64_t NumberOfEntries() const;

		/*!
		Get the list of file entries.
		@param[out] The list of file entries.
		*/
		void FileEntries(std::list<std::string>& entries) const;

		/*!
		Read the specified file from the zip.
		@param[in] filename The file name in the zip to read.
		@param[out] data The array to write to.
		*/
		void ReadFile(const char* filename, XByteArray& data) const;

	private:
		unzFile OpenFile() const;

	private:
		std::string _path;
	};
}

extern "C" FILE* Xfopen(const char* utf8Filename, const char* mode);

extern "C" FILE* Xfopen64(const char* utf8Filename, const char* mode);

#endif
