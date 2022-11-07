#include <codecvt>
#include "PNGConverter.h"

bool checkDirectory(LPCWSTR folderpath)//wchar_t*
{
	DWORD dwFileAttributes = GetFileAttributes(folderpath);
	if (dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY || dwFileAttributes == 22)
		return true;
	return false;
}

extern NOTIFYICONDATA Icon;

std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE WinApiMessage, LPSTR dir, int)
{
	PNGConverter mainClass(instance, WinApiMessage, dir);

	auto hDir = mainClass.hDir;
	auto dirW = mainClass.monitoringFolder;

	TCHAR szBuffer[1024 * 128];
	DWORD BytesReturned;
	MSG message;
	//»де€: в while провер€ть сразу ReadDirectory и PeekMessage(GetMessage), но присваивать результат PeekMessage переменной. ƒалее в цикле провер€ть значение переменной и делать что-либо
	BOOL bRet;

	//Create WinAPI hook to check folder
	while (ReadDirectoryChangesW(
		hDir,                          // handle to directory
		&szBuffer,                       // read results buffer
		sizeof(szBuffer),                // length of buffer
		TRUE,                          // monitoring option
		FILE_NOTIFY_CHANGE_SECURITY |
		FILE_NOTIFY_CHANGE_CREATION |
		FILE_NOTIFY_CHANGE_LAST_WRITE |
		FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_ATTRIBUTES |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_FILE_NAME,  // filter conditions
		&BytesReturned,                // bytes returned
		NULL,                          // overlapped buffer
		NULL                           // completion routine
	) || (bRet = GetMessage(&message, NULL, 0, 0)))
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		

		DWORD dwOffset = 0;
		FILE_NOTIFY_INFORMATION* pInfo = NULL;

		do {
			// Get a pointer to the first change record
			pInfo = (FILE_NOTIFY_INFORMATION*)&szBuffer[dwOffset];

			std::wstring file_name;
 			std::wstring file_extension;
			std::wstring magic_cmd(L" \"");

			if (pInfo->Action == FILE_ACTION_ADDED) {
				//Get file extension
				for (size_t i = 0; i < pInfo->FileNameLength; i++) {
					if (pInfo->FileName[i] == L'.' && i < pInfo->FileNameLength - 3) {
						for (size_t j = 0; j < 4; j++) file_extension.push_back(pInfo->FileName[j + i]);
						break;
					}
					else file_name.push_back(pInfo->FileName[i]);
				}

				//Creating commands args for magick
				if (file_extension == L".png") {
					magic_cmd += dirW + L"\\" + file_name + file_extension + L'\"' + L' ' + L'\"' + dirW + L"\\" + file_name + L".jpg" + L'\"';

					//Auxiliary structures for CreateProcess
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory(&si, sizeof(si));
					ZeroMemory(&pi, sizeof(pi));
					
					//Create WinApi process
					BOOL bCreateProcess = NULL;
					bCreateProcess = CreateProcessW(
						L"C:\\Users\\CrIcKeT\\source\\repos\\PNGConverter\\Debug\\magick.exe",
						(LPWSTR)magic_cmd.c_str(),
						NULL, 
						NULL, 
						FALSE, 
						CREATE_NO_WINDOW,
						NULL, 
						NULL,
						&si, 
						&pi);
					
					if (bCreateProcess == FALSE) {
						MessageBox(0, L"Cannot create process!", 0, MB_OK);
						return 0;
					}

					WaitForSingleObject(pi.hProcess, INFINITE);
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
					DeleteFile((dirW + L"\\" + file_name + file_extension).c_str());
				}
				file_name.clear();
				file_extension.clear();
				magic_cmd.clear();
			}

			// More than one change may happen at the same time. Load the next change and continue
			dwOffset += pInfo->NextEntryOffset;

		}while (pInfo->NextEntryOffset != 0);

		//Tray context menu
	}

	//Delete icon
	Shell_NotifyIcon(NIM_DELETE, &Icon);
	return 0;
}