/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef XUTILITIES_HPP
#define XUTILITIES_HPP

#include "Xylasoft.hpp"
#include "XByteArray.hpp"
#include <vector>
#include <fstream>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <cstdio>
#ifdef _WIN32
    #include <Windows.h>
#endif

namespace ComicSeer
{
	/*!
	This is a static class that provides general functionality.
	*/
	class XUtilities
	{
	public:
		/*!
		Compare two arrays of byte (or raw data) for equivelence.
		@param[in] data1 The first array of data.
		@param[in] data1Size The length of the first array.
		@param[in] data2 The second array of data.
		@param[in] data2Size The length of the second array.
		@returns True if the array are equal.
		*/
		static bool CompareByteArrays(const char* data1, long data1Size,
							   const char* data2, long data2Size);

		/*!
		Convert from a std::wstring to std::string.
		@param[in] ws The std::wstring to convert.
		@returns The std::string format.
		*/
		static std::string GetStringFromWString(const std::wstring& ws);

		/*!
		Convert from a std::string to std::wstring.
		@param[in] s The std::string to convert.
		@returns The std::wstring format.
		*/
        static std::wstring GetWStringFromString(const std::string& s);

		/*!
		Perform a case-insensitive compare of Strings.
		@param[in] s1 The first string.
		@param[in] s2 The second string.
		@returns 0 if strings are equal.
		*/
        static int AsciiCaseInsensitiveCompare(const std::string& s1, const std::string& s2);

        /*!
        Perform a case-insensitive compare of WStrings.
        @param[in] s1 The first string.
        @param[in] s2 The second string.
        @returns 0 if strings are equal.
       */
        static int WCharCaseInsensitiveCompare(const std::wstring& s1, const std::wstring& s2);

        /*!
        Perform a case-insensitive compare of UTF-8 c-strings.
        @param[in] s1 The first string.
        @param[in] s2 The second string.
        @returns 0 if strings are equal.
       */
        static int Utf8CaseInsensitiveCompare(const std::string& s1, const std::string& s2);

		/*!
		Remove a character from the given string.
		@param[in] ws The string to remove characters from.
		@param[in] c The character to remove.
		*/
		static void RemoveCharacterFromString(std::wstring& ws, wchar_t c);

		/*!
		Convert the given hexidecimal string to an array of bytes.
		@param[in] s The string to convert.
		@param[in] bytes The array to load the bytes into.
		*/
		static void ConvertHexStringToByte(std::string& s, std::vector<char>& bytes);

		/*!
		Generate a pseudo-random number.
		@returns The number.
		*/
		static int GenerateRandomNumber();

		/*!
		Read the entire specified file.
		@param[in] filename The path to the file to read.
		@param[out] data The data in the file.
		*/
		static void ReadFile(const char* filename, XByteArray& data);

		/*!
		Write the entire specified file.
		@param[in] filename The path to the file to write.
		@param[in] data The data to be written.
		*/
		static void WriteFile(const char* filename, XByteArray& data);

        /*!
        @brief Convert a UTF-8 string to wide character string.
        @param utf8 The UTF-8 string.
        @return The wide character string.
        */
        static std::wstring ConvertUtf8ToWString(const std::string& utf8string);

        /*!
        @brief Convert a wide character string to UTF-8 string.
        @param wstr The wide character string.
        @return The UTF-8 string.
        */
        static std::string ConvertWStringToUtf8(const std::wstring& wstr);

        /*!
        @brief Open the file stream operating-specific.
        @param file The file stream to open.
        @param filepath The file path.
        @param mode The file stream open mode.
        */
        static void Openfstream(std::fstream& file, const char* filepath, std::ios_base::openmode mode);

        /*!
        @brief Get the system-specific temporary folder ending with the appropriate slash.
        @return The path.
        throw EnvironmentVariableException When the environment does not have the appropriate variable.
        */
        static std::string GetTempFolderDir();

        /*!
        @brief Check whether a process is running.
        @param pid The process identifier.
        @return True if running, false otherwise.
        */
        static bool IsProcessRunning(int pid);

        /*!
        @brief Sleep the current thread the specified length of time.
        @param msSleep The time to sleep in milliseconds.
        */
        static void Sleep(int msSleep);

        /*!
        @brief Get the local operating system information.
        @param name The name of the system (ie: Windows/Linux)
        @param machine The machine type.
        @param release The release of the system.
        @param version The version of the system.
        */
        static void GetOperatingSystem(std::string& kernelRelease, std::string& version);

        /*!
        Get the file extension from the specified file name.
        @param[in] file The file name to parse.
        @returns The file extension.
       */
        static std::string GetFileExtension(const char* file);

        /*!
        File description structure.
        */
        typedef struct
        {
            /*! The name of the file. */
            std::string name;

            /*! The full path to the file. */
            std::string fullname;

            /*! Whether this is a file, otherwise it is a directory. */
            bool isFile;

            /*! The modification date/time in UTC. */
            time_t modTime;
        } XFileDesc;

        /*!
        @brief Get the list of files in a given directory.
        @param dirpath The path to the directory.
        @param entries The entries in the specified directory.
        */
        static void GetFilesInDirectory(const char* dirpath, std::vector<XFileDesc>& entries);

#ifdef _WIN32
        static time_t FiletimeToTimeT(const FILETIME& ft);
#endif

        /*!
        @brief Convert a UUID to string.
        @param uuid The UUID.
        @return The string form.
        */
        static std::string BoostUuidToString(const boost::uuids::uuid &uuid);

        /*!
        @brief Convert a string to UUID.
        @param str The string to convert.
        @return The UUID.
        */
        static boost::uuids::uuid StringToBoostUuid(const char *str);

        /*!
        @brief Get an empty UUID.
        @return Empty UUID.
        */
        static boost::uuids::uuid EmptyUuid();

        /*!
        @brief Generate a new UUID.
        @return New UUID.
        */
        static boost::uuids::uuid NewUuid();

        /*!
        @brief Delete the specified file.
        @param path The file to delete.
        @return True if successful.
        */
        static bool DelFile(const char *path);

		// make all these methods private, this is a static class
	private:
		XUtilities();
		XUtilities& operator=(const XUtilities&);
		XUtilities(const XUtilities& other);

	private:
		static const double m_doubleDiffValue;
    };
}

#endif // UTILITIES_H
