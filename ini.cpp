#include "ini.h"

#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <set>
#include <algorithm>
#include <functional>

static std::mutex m_mutex;
static const int32_t MAX_BUFF_LEN = 2048;

struct ini_t
{
	std::string m_path;
	std::set<std::string> m_sections;
	std::map<std::string, std::map<std::string, std::string>> m_fields;
};

static std::string& trim_l(std::string& str)
{
	std::string::iterator it = find_if(str.begin(), str.end(), std::not1(std::ptr_fun(::isspace)));
	str.erase(str.begin(), it);
	return str;
}

static std::string& trim_r(std::string& str)
{
	std::string::reverse_iterator it = find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun(::isspace)));
	str.erase(it.base(), str.end());
	return str;
}

static std::string& trim(std::string& str)
{
	return trim_r(trim_l(str));
}

static int parse(ini_t* ini, FILE* file)
{
	char buff[MAX_BUFF_LEN] = { 0 };
	int32_t line_no = 0;
	std::string utf8bom;
	utf8bom.push_back(0xEF);
	utf8bom.push_back(0xBB);
	utf8bom.push_back(0xBF);
	std::map<std::string, std::string>* fields_map = NULL;
	while (fgets(buff, MAX_BUFF_LEN, file) != NULL)
	{
		line_no++;
		std::string line(buff);
		// UTF-8 BOM
		if (1 == line_no && line.find_first_of(utf8bom) == 0)
		{
			line.erase(0, 3);
		}

		for (size_t i = 0; i < line.length(); ++i)
		{
			if (';' == line[i] || '#' == line[i])
			{
				line.erase(i);
				break;
			}
		}

		trim(line);

		if (line.empty())
		{
			continue;
		}

		if (line[0] == '[' && line[line.length() - 1] == ']')
		{
			std::string section(line.substr(1, line.length() - 2));
			trim(section);
			if (section.empty())
			{
				return line_no;
			}
			ini->m_sections.insert(section);
			fields_map = &(ini->m_fields[section]);
			continue;
		}

		if (NULL == fields_map)
		{
			continue;
		}

		// fileds
		size_t pos = line.find('=');
		if (std::string::npos == pos)
		{
			continue;
		}
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		trim(key);
		trim(value);
		if (key.empty() || value.empty())
		{
			continue;
		}

		(*fields_map)[key] = value;
	}

	return 0;
}

ini_t* ini_load(const char* path)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	FILE* file = fopen(path, "ab+");
	if (NULL == file)
	{
		return nullptr;
	}
	ini_t* ini = new ini_t();
	ini->m_path = std::string(path);
	parse(ini, file);
	fclose(file);
	return ini;
}

int ini_get(ini_t* ini, const char* section, const char* key, void** value, void** default_value)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (ini == nullptr)
	{
		return -1;
	}

	std::map<std::string, std::map<std::string, std::string>>::iterator it = ini->m_fields.find(section);
	if (ini->m_fields.end() == it)
	{
		*value = *default_value;
		return -1;
	}

	std::map<std::string, std::string>& fields_map = it->second;
	std::map<std::string, std::string>::iterator cit = fields_map.find(key);
	if (fields_map.end() == cit)
	{
		*value = *default_value;
		return -1;
	}
	*value = (void*)(cit->second.c_str());
	return 0;
}

int ini_set(ini_t* ini, const char* section, const char* key, const char* value)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (ini == nullptr)
	{
		return -1;
	}
	std::map<std::string, std::string>* fields_map = nullptr;
	ini->m_sections.insert(section);
	fields_map = &(ini->m_fields[section]);
	(*fields_map)[key] = value;
	return 0;
}

int ini_write(ini_t* ini)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (ini == nullptr)
	{
		return -1;
	}

	FILE* file = fopen(ini->m_path.c_str(), "w+");

	if (NULL == file)
	{
		return -1;
	}

	std::map<std::string, std::map<std::string, std::string>>::iterator it = ini->m_fields.begin();
	while (ini->m_fields.end() != it)
	{
		fputs(("[" + it->first + "]").c_str(), file);
		fputs("\n", file);
		std::map<std::string, std::string>& fields_map = it->second;
		std::map<std::string, std::string>::iterator cit = fields_map.begin();
		while (fields_map.end() != cit)
		{
			fputs((cit->first + "=" + cit->second).c_str(), file);
			fputs("\n", file);
			cit++;
		}
		it++;
	}
	fclose(file);
	return 0;
}