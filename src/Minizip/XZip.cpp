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

#include "XZip.hpp"
#include <XException.hpp>
#include "unzip.h"
#include "FileFormatMinizipException.h"
#include <XUtilities.hpp>

using namespace ComicSeer;
using namespace std;

#define ZIP_FILENAME_SIZE 512
#define WRITEBUFFERSIZE 8192

XZip::XZip(const char* path)
{
    _path = path;
}

XZip::~XZip(void)
{
}

uint64_t XZip::NumberOfEntries() const
{
	unzFile uf = OpenFile();

	unz_global_info64 gi;

	int err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK)
	{
		unzClose(uf);
		throw FileFormatMinizipException("The file could not be read.", err);
	}

	unzClose(uf);

	return gi.number_entry;
}

void XZip::FileEntries(std::list<std::string>& entries) const
{
	unzFile uf = OpenFile();

	unz_global_info64 gi;

	int err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK)
	{
		unzClose(uf);
		throw FileFormatMinizipException("The file could not be read.", err);
	}

	for (uint64_t i = 0; i < gi.number_entry; i++)
	{
		char filename_inzip[ZIP_FILENAME_SIZE];
        unz_file_info64 file_info;

        err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, ZIP_FILENAME_SIZE, NULL, 0, NULL, 0);

		// in testing - 16 (0x10) was folder, 32 (0x20) was file
		if (err == UNZ_OK)
		{
			entries.push_back(filename_inzip);
		}

		if (i + 1 < gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK)
			{
				unzClose(uf);
				throw FileFormatMinizipException("Could not move to next file.", err);
			}
		}
	}

	unzClose(uf);
}

void  XZip::ReadFile(const char* filename, XByteArray& data) const
{
	unzFile uf = OpenFile();

	int err = UNZ_OK;
    if (unzLocateFile(uf, filename, 0) != UNZ_OK)
    {
        unzClose(uf);
		throw FileFormatMinizipException("The file could not be read.", err);
    }

    char filename_inzip[ZIP_FILENAME_SIZE];

    unz_file_info64 file_info;
    err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, ZIP_FILENAME_SIZE, NULL, 0, NULL, 0);

	if (err != UNZ_OK)
    {
        unzCloseCurrentFile(uf);
		unzClose(uf);
		throw FileFormatMinizipException("The file could not be read.", err);
    }

	data.Clear();
	data.Resize(static_cast<long>(file_info.uncompressed_size));

    err = unzOpenCurrentFilePassword(uf, /* password */ NULL);
	if (err != UNZ_OK)
    {
		unzCloseCurrentFile(uf);
		unzClose(uf);
		throw FileFormatMinizipException("The file could not be read.", err);
    }

	err = unzReadCurrentFile(uf, data.Data(), data.Size());
	if (err < 0)
    {
        unzCloseCurrentFile(uf);
		unzClose(uf);
		throw FileFormatMinizipException("The file could not be read.", err);
    }

    unzCloseCurrentFile(uf);
	unzClose(uf);
}

unzFile XZip::OpenFile() const
{
    unzFile uf;

#ifdef USEWIN32IOAPI
	zlib_filefunc64_def ffunc;
    fill_win32_filefunc64W(&ffunc);
    uf = unzOpen2_64(zipfilename,&ffunc);
#else
    uf = unzOpen64(_path.c_str());
#endif

	if (uf == NULL)
	{
		throw FileException("The file could not be loaded.");
	}

	return uf;
}
