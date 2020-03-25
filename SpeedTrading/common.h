#pragma once

#include <string>
#include <tchar.h>

#include "structs.h"

TCHAR *char2tchar(const char *param);
std::wstring char2wstring(const char *param);
std::string wchar2string(const wchar_t *param);

std::wstring format2wstr(const TCHAR *fmt, ...);
std::string format2str(const char *fmt, ...);

std::wstring getCurrentDateTime(DATETYPE type);

std::string extractData(std::string response);
bool findInsensitive(std::wstring data, std::wstring toSearch, size_t pos = 0);

