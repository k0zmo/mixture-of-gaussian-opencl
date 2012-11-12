#pragma once

#include <string>
#include <unordered_map>

class ConfigFile
{
public:
	ConfigFile();
	~ConfigFile();

	// Otwiera i parsuje plik
	bool load(const std::string& filename, const std::string& separators = "\t=:");
	// Zapisuje przechowywane dane do pliku
	bool save(const std::string& filename, const std::string& separator = "=");
	// Zwalnia zaalokowane zasoby
	void clear();

	// Pobiera wartosc dla podanego klucza i sekcji, w ktorej sie znajduje
	std::string value(const std::string& key, const std::string& section);
	// Sprawdza czy podany klucz w danej sekcji istnieje
	bool exists(const std::string& key, const std::string& section);

private:
	// Mapa sekcja - mapa[kluczy - wartosci]
	typedef std::unordered_map<std::string, std::string> AttributeMap; 
	typedef std::unordered_map<std::string, AttributeMap*> Settings;

	Settings mSettings;

private:
	ConfigFile(const ConfigFile& other);
	ConfigFile& operator=(const ConfigFile& other);
};
