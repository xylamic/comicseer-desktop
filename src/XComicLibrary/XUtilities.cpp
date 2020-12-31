/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "XUtilities.hpp"
#include "XException.hpp"
#include <locale>
#include <algorithm>
#include <wctype.h>
#include <sstream>
#include <locale>
#include <ctime>
#include <cwctype>
#include <fstream>
#include <boost/smart_ptr/scoped_array.hpp>
#include <utf8.h>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <signal.h>
    #include <dirent.h>
    #include <sys/utsname.h>
#endif

using namespace ComicSeer;
using namespace std;

bool XUtilities::CompareByteArrays(const char* data1, long data1Size,
                       const char* data2, long data2Size)
{
    if (data1Size != data2Size)
    {
        return false;
    }

    for (int i = 0; i < data1Size; i++)
    {
        if (data1[i] != data2[i])
        {
            return false;
        }
    }

    return true;
}

std::string XUtilities::GetStringFromWString(const std::wstring& ws)
{
	if (ws.empty())
	{
		return string();
	}

    size_t length = ws.size() * 2;
	boost::scoped_array<char> mbcstring(new char[length]);
	size_t actual_length = wcstombs(mbcstring.get(), ws.c_str(), length);
	if (length == actual_length)
	{
		mbcstring[length - 1] = '\0';
	}

	return mbcstring.get();
}

std::wstring XUtilities::GetWStringFromString(const std::string& s)
{
	if (s.empty())
	{
		return wstring();
	}

    size_t length = s.size() * 2;
	boost::scoped_array<wchar_t> wcstring(new wchar_t[length]);
	size_t actual_length = mbstowcs(wcstring.get(), s.c_str(), length);
	if (length == actual_length)
	{
		wcstring[length - 1] = L'\0';
	}

	return wcstring.get();
}

int XUtilities::WCharCaseInsensitiveCompare(const std::wstring& ws1, const std::wstring& ws2)
{
	std::wstring wsl1 = ws1;
	std::wstring wsl2 = ws2;

	std::transform(wsl1.begin(), wsl1.end(), wsl1.begin(), ::towlower);
	std::transform(wsl2.begin(), wsl2.end(), wsl2.begin(), ::towlower);

    return wsl1.compare(wsl2);
}

int XUtilities::Utf8CaseInsensitiveCompare(const string &s1, const string &s2)
{
#ifdef _WIN32
    wstring ws1 = XUtilities::ConvertUtf8ToWString(s1);
    wstring ws2 = XUtilities::ConvertUtf8ToWString(s2);

    return ::_wcsicmp(ws1.c_str(), ws2.c_str());
#else
    return ::strcasecmp(s1.c_str(), s2.c_str());
#endif
}

int XUtilities::AsciiCaseInsensitiveCompare(const std::string& s1, const std::string& s2)
{
	std::string sl1 = s1;
	std::string sl2 = s2;

	std::transform(sl1.begin(), sl1.end(), sl1.begin(), ::tolower);
	std::transform(sl2.begin(), sl2.end(), sl2.begin(), ::tolower);

	return sl1.compare(sl2);
}

void XUtilities::RemoveCharacterFromString(std::wstring& ws, wchar_t c)
{
	size_t index = 0;
	while (index < ws.size())
	{
		if (ws[index] == c)
		{
			ws.erase(index, 1);
		}
		else
		{
			index++;
		}
	}
}

void XUtilities::ConvertHexStringToByte(std::string& s, std::vector<char>& bytes)
{
	long length = static_cast<long>(s.length());

	// make sure the input string has an even digit numbers
	if(length%2 == 1)
	{
		s = "0" + s;
		length++;
	}

	// allocate memory for the output array
	bytes.reserve(length/2);
	
	std::stringstream ss(s);
	for(int i=0; i < length/2; i++)
	{
		char ch1, ch2;
		ss >> ch1 >> ch2;

        int dig1 = 0, dig2 = 0;
		if(isdigit(ch1)) dig1 = ch1 - '0';
		else if(ch1>='A' && ch1<='F') dig1 = ch1 - 'A' + 10;
		else if(ch1>='a' && ch1<='f') dig1 = ch1 - 'a' + 10;
		if(isdigit(ch2)) dig2 = ch2 - '0';
		else if(ch2>='A' && ch2<='F') dig2 = ch2 - 'A' + 10;
		else if(ch2>='a' && ch2<='f') dig2 = ch2 - 'a' + 10;

		bytes.push_back(dig1*16 + dig2);
	}
}

