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

#include <XUtilities.hpp>
#include "XZip.hpp"

using namespace ComicSeer;
using namespace std;

extern "C" FILE* Xfopen(const char* utf8Filename, const char* mode)
{
#if _WIN32
        std::wstring wfilename = XUtilities::ConvertUtf8ToWString(utf8Filename);
        return _wfopen(wfilename.c_str(), XUtilities::GetWStringFromString(mode).c_str());
#else
        return fopen(utf8Filename, mode);
#endif
}

extern "C" FILE* Xfopen64(const char* utf8Filename, const char* mode)
{
#if _WIN32
        std::wstring wfilename = XUtilities::ConvertUtf8ToWString(utf8Filename);
        return _wfopen(wfilename.c_str(), XUtilities::GetWStringFromString(mode).c_str());
#elif __APPLE__
        return fopen(utf8Filename, mode);
#else
        return fopen64(utf8Filename, mode);
#endif
}
