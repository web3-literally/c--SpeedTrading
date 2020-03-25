
#include "common.h"

#include "constants.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <locale>
#include <stdarg.h>
#include <time.h>

using namespace std;

/*
* Get current date or date_time. Should free memory after use returned value
*/
wstring getCurrentDateTime(DATETYPE type) 
{
	time_t now = time(0);
	struct tm  tstruct;
	TCHAR buf[80] = {0};
	tstruct = *localtime(&now);

	if (type == DATE_TIME)
		_tcsftime(buf, sizeof(buf), _T("%Y-%m-%d %X"), &tstruct);
	else if (type == DATE_ONLY)
		_tcsftime(buf, sizeof(buf), _T("%Y-%m-%d"), &tstruct);
	
	return wstring(buf);
	/*TCHAR *ret = (TCHAR *)calloc(_tcslen(buf) + 1, sizeof(TCHAR));
	_tcscpy(ret, buf);
	return ret;*/
};

/**
* Convert char to tchar. Should release memory after use return value
*/
TCHAR *char2tchar(const char *param) 
{
	size_t str_len_in_char = strlen(param) + 1;
	size_t str_len_in_tchar = str_len_in_char * (ceil((float)sizeof(TCHAR) / (float)sizeof(char)));
	TCHAR *ret = (TCHAR *)calloc(str_len_in_tchar, sizeof(TCHAR));
	_stprintf(ret, _T("%hs"), param);
	
	// Another way to use mbstowcs
	/*wstring wc(str_len_in_char, L'#');
	mbstowcs(&wc[0], param, str_len_in_char);*/
	
	return ret;
}

wstring char2wstring(const char *param) 
{
	size_t str_len_in_char = strlen(param) + 1;
	wstring wc(str_len_in_char, L'#');
	mbstowcs(&wc[0], param, str_len_in_char);
	return wc;
}

string wchar2string(const wchar_t *param)
{
	string str;
	size_t str_len_in_wchar = wcslen(param) + 1;
	size_t str_len_in_char = str_len_in_wchar * (ceil((float)sizeof(wchar_t) / (float)sizeof(char)));
	char *c_param = (char *)calloc(str_len_in_char, sizeof(char));
	size_t ret = wcstombs(c_param, param, str_len_in_char);
	if (ret == str_len_in_char) {
		c_param[str_len_in_char - 1] = '\0';
	}
	if (ret) {
		str = string(c_param);
	}
	
	free(c_param);

	return str;
}

/**
* Format wstring with argument list
*/
wstring format2wstr(const TCHAR *fmt, ...) 
{
	TCHAR buf[MAX_LENGTH] = { 0 };
	va_list args;
	va_start(args, fmt);
	_vstprintf(buf, fmt, args);
	va_end(args);
	return wstring(buf);
}

/**
* Format string with argument list
*/
string format2str(const char *fmt, ...) 
{
	char buf[MAX_LENGTH] = { 0 };
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	return string(buf);
}

/**
* Extract data part only from response
*/
string extractData(string response)
{
	string delimiter = "\r\n";
	size_t pos = 0;
	string token = "";
	
	pos = response.find_last_of(delimiter);
	if (pos != string::npos) {
		token = response.substr(pos + 1);
	}

	return token;
}

/**
* Find key in src (case-in-sensitive)
*/
bool findInsensitive(wstring data, wstring toSearch, size_t pos)
{
	// Convert complete given String to lower case
	transform(data.begin(), data.end(), data.begin(), tolower);
	// Convert complete given Sub String to lower case
	transform(toSearch.begin(), toSearch.end(), toSearch.begin(), tolower);
	// Find sub string in given string
	return data.find(toSearch, pos) != wstring::npos;
}