int XUtilities::GenerateRandomNumber()
{
    /**** THIS NEEDS TO BE REPLACED WITH A HIGH PERFORMANCE RANDOM NUMBER GENERATOR *****/
    ::srand(static_cast<unsigned int>(::time(NULL)));
    return ::rand();
}

void XUtilities::ReadFile(const char* filename, XByteArray& data)
{
	// clear any existing data
	data.Clear();

	// open the file
	ifstream ofile(filename, ios_base::binary | ios_base::in);
	if (!ofile.is_open())
	{
		throw FileException("The file could not be opened.");
	}

	// get length of file:
	ofile.seekg (0, ios::end);
	data.Resize(static_cast<long>(ofile.tellg()));
	ofile.seekg (0, ios::beg);

	ofile.read(data.Data(), data.Size());
	ofile.close();
}

void XUtilities::WriteFile(const char* filename, XByteArray& data)
{
	// open the file
	ofstream ofile(filename, ios_base::binary | ios_base::out | ios_base::trunc);
	if (!ofile.is_open())
	{
		throw FileException("The file could not be opened.");
	}

	// write the content
	ofile.write(data.ConstData(), data.Size());

	// close the file
	ofile.flush();
    ofile.close();
}

wstring XUtilities::ConvertUtf8ToWString(const string &utf8string)
{
    wstring wcharstr;
    if (sizeof(wchar_t) == 2)
    {
        utf8::utf8to16(utf8string.begin(), utf8string.end(), back_inserter(wcharstr));
    }
    else
    {
        utf8::utf8to32(utf8string.begin(), utf8string.end(), back_inserter(wcharstr));
    }
    return wcharstr;
}

string XUtilities::ConvertWStringToUtf8(const wstring &wstr)
{
    string charstr;
    if (sizeof(wchar_t) == 2)
    {
        utf8::utf16to8(wstr.begin(), wstr.end(), back_inserter(charstr));
    }
    else
    {
        utf8::utf32to8(wstr.begin(), wstr.end(), back_inserter(charstr));
    }
    return charstr;
}

void XUtilities::Openfstream(fstream &file, const char *filepath, std::ios_base::openmode mode)
{
#ifdef _WIN32
    wstring wfilepath = XUtilities::ConvertUtf8ToWString(filepath);
    file.open(wfilepath, mode);
#else
    file.open(filepath, mode);
#endif
}

string XUtilities::GetTempFolderDir()
{
    char *tempDir = ::getenv("TEMP");
    if (tempDir == NULL || ::strlen(tempDir) == 0)
    {
        throw EnvironmentVariableException("The temp directory variable was not found.");
    }

    string temp = tempDir;

    if (temp[temp.size() - 1] != '/' || temp[temp.size() - 1] != '\\')
    {
#ifdef _WIN32
        temp.push_back('\\');
#else
        temp.push_back('/');
#endif
    }

    return temp;
}

bool XUtilities::IsProcessRunning(int pid)
{
    if (pid <= 0)
    {
        return false;
    }

    #ifdef _WIN32
        HANDLE handle = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
        if (!handle)
        {
            return false;
        }
        else
        {
            DWORD code;
            ::GetExitCodeProcess(handle, &code);
            if (code == STILL_ACTIVE)
            {
                CloseHandle(handle);
                return true;
            }
            else
            {
                CloseHandle(handle);
                return false;
            }
        }
    #else
        if (kill(pid, 0) == -1) {
            return false;
        } else {
            return true;
        }
#endif
}

void XUtilities::Sleep(int msSleep)
{
#ifdef _WIN32
    ::Sleep(msSleep);
#else
    ::usleep(msSleep * 1000);
#endif
}

void XUtilities::GetOperatingSystem(string &kernelRelease, string &versionName)
{
#ifdef _WIN32
    OSVERSIONINFOEX osvi;
    ::memset(&osvi, 0, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);

    GetVersionEx((LPOSVERSIONINFO)&osvi);

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
    {
        versionName = "Windows 2000";
    }
    else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
    {
        versionName = "Windows XP";
    }
    else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
    {
        versionName = "Windows Server 2003";
    }
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION)
    {
        versionName = "Windows Vista";
    }
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
    {
        versionName = "Windows Server 2008";
    }
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && osvi.wProductType == VER_NT_WORKSTATION)
    {
        versionName = "Windows 7";
    }
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
    {
        versionName = "Windows Server 2008 R2";
    }
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && osvi.wProductType == VER_NT_WORKSTATION)
    {
        versionName = "Windows 8";
    }
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
    {
        versionName = "Windows Server 2012";
    }
    else
    {
        versionName = "Windows Unknown";
    }

    stringstream ss;
    ss << osvi.dwMajorVersion << "." << osvi.dwMinorVersion;
    kernelRelease = ss.str();
