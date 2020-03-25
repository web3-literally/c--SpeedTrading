#pragma once

#include <string>

std::wstring getRunningDirectory();
std::wstring getSystem32Directory();

void json2DBFile(std::wstring jsonstr);
std::wstring dbFile2Json();

void log2File(std::wstring logstr);
