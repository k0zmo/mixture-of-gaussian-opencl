#include "ConfigFile.h"

#include <fstream>

using std::string;
using std::unordered_map;

#pragma warning(disable:4503)

namespace detail 
{
	void trim(string* str, bool left = true, bool right = true)
	{
		if(left)
		{
			size_t pos = str->find_first_not_of(" \t\r");
			if(pos != 0 && pos != string::npos)
			{
				*str = str->substr(pos);
			}
		}

		if(right)
		{
			size_t pos = str->find_last_not_of(" \t\r");
			if(pos < str->length()-1)
			{
				*str = str->substr(0, pos+1);
			}
		}
	}
}

ConfigFile::ConfigFile()
{
}

ConfigFile::~ConfigFile()
{
	clear();
}

void ConfigFile::clear()
{
	for(auto i = mSettings.begin(), e = mSettings.end();  i != e; ++i)
		delete i->second;
	mSettings.clear();
}

bool ConfigFile::load(const string& filename, const string& separators)
{
	// sprobuj otworzyc plik do odczytu
	std::ifstream strm;
	strm.open(filename.c_str(), std::ios::in | std::ios::binary);
	if(!strm.is_open() || strm.fail())
		return false;

	string currSection;
	AttributeMap* currSettings = new AttributeMap();
	mSettings[currSection] = currSettings;

	string line, optName, optValue;
	while(!strm.eof())
	{
		getline(strm, line);
		detail::trim(&line);

		// jesli to nie linia z komentarzem ani nie pusta linia
		if(line.length() && line.at(0) != '#' && line.at(0) != ';')
		{
			// nowa sekcja
			if(line.at(0) == '[' && line.at(line.length()-1) == ']')
			{
				currSection = line.substr(1, line.length() - 2);
				auto seci = mSettings.find(currSection);
				// sekcja jeszcze nie istnieje
				if(seci == mSettings.end())
				{
					currSettings = new AttributeMap();
					mSettings[currSection] = currSettings;
				}
				else
				{
					currSettings = seci->second;
				}
			}
			else
			{
				// znajdz pozycje separatora
				string::size_type separator_pos = line.find_first_of(separators, 0);
				if(separator_pos != string::npos)
				{
					// oddziel klucz od wartosci
					optName = line.substr(0, separator_pos);
					// znajdz pozycje wartosci
					string::size_type nonseparator_pos = line.find_first_not_of(separators, separator_pos);
					// oddziel wartosc
					optValue = (nonseparator_pos == string::npos) ? "" : line.substr(nonseparator_pos);
					detail::trim(&optName), detail::trim(&optValue);
					currSettings->insert(AttributeMap::value_type(optName, optValue));
				}
			}
		}
	}

	strm.close();
	return true;
}

bool ConfigFile::save(const string& filename, const string& separator)
{
	// sprobuj otworzyc plik do zapisu
	std::ofstream strm;
	strm.open(filename.c_str(), std::ios::trunc);
	if(!strm.is_open() || strm.fail())
		return false;

	// iteruj po wszystkich sekcjach
	auto seci = mSettings.begin();
	while(seci != mSettings.end())
	{
		strm << "[" << seci->first << "]" << std::endl;

		// iteruj po wszystkich atrybutach
		auto i = seci->second->begin();
		while(i != seci->second->end())
		{
			strm << i->first << separator << i->second << std::endl;
			i++;
		}
		++seci;
	}

	strm.close();
	return true;
}

string ConfigFile::value(const string& key, const string& section)
{
	auto seci = mSettings.find(section);
	if(seci == mSettings.end())
	{
		return "";
	}
	else
	{
		auto i = seci->second->find(key);
		if(i == seci->second->end())
		{
			return "";
		}
		else
		{
			return i->second;
		}
	}
}

bool ConfigFile::exists(const string& key, const string& section)
{
	auto seci = mSettings.find(section);
	if(seci == mSettings.end())
	{
		return false;
	}
	else
	{
		auto i = seci->second->find(key);
		if(i == seci->second->end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}