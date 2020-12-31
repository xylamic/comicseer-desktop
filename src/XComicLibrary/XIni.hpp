/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#ifndef XINI_HPP
#define XINI_HPP

#include "Xylasoft.hpp"
#include <list>

namespace ComicSeer
{
	/*!
	This provides the facilities for reading and writing an INI file. This supports
	single values with an associated key, and comments on their own lines starting 
	with ; or #. Currently, comments cannot be on the same line as a section name 
	or key/value pair.
	*/
	class XIni
	{
	public:
		/*!
		Construct an empty file.
		*/
		XIni();

		/*!
		Construct an object from the specified file.
		*/
		XIni(const char* filepath);

		/*!
		Save the contents of the file.
		@param[in] filepath The path to save the file.
		*/
		void Save(const char* filepath) const;

		/*!
		Get the list of keys in the specified section.
		@param[in] section The section to look up.
		@returns The list of keys in the section.
		*/
		std::list<std::string> GetKeysForSection(const char* section) const;

		/*!
		Get the value for the specified key.
		@param[in] section The section to lookup.
		@param[in] key The key to lookup.
		@returns The value found or an empty string if not found.
		*/
		std::string GetValueForKey(const char* section, const char* key) const;

        /*!
        Get the value for the specified key.
        @param[in] section The section to lookup.
        @param[in] key The key to lookup.
        @param[in,out] ivalue The references value to write to.
        @returns True if read, false is not present or invalid.
       */
        bool GetValueForKeyCast(const char* section, const char* key, long& ivalue);

        /*!
        Get the value for the specified key.
        @param[in] section The section to lookup.
        @param[in] key The key to lookup.
        @param[in,out] dvalue The references value to write to.
        @returns True if read, false is not present or invalid.
       */
        bool GetValueForKeyCast(const char* section, const char* key, double& dvalue);

		/*!
		Set the value for the specified section and key.
		@param[in] section The section of the value.
		@param[in] key The key of the value.
		@param[in] value The value to write.
		*/
		void SetKeyValueForSection(const char* section, const char* key, const char* value);

        /*!
        Set the value for the specified section and key.
        @param[in] section The section of the value.
        @param[in] key The key of the value.
        @param[in] value The value to write.
       */
        void SetKeyValueForSection(const char* section, const char* key, int value);

        /*!
        Set the value for the specified section and key.
        @param[in] section The section of the value.
        @param[in] key The key of the value.
        @param[in] value The value to write.
       */
        void SetKeyValueForSection(const char* section, const char* key, long value);

        /*!
        Set the value for the specified section and key.
        @param[in] section The section of the value.
        @param[in] key The key of the value.
        @param[in] value The value to write.
       */
        void SetKeyValueForSection(const char* section, const char* key, double value);

		/*!
		Remove the specified key.
		@param[in] section The section of the key.
		@param[in] key The key to remove in the section.
		*/
		void RemoveKey(const char* section, const char* key);

		/*!
		Remove all keys from the specified section.
		@param[in] section The section to clear.
		*/
		void RemoveAllKeys(const char* section);

	protected:
		/*!
		Verify that the given section name is valid.
		@param[in] section The section name.
		*/
		void ValidateSectionName(const char* section) const;

		/*!
		Verify that the given key/value name is valid.
		@param[in] name The key/value name.
		*/
		void ValidateKeyValueName(const char* name) const;

		/*!
		Create the line for a section.
		@param[in] section The section name.
		@returns The section line.
		*/
		std::string CreateSectionLine(const char* section) const;

		/*!
		Create the line for a key/value pair.
		@param[in] key The key.
		@param[in] value The value.
		@returns The key/value line.
		*/
		std::string CreateKeyValueLine(const char* key, const char* value) const;

		/*!
		Get whether the given line contains a key/value pair.
		@param[in] line The line to parse.
		@param[out] key The key that was parsed.
		@param[out] value The value that was parsed.
		@returns True if this was a key/value line, false otherwise.
		*/
		bool IsKey(const std::string& line, std::string& key, std::string& value) const;

		/*!
		Get whether the given line contains a section pair.
		@param[in] line The line to parse.
		@param[out] section The section that was parsed.
		@returns True if this was a section line, false otherwise.
		*/
		bool IsSection(const std::string& line, std::string& sectionname) const;

        /*!
        @brief Clean newline characters and do substitutions.
        @param s The string to clean.
        */
        void CleanNewlineCharacters(std::string& s) const;

        /*!
        @brief Replace newline characters from substitutions.
        @param s The string to replace.
        */
        void ReplaceNewlineCharacters(std::string& s) const;

private:
		std::list<std::string> m_content;
	};
}

#endif
