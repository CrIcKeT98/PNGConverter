#include "PNGConverter.h"

NOTIFYICONDATA Icon = { 0 };

bool myflag = false;

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
	{
		if (myflag) {
			MessageBox(0, TEXT("Привет"), TEXT(""), 0);
		}
	}
	case WM_USER:
		if (lParam == WM_RBUTTONDOWN)
			if (MessageBox(NULL, TEXT("Exit?"), TEXT("Tray"), MB_YESNO) == IDYES)
				DestroyWindow(window);
		break;
	// Стандартная обработка сообщений
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}

std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

bool checkDirectory(LPCSTR folderpath)
{
	DWORD dwFileAttributes = GetFileAttributesA(folderpath);
	if (dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY || dwFileAttributes == 22)
		return true;
	return false;
}

PNGConverter::PNGConverter(HINSTANCE instance, LPSTR dir) {
	m_monitoringFolder = s2ws(std::string(dir));

	if (!checkDirectory(dir)) {
		MessageBox(0, TEXT("This path is not available!"), 0, MB_OK);
		exit(0);
	}

	// Регистрация класса окна
	WNDCLASSEX main = { 0 };
	main.cbSize = sizeof(WNDCLASSEX);
	main.hInstance = instance;
	main.lpszClassName = TEXT("Main");
	main.lpfnWndProc = WndProc;
	RegisterClassEx(&main);

	// Создание главного окна
	HWND window = CreateWindowEx(0, TEXT("Main"), NULL, 0, 0, 0, 0, 0, NULL, NULL, instance, NULL);

	//Create directory handle
	m_hDir = CreateFile(
		this->m_monitoringFolder.c_str(),                       // pointer to the file name
		FILE_LIST_DIRECTORY,                // access (read/write) mode
		// Share mode MUST be the following to avoid problems with renames via Explorer!
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode
		NULL,                               // security descriptor
		OPEN_EXISTING,                      // how to create
		FILE_FLAG_BACKUP_SEMANTICS,         // file attributes
		NULL                                // file with attributes to copy
	);

	if (m_hDir == INVALID_HANDLE_VALUE) {
		MessageBox(0, TEXT("Directory handle error!"), 0, MB_OK);
		exit(0);
	}

	// Создание значка
	Icon.cbSize = sizeof(NOTIFYICONDATA);
	Icon.hWnd = window;
	Icon.uVersion = NOTIFYICON_VERSION;
	Icon.uCallbackMessage = WM_USER;
	Icon.hIcon = LoadIcon(NULL, IDI_SHIELD);
	Icon.uFlags = NIF_MESSAGE | NIF_ICON;
	Shell_NotifyIcon(NIM_ADD, &Icon);

	// Включение таймера
	SetTimer(window, 0, 1000, NULL);
}

