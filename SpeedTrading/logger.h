#pragma once

#include <stdarg.h>
#include <Windows.h>

#include "constants.h"
#include "structs.h"

void log(LOGLEVEL level, const TCHAR *msg);

inline void log_error(const TCHAR *fmt, ...) {
	TCHAR buf[MAX_LENGTH] = { 0 };
	va_list args;
	va_start(args, fmt);
	_vstprintf(buf, fmt, args);
	va_end(args);
	log(FATAL, buf);
}

inline void log_warning(const TCHAR *fmt, ...) {
	TCHAR buf[MAX_LENGTH] = { 0 };
	va_list args;
	va_start(args, fmt);
	_vstprintf(buf, fmt, args);
	va_end(args);
	log(WARNING, buf);
}

inline void log_inform(const TCHAR *fmt, ...) {
	TCHAR buf[MAX_LENGTH] = { 0 };
	va_list args;
	va_start(args, fmt);
	_vstprintf(buf, fmt, args);
	va_end(args);
	log(INFORMATION, buf);
}

inline void printError(const TCHAR *msg) {
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; } while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Display the message
	TCHAR buf[MAX_LENGTH] = { 0 };
	_stprintf(buf, TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
	log(FATAL, buf);
}

