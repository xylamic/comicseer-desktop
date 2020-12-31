/*
Copyright (c) 2012-2014 Adam Jordan (xylamic@gmail.com)
All Rights Reserved

This file is part of Comic Seer.
http://comicseer.xylasoft.com
*/

#include "XIni.hpp"
#include "XException.hpp"
#include "XUtilities.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace ComicSeer;
using namespace std;

XIni::XIni()
{
}

XIni::XIni(const char* filepath)
{
    std::fstream file;
    XUtilities::Openfstream(file, filepath, std::ios::in);

	// try to open the file
	if (!file.is_open())
	{
		throw IOException("The file could not be opened for read.");
	}

	// read the whole file
	std::string filetext;
	char c = file.get();
	while (!file.eof())
	{
		filetext.push_back(c);
		c = file.get();
	}

	// go ahead and close the file
	file.close();

	// iterate through the file and break it up
	size_t index = 0;
	std::string line;
	while (index < filetext.size())
	{
		// if we get a carriage return
		if (filetext[index] == '\r')
		{
			// if it is followed by a new line, skip over the new line as a single return
			if (filetext.size() < index + 1 && filetext[index + 1] != '\n')
			{
				index++;
			}

			this->m_content.push_back(line);
			line.clear();
		}
		// if we get a new line
		else if (filetext[index] == '\n')
		{
			// if it is followed by a carriage return, skip over the return as a single return
			if (filetext.size() < index + 1 && filetext[index + 1] != '\r')
			{
				index++;
			}

			this->m_content.push_back(boost::algorithm::trim_copy(line));
			line.clear();
		}
		// else this is a regular character and should be added
		else
		{
			line.push_back(filetext[index]);
		}

		// move to the next character
		index++;
	}

	// if we reached the eof the file with some characters left, write them
	if (line.size() > 0)
	{
		this->m_content.push_back(boost::algorithm::trim_copy(line));
	}
}

void XIni::Save(const char* filepath) const
{
    std::fstream file;
    XUtilities::Openfstream(file, filepath, std::ios::out | std::ios::trunc);

	if (!file.is_open())
    {
		throw IOException("The file could not be opened for write.");
	}

	for (std::list<std::string>::const_iterator it = this->m_content.cbegin();
		it != this->m_content.cend(); it++)
	{
		file.write(it->c_str(), it->size());
		file.write("\n", 1);
	}

	file.close();
}

std::list<std::string> XIni::GetKeysForSection(const char* section) const
{
	this->ValidateSectionName(section);

	bool inSection = false;
	std::string currentSection, key, value;
	std::list<std::string> keys;

	for (std::list<std::string>::const_iterator it = this->m_content.cbegin();
		it != this->m_content.cend(); it++)
	{
		if (this->IsSection(*it, currentSection))
		{
			if (currentSection.compare(section) == 0)
			{
				inSection = true;
			}
			else
			{
				inSection = false;
			}
		}
		else if (inSection && this->IsKey(*it, key, value))
		{
			keys.push_back(key);
		}
	}

	return keys;
}

bool XIni::GetValueForKeyCast(const char* section, const char* key, long& ivalue)
{
    string valuestr = this->GetValueForKey(section, key);
    if (valuestr.empty())
    {
        return false;
    }

    ivalue = atol(valuestr.c_str());

    return true;
}

bool XIni::GetValueForKeyCast(const char* section, const char* key, double& dvalue)
{
    string valuestr = this->GetValueForKey(section, key);
    if (valuestr.empty())
    {
        return false;
    }

    dvalue = atof(valuestr.c_str());

    return true;
}

std::string XIni::GetValueForKey(const char* section, const char* key) const
{
	this->ValidateSectionName(section);
	this->ValidateKeyValueName(key);

	bool inSection = false;
	std::string currentSection, currentKey, currentValue;

	for (std::list<std::string>::const_iterator it = this->m_content.cbegin();
		it != this->m_content.cend(); it++)
	{
		if (this->IsSection(*it, currentSection))
		{
			if (currentSection.compare(section) == 0)
			{
				inSection = true;
			}
			else
			{
				inSection = false;
			}
		}
		else if (inSection && this->IsKey(*it, currentKey, currentValue) && currentKey.compare(key) == 0)
		{
            this->ReplaceNewlineCharacters(currentValue);
			return currentValue;
		}
	}

	return "";
}

void XIni::SetKeyValueForSection(const char* section, const char* key, int value)
{
    this->SetKeyValueForSection(section, key, static_cast<long>(value));
}

void XIni::SetKeyValueForSection(const char* section, const char* key, long value)
{
    stringstream ss;
    ss << value;

    this->SetKeyValueForSection(section, key, ss.str().c_str());
}

void XIni::SetKeyValueForSection(const char* section, const char* key, double value)
{
    stringstream ss;
    ss << value;

    this->SetKeyValueForSection(section, key, ss.str().c_str());
}

