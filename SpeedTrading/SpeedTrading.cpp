// SpeedTrading.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "constants.h"
#include "Determination.h"
#include "Elevation.h"
#include "netmanager.h"
#include "procmanager.h"

using namespace std;

void checkPrivilege()
{
	if (!IsRunAsAdministrator()) {
		cout << "Should run under admin privilege\n";
		ElevateNow();
	}
}

void capture_thread_func()
{
	while (TRUE) {
		runCapture();
		cout << "Sleeping for 10 seconds in capture thread...\n";
		this_thread::sleep_for(std::chrono::seconds(CAPTURE_INTERVAL));
	}
}

void net_thread_func()
{
	while (TRUE) {
		runNetTransfer();
		cout << "Sleeping for 10 seconds in net thread...\n";
		this_thread::sleep_for(std::chrono::seconds(NET_INTERVAL));
	}
}

int main()
{
    std::cout << "Speed Trading V 1.0.0\n";

	//checkPrivilege();

	initCapture();

	thread capture_thread(capture_thread_func);
	thread net_thread(net_thread_func);

	capture_thread.join();
	net_thread.join();

	cout << "Ending now...\n";

	return 0;
}