#else
    struct utsname uname;

    if (::uname(&uname) == 0)
    {
        kernelRelease = uname.release;
        versionName = uname.version;
    }
#endif
}

std::string XUtilities::GetFileExtension(const char* file)
{
    string sfile = file;

    size_t extIndex = sfile.find_last_of('.');

    if (extIndex == sfile.npos || extIndex + 1 >= sfile.size())
    {
        return string();
    }
    else
    {
        string ext = sfile.substr(extIndex + 1);
        boost::algorithm::to_lower(ext);
        return ext;
    }
}

void XUtilities::GetFilesInDirectory(const char *dirpath, std::vector<XFileDesc> &entries)
{
    string path = dirpath;
    if (path.size() > 0 && path[path.size() - 1] != '/' && path[path.size() - 1] != '\\')
    {
#ifdef _WIN32
        path.push_back('\\');
#else
        path.push_back('/');
#endif
    }

#ifdef _WIN32
    WIN32_FIND_DATAW ffd;

    string searchpath = path;
    searchpath.push_back('*');

    wstring wpath = ConvertUtf8ToWString(searchpath);
    HANDLE hFind = FindFirstFileW(wpath.c_str(), &ffd);

    wchar_t buffer[32768];

    if (INVALID_HANDLE_VALUE == hFind)
    {
        throw FileException("The directory was not found.");
    }

    // List all the files in the directory with some info about them.
    do
    {
        // create the file descriptor
        XFileDesc desc;
        desc.name = ConvertWStringToUtf8(ffd.cFileName);
        desc.isFile = !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        desc.fullname = path + desc.name;
        desc.modTime = FiletimeToTimeT(ffd.ftLastWriteTime);

        // verify it is not a virtual dot/dotdot directory
        if (desc.name.compare(".") == 0 || desc.name.compare("..") == 0)
            continue;

        // get the full path
        if (GetFullPathNameW(ConvertUtf8ToWString(path + desc.name).c_str(), 32768, buffer, NULL) > 0)
            desc.fullname = ConvertWStringToUtf8(buffer);

        // add the entry
        entries.push_back(desc);
    }
    while (FindNextFileW(hFind, &ffd) != 0);

    DWORD dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        FindClose(hFind);
        throw FileException("The directory could not be read.");
    }

    FindClose(hFind);
#else
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(path.c_str())) == NULL) {
        throw FileException("The directory could not be opened.");
    }

    char realpathbuffer[PATH_MAX];
    struct stat s;

    while ((dirp = readdir(dp)) != NULL) {
        if (!(dirp->d_type & DT_REG) && !(dirp->d_type & DT_DIR))
            continue;

        XFileDesc desc;
        desc.name = dirp->d_name;
        desc.isFile = dirp->d_type & DT_REG;

        // evaluate the full path
        if (desc.isFile)
        {
            realpath((path + desc.name).c_str(), realpathbuffer);
            desc.fullname = realpathbuffer;
        }
        else
        {
            desc.fullname = path + desc.name;
        }

        // get the modification information
        if (stat(desc.fullname.c_str(), &s) == 0)
        {
#ifdef unix
            desc.modTime = s.st_mtim.tv_sec;
#else
            desc.modTime = s.st_mtimespec.tv_sec;
#endif
        }

        // verify it is not a virtual dot/dotdot directory
        if (!desc.isFile && (desc.name.compare(".") == 0 || desc.name.compare("..") == 0))
            continue;

        entries.push_back(desc);
    }
    closedir(dp);
#endif
}

#ifdef _WIN32
time_t XUtilities::FiletimeToTimeT(const FILETIME &ft)
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;

    return ull.QuadPart / 10000000ULL - 11644473600ULL;
}
#endif

string XUtilities::BoostUuidToString(const boost::uuids::uuid &uuid)
{
    return boost::uuids::to_string(uuid);
}

boost::uuids::uuid XUtilities::StringToBoostUuid(const char *str)
{
    return boost::lexical_cast<boost::uuids::uuid>(string(str));
}

boost::uuids::uuid XUtilities::EmptyUuid()
{
    boost::uuids::uuid u = { 0 };
    return u;
}

boost::uuids::uuid XUtilities::NewUuid()
{
    return boost::uuids::random_generator()();
}

bool XUtilities::DelFile(const char *path)
{
    if (remove(path) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
