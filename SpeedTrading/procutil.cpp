
#include "procutil.h"

#include "common.h"
#include "fileutil.h"
#include "logger.h"

#include <iostream>
#include <string>
#include <tchar.h>
#include <TlHelp32.h>

using namespace std;

vector<MODULE_STRUCT> ListProcessModules(DWORD dwPID)
{
	vector<MODULE_STRUCT> modules;

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		//printError(TEXT("CreateToolhelp32Snapshot (of modules)"));
		return modules;
	}

	// Set the size of the structure before using it.
	me32.dwSize = sizeof(MODULEENTRY32);

	// Retrieve information about the first module,
	// and exit if unsuccessful
	if (!Module32First(hModuleSnap, &me32))
	{
		//printError(TEXT("Module32First"));  // show cause of failure
		CloseHandle(hModuleSnap);           // clean the snapshot object
		return modules;
	}

	wstring system_directory = getSystem32Directory();

	// Now walk the module list of the process,
	// and display information about each module
	do
	{
		// Skip the modules in the windows/system32 folder
		if (findInsensitive(wstring(me32.szExePath), system_directory))
			continue;

		MODULE_STRUCT module_info;
		module_info.module_name = wstring(me32.szModule);
		module_info.module_path = wstring(me32.szExePath);

		modules.push_back(module_info);
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	return modules;
}

BOOL ListProcessThreads(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return(FALSE);

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if (!Thread32First(hThreadSnap, &te32))
	{
		printError(_T("Thread32First")); // show cause of failure
		CloseHandle(hThreadSnap);          // clean the snapshot object
		return(FALSE);
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			_tprintf(_T("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID);
			_tprintf(_T("\n     Base priority  = %d"), te32.tpBasePri);
			_tprintf(_T("\n     Delta priority = %d"), te32.tpDeltaPri);
			_tprintf(_T("\n"));
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return(TRUE);
}

/**
* Get running process list
*/
vector<PROCESS_STRUCT> GetProcessList()
{
	vector<PROCESS_STRUCT> processes;

	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//printError(_T("CreateToolhelp32Snapshot (of processes)"));
		return processes;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		//printError(_T("Process32First")); // show cause of failure
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return processes;
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		PROCESS_STRUCT process_info;
		process_info.process_name = wstring(pe32.szExeFile);
		process_info.run_time = getCurrentDateTime(DATE_TIME);
		vector<MODULE_STRUCT> modules = ListProcessModules(pe32.th32ProcessID);
		if( modules.size() == 0 ) {
			continue;
		}
		// List the modules and threads associated with this process
		process_info.modules = modules;
		
		processes.push_back(process_info);
		//ListProcessThreads(pe32.th32ProcessID);
	} while (Process32Next(hProcessSnap, &pe32));
		
	CloseHandle(hProcessSnap);
	return( processes );
}
