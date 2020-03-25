#pragma once

#include <string>
#include <tchar.h>
#include <vector>

typedef enum {
	DATE_ONLY = 0,
	DATE_TIME
} DATETYPE;

typedef enum {
	FATAL = 0,
	WARNING,
	INFORMATION
} LOGLEVEL;

typedef enum {
	GET = 0,
	POST,
	PUT
} APITYPE;

typedef struct {
	std::wstring module_name;
	std::wstring module_path;
} MODULE_STRUCT;

typedef struct {
	std::wstring process_name;
	std::vector<MODULE_STRUCT> modules;
	std::wstring run_time;
} PROCESS_STRUCT;
