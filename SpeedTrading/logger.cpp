#include "logger.h"

#include "constants.h"
#include "common.h"
#include "fileutil.h"
#include "structs.h"

#include <fstream>

using namespace std;

/*
* Make log on several levels
*/
void log(LOGLEVEL level, const TCHAR *msg) {
	const TCHAR *level_str[3] = { _T("ERROR"), _T("WARNING"), _T("INFORMATION") };

	wstring cur_date_time = getCurrentDateTime(DATE_TIME);
	wstring formatted_log = format2wstr(_T("%s  (%s) ==> %s"), cur_date_time.c_str(), level_str[level], msg);

#ifdef LOG2FILE
	log2File(formatted_log);
#endif

#ifdef LOG2CONSOLE
	_tprintf(formatted_log.c_str());
#endif

}
