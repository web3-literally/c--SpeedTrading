
#include "fileutil.h"

#include "common.h"
#include "constants.h"

#include <fstream>
#include <mutex>
#include <tchar.h>
#include <Windows.h>

using namespace std;

mutex g_db_mutex;
mutex g_log_mutex;

/**
* Save json string to db file
*/
void json2DBFile(wstring jsonstr)
{
	wstring path = getRunningDirectory();
	if (!path.empty()) {
		path = format2wstr(_T("%s%s"), path.c_str(), DB_FILE.c_str());
		wofstream outfile;

		{	// Critical section
			lock_guard<mutex> lock(g_db_mutex);
			outfile.open(path.c_str(), ios_base::out);
			outfile << jsonstr.c_str() << endl;
			outfile.close();
		}
	}
}

/**
* Read the first line from db file
*/
wstring dbFile2Json()
{
	wstring json_str;
	wstring path = getRunningDirectory();
	if (!path.empty()) {
		path = format2wstr(_T("%s%s"), path.c_str(), DB_FILE.c_str());
		wifstream infile;

		{	// Critical section
			lock_guard<mutex> lock(g_db_mutex);
			infile.open(path.c_str(), ios_base::in);
			while (getline(infile, json_str)) {
				if (!json_str.empty()) {
					break;
				}
			}
			infile.close();
		}
	}
	return json_str;
}

/**
* Get current directory name
*/
wstring getRunningDirectory()
{
	wstring dir_path(_T(""));
	TCHAR buf[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, buf, MAX_PATH)) {
		dir_path = wstring(buf);
		size_t npos = dir_path.find_last_of(_T("\\/"));
		dir_path = dir_path.substr(0, npos + 1);
	}
	return dir_path;
}

/**
* Get system directory
*/
wstring getSystem32Directory()
{
	TCHAR buf[MAX_LENGTH] = { 0 };
	GetSystemDirectory(buf, MAX_LENGTH);
	return wstring(buf);
}

/**
* Append log to file
*/
void log2File(wstring log_str)
{
	wstring path = getRunningDirectory();
	if (!path.empty()) {
		path = format2wstr(_T("%s%s.log"), path.c_str(), getCurrentDateTime(DATE_ONLY).c_str());
		wofstream outfile;

		{	// Critical section
			lock_guard<mutex> lock(g_log_mutex);
			outfile.open(path.c_str(), ios_base::app);
			outfile << log_str.c_str() << endl;
			outfile.close();
		}
	}
}

