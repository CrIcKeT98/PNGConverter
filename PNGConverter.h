#pragma once
#include <Windows.h>
#include <string>

std::wstring s2ws(const std::string& str); //string to wide string 
LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
bool checkDirectory(LPCSTR folderpath);

class PNGConverter
{
public:
	PNGConverter(HINSTANCE instance, LPSTR dir);

private:
	HANDLE m_hDir;
	std::wstring m_monitoringFolder;
};

