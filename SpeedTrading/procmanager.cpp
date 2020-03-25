
#include "procmanager.h"

#include "constants.h"
#include "fileutil.h"
#include "procutil.h"
#include "structs.h"

#include <document.h>
#include <string>
#include <stringbuffer.h>
#include <vector>
#include <Windows.h>
#include <writer.h>

using namespace rapidjson;
using namespace std;

vector<PROCESS_STRUCT> _p_processes;

/**
* Parse json string into process structure vector
*/
vector<PROCESS_STRUCT> json2Vector(wstring json_str)
{
	vector<PROCESS_STRUCT> processes;
	GenericDocument<UTF16<>> document;
	if( document.Parse(json_str.c_str()).HasParseError() || !document.HasMember(L"pl") ) {
		return processes;
	}

	GenericValue< UTF16<> >& process_list = document[L"pl"];
	if( process_list.IsArray() ) {
		for( SizeType i = 0; i < process_list.Size(); i++ ) {
			GenericValue< UTF16<> >& process_node = process_list[i];
			PROCESS_STRUCT process;
			process.process_name = process_node[L"pn"].GetString();
			process.run_time = process_node[L"rt"].GetString();
			GenericValue< UTF16<> >& module_list = process_node[L"ml"];
			if( module_list.IsArray() ) {
				for (SizeType ii = 0; ii < module_list.Size(); ii++) {
					GenericValue< UTF16<> >& module_node = module_list[ii];
					MODULE_STRUCT module_info;
					module_info.module_name = module_node[L"mn"].GetString();
					module_info.module_path = module_node[L"mp"].GetString();
					process.modules.push_back(module_info);
				}
			}
			processes.push_back(process);
		}
	}
	return processes;
}

/**
* Get json formatted string from process structure vector
*/
wstring vector2Json(vector<PROCESS_STRUCT> processes)
{
	GenericDocument<UTF16<>> document;
	document.SetObject();
	GenericValue<UTF16<>> jsonArray(kArrayType);
	Document::AllocatorType& allocator = document.GetAllocator();

	vector<PROCESS_STRUCT>::iterator it;
	for( it = processes.begin(); it != processes.end(); it++ ) {
		PROCESS_STRUCT proc_info = *it;

		GenericValue<UTF16<>> jsonObject(kObjectType);

		// Process name
		GenericValue<UTF16<>> key_name(kStringType);
		key_name.SetString(TEXT("pn"), allocator);
		GenericValue<UTF16<>> value_name(kStringType);
		value_name.SetString(proc_info.process_name.c_str(), allocator);
		jsonObject.AddMember(key_name, value_name, allocator);

		// Run time
		GenericValue<UTF16<>> key_time(kStringType);
		key_time.SetString(TEXT("rt"), allocator);
		GenericValue<UTF16<>> value_time(kStringType);
		value_time.SetString(proc_info.run_time.c_str(), allocator);
		jsonObject.AddMember(key_time, value_time, allocator);

		// List the modules and threads associated with this process
		GenericValue<UTF16<>> key_list(kStringType);
		key_list.SetString(TEXT("ml"), allocator);
		GenericValue<UTF16<>> jsonModuleArray(kArrayType);

		vector<MODULE_STRUCT>::iterator itt;
		for (itt = proc_info.modules.begin(); itt != proc_info.modules.end(); itt++) {
			MODULE_STRUCT module_info = *itt;

			// Module name
			GenericValue<UTF16<>> jsonModuleObject(kObjectType);
			GenericValue<UTF16<>> key_modulename(kStringType);
			key_modulename.SetString(TEXT("mn"), allocator);
			GenericValue<UTF16<>> value_modulename(kStringType);
			value_modulename.SetString(module_info.module_name.c_str(), allocator);
			jsonModuleObject.AddMember(key_modulename, value_modulename, allocator);

			// Module path
			GenericValue<UTF16<>> key_modulepath(kStringType);
			key_modulepath.SetString(TEXT("mp"), allocator);
			GenericValue<UTF16<>> value_modulepath(kStringType);
			value_modulepath.SetString(module_info.module_path.c_str(), allocator);
			jsonModuleObject.AddMember(key_modulepath, value_modulepath, allocator);

			jsonModuleArray.PushBack(jsonModuleObject, allocator);
		}

		jsonObject.AddMember(key_list, jsonModuleArray, allocator);
		jsonArray.PushBack(jsonObject, allocator);
	}

	// Total process list running on this computer
	document.AddMember(TEXT("pl"), jsonArray, allocator);

	GenericStringBuffer<UTF16<>> strbuf;
	Writer< GenericStringBuffer< UTF16<> >, UTF16<> > writer(strbuf);
	document.Accept(writer);

	return wstring(strbuf.GetString());
}

/**
* Check if two process structure are same
*/
BOOL isSame(PROCESS_STRUCT first, PROCESS_STRUCT second)
{
	if( first.process_name.compare(second.process_name) != 0 ) {
		return FALSE;
	}
	if (first.modules.size() != second.modules.size()) {
		return FALSE;
	}
	for (size_t i = 0; i < first.modules.size(); i++) {
		if (first.modules[i].module_name != second.modules[i].module_name
			|| first.modules[i].module_path != second.modules[i].module_path) {
			return FALSE;
		}
	}
	return TRUE;
}

/**
* Update process vector with the newly captured process vector
*/
vector<PROCESS_STRUCT> append2Origin(vector<PROCESS_STRUCT> origin_processes, vector<PROCESS_STRUCT> new_processes)
{
	vector<PROCESS_STRUCT> updated_processes = origin_processes;
	vector<PROCESS_STRUCT> temp_processes;
	vector<PROCESS_STRUCT>::iterator it;
	vector<PROCESS_STRUCT>::iterator itt;
	for (it = new_processes.begin(); it != new_processes.end(); it++) {
		PROCESS_STRUCT new_process = *it;
		BOOL found_same = FALSE;
		for (itt = origin_processes.begin(); itt != origin_processes.end(); itt++) {
			PROCESS_STRUCT origin_process = *itt;

			if (isSame(origin_process, new_process)) {		
				// If there are same process information, update time only
				found_same = TRUE;
				int index = distance(origin_processes.begin(), itt);
				updated_processes[index].run_time = new_process.run_time;
				break;
			}
		}
		if (!found_same) {
			temp_processes.push_back(new_process);
		}
	}
	updated_processes.insert(end(updated_processes), begin(temp_processes), end(temp_processes));
	return updated_processes;
}

/**
* Run capture flow
*/
void runCapture()
{
	vector<PROCESS_STRUCT> processes = GetProcessList();
	_p_processes = append2Origin(_p_processes, processes);
	wstring json_str = vector2Json(_p_processes);
	json2DBFile(json_str);
}

/**
* Init capture - load past process vector from local db
*/
void initCapture()
{
	wstring json_str = dbFile2Json();
	_p_processes = json2Vector(json_str);
}
