#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

class PNGConverter
{
private:
	//static NOTIFYICONDATA Icon;
	//std::wstring monitoringFolder;
	//HANDLE hDir;

	WNDCLASSEX main = { 0 };
	

public:
	PNGConverter(HINSTANCE instance, HINSTANCE WinApiMessage, LPSTR dir);
	HANDLE hDir;
	std::wstring monitoringFolder;
	std::wstring s2ws(const std::string& str);
};

