/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "CbrComicFile.hpp"
#include <dll.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include "ComicUtilities.hpp"
#include <XUtilities.hpp>
#include <cstdio>
#include <string>
#include <cstring>

using namespace ComicSeer;
using namespace std;
using namespace boost;

int CALLBACK CallbackProc(UINT msg, LPARAM bufferPtr, LPARAM rarBuffer, LPARAM bytesProcessed)
{
	switch(msg)
	{
	case UCM_CHANGEVOLUME:
		return -1;
		break;
	case UCM_PROCESSDATA:
		memcpy(*(char**)bufferPtr, (char*)rarBuffer, bytesProcessed);
		*(char**)bufferPtr += bytesProcessed;
		return 1;
		break;
	case UCM_NEEDPASSWORD:
		return -1;
		break;
	}

	return 1;
}

CbrComicFile::CbrComicFile(const char* filepath)
	: ComicFile(filepath)
{
}

ComicFile::ComicFileType CbrComicFile::GetFileType() const
{
    return ComicFile::Rar;
}

std::list<std::string> CbrComicFile::GetFileList() const
{
#ifdef _WIN32
	// copy the name to a non-const string
    wchar_t name[4096];
    ::wcscpy(name, XUtilities::ConvertUtf8ToWString(this->FilePath()).c_str());

	// specify the archive
	RAROpenArchiveDataEx s;
	memset(&s, 0, sizeof(s));
    s.ArcNameW = name;
	s.OpenMode = RAR_OM_LIST;
#else
    // copy the name to a non-const string
    char name[4096];
    ::strcpy(name, this->FilePath().c_str());

    // specify the archive
    RAROpenArchiveDataEx s;
    memset(&s, 0, sizeof(s));
    s.ArcName = name;
    s.OpenMode = RAR_OM_LIST;
#endif

	// open the archive for listing
	HANDLE rarHandle = RAROpenArchiveEx(&s);

	// verify it successfull opened
	if (s.OpenResult != 0)
	{
		throw FileException("The archive could not be read.");
	}

	// read each file
	list<string> entries;
	RARHeaderDataEx d;
	memset(&d, 0, sizeof(RARHeaderDataEx));
	while (RARReadHeaderEx(rarHandle, &d) == 0)
	{
#ifdef _WIN32
		// if this is a file
        entries.push_back(XUtilities::ConvertWStringToUtf8(d.FileNameW));
#else
        entries.push_back(d.FileName);
#endif

		if (RARProcessFile(rarHandle, RAR_SKIP, NULL, NULL) != 0)
		{
			throw FileException("The archive is corrupted.");
		}
	}

	// close the archive
	RARCloseArchive(rarHandle);

	return entries;
}

void CbrComicFile::ReadFile(const char* name, XByteArray& data) const
{
#ifdef _WIN32
	// copy the name to a non-const string
    wchar_t filename[4096];
    ::wcscpy(filename, XUtilities::ConvertUtf8ToWString(this->FilePath()).c_str());

    // get the name as wstring
    wstring wname = XUtilities::ConvertUtf8ToWString(name);

	// specify the archive
	RAROpenArchiveDataEx s;
	memset(&s, 0, sizeof(s));
    s.ArcNameW = filename;
	s.OpenMode = RAR_OM_EXTRACT;
#else
    // copy the name to a non-const string
    char filename[4096];
    ::strcpy(filename, this->FilePath().c_str());

    // specify the archive
    RAROpenArchiveDataEx s;
    memset(&s, 0, sizeof(s));
    s.ArcName = filename;
    s.OpenMode = RAR_OM_EXTRACT;
#endif

	// open the archive for listing
	HANDLE rarHandle = RAROpenArchiveEx(&s);

	// verify it successfull opened
	if (s.OpenResult != 0)
	{
		throw FileException("The archive could not be read.");
	}

	// read each file
	RARHeaderDataEx d;
	memset(&d, 0, sizeof(RARHeaderDataEx));
	while (RARReadHeaderEx(rarHandle, &d) == 0)
	{
#ifdef _WIN32
        if (wcscmp(d.FileNameW, wname.c_str()) == 0)
#else
        if (strcmp(d.FileName, name) == 0)
#endif
		{
			// set the data size and callback
			data.Resize(d.UnpSize);
			char* bufferPtr = data.Data();
			RARSetCallback(rarHandle, CallbackProc, (long)&bufferPtr);

			if (0 != RARProcessFile(rarHandle, RAR_TEST, NULL, NULL))
			{
				throw FileException("The archive is corrupted.");
			}

			break;
		}
		else
		{
			if (RARProcessFile(rarHandle, RAR_SKIP, NULL, NULL) != 0)
			{
				throw FileException("The archive is corrupted.");
			}
		}
	}

	// close the archive
	RARCloseArchive(rarHandle);
}
