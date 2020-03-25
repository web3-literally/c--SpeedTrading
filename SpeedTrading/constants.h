#pragma once

#define MAX_LENGTH		4096

#define LOG2FILE		
#define LOG2CONSOLE

#include <string>

//const std::string API_URL[] = _T("http://localhost/realtrade/api/");
const std::string API_URL = "http://192.168.2.24:8880/api/";

const std::wstring DB_FILE = L"heap.db";

// Process capture interval
const int CAPTURE_INTERVAL = 10;

// Network communication interval
const int NET_INTERVAL = 10;
