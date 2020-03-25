
#include "netmanager.h"

#include "api.h"
#include "fileutil.h"

#include <string>

using namespace std;

void runNetTransfer()
{
	wstring json_str = dbFile2Json();
	if (!json_str.empty()) {
		do_post_request("process_list", json_str);
	}
}