void XIni::SetKeyValueForSection(const char* section, const char* key, const char* value)
{
	this->ValidateSectionName(section);
	this->ValidateKeyValueName(key);
	this->ValidateKeyValueName(value);

    string valuestr;
    if (value != NULL)
    {
        valuestr = value;
        this->CleanNewlineCharacters(valuestr);
    }
    value = valuestr.data();

	std::list<std::string>::iterator secit = this->m_content.end();
	std::string currentSection, currentKey, currentValue;

	for (std::list<std::string>::iterator it = this->m_content.begin();
		it != this->m_content.end(); it++)
	{
		if (this->IsSection(*it, currentSection))
		{
			// if the section found was the section we're looking for
			if (currentSection.compare(section) == 0)
			{
				secit = it;
			}
			else
			{
				// if we found a section and we previously found the section we were looking for,
				// then the key/value needs to be added to the previous section
				if (secit != this->m_content.end())
				{
					this->m_content.insert(++secit, this->CreateKeyValueLine(key, value));
					return;
				}
			}
		}
		// if this is the key we're looking for inside the section, set the value
		else if (secit != this->m_content.end() && this->IsKey(*it, currentKey, currentValue) && currentKey.compare(key) == 0)
		{
			*it = this->CreateKeyValueLine(key, value);
			return;
		}
	}

	// if we never saw the section, then add the section
	if (secit == this->m_content.end())
	{
		this->m_content.push_back(this->CreateSectionLine(section));
	}

	// whether the section exists prior or not, it now exists as the last section
	// in the file, add the key/value
	this->m_content.push_back(this->CreateKeyValueLine(key, value));
}

void XIni::RemoveAllKeys(const char* section)
{
	std::list<std::string> keys = this->GetKeysForSection(section);
	for (std::list<std::string>::iterator it = keys.begin(); it != keys.end(); it++)
	{
		this->RemoveKey(section, it->c_str());
	}
}

void XIni::RemoveKey(const char* section, const char* key)
{
	this->ValidateSectionName(section);
	this->ValidateKeyValueName(key);

	bool inSection = false;
	std::string currentSection, currentKey, currentValue;

	for (std::list<std::string>::iterator it = this->m_content.begin();
		it != this->m_content.end(); it++)
	{
		if (this->IsSection(*it, currentSection))
		{
			if (currentSection.compare(section) == 0)
			{
				inSection = true;
			}
			else
			{
				inSection = false;
			}
		}
		else if (inSection && this->IsKey(*it, currentKey, currentValue) && currentKey.compare(key) == 0)
		{
			this->m_content.erase(it);
			return;
		}
	}
}

void XIni::ValidateSectionName(const char* section) const
{
	std::string sectionname = section;
	if (sectionname.find('[') != sectionname.npos || sectionname.find(']') != sectionname.npos)
	{
		throw ArgumentException("The section cannot contain brackets.");
	}
}

void XIni::ValidateKeyValueName(const char* name) const
{
	std::string kvname = name;
	if (kvname.find('=') != kvname.npos)
	{
		throw ArgumentException("The name cannot contain equal sign.");
	}
}

std::string XIni::CreateSectionLine(const char* section) const
{
	std::string sectionline;
	sectionline.append("[").append(section).append("]");
	return sectionline;
}

std::string XIni::CreateKeyValueLine(const char* key, const char* value) const
{
	std::string keyline;
	keyline.append(key).append("=").append(value);
	return keyline;
}

bool XIni::IsKey(const std::string& line, std::string& key, std::string& value) const
{
	if (line.size() == 0 || line[0] == ';' || line[0] == '#' || line[0] == '[')
	{
		return false;
	}
			
	size_t index = line.find('=');
	if (index == line.npos)
	{
		return false;
	}

	key = boost::algorithm::trim_copy(line.substr(0, index));

	value = boost::algorithm::trim_copy(line.substr(index + 1));
	if (value.size() > 1 && value[0] == '\"' && value[value.size() - 1] == '\"')
	{
		value.erase(0, 1);
		value.erase(value.size() - 1, 1);
	}
			
	return true;
}

bool XIni::IsSection(const std::string& line, std::string& sectionname) const
{
	if (line.size() > 1 && line[0] == '[' && line[line.size() - 1] == ']')
	{
		sectionname = line.substr(1, line.size() - 2);
		return true;
	}
	else
	{
		return false;
	}
}

void XIni::CleanNewlineCharacters(std::string& s) const
{
    boost::algorithm::replace_all(s, "\n", "%%n%%");
    boost::algorithm::replace_all(s, "\r", "%%r%%");
}

void XIni::ReplaceNewlineCharacters(std::string& s) const
{
    boost::algorithm::replace_all(s, "%%r%%", "\r");
    boost::algorithm::replace_all(s, "%%n%%", "\n");
}
