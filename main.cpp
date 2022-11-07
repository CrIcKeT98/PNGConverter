#include <codecvt>
#include "PNGConverter.h"
#include <iostream>
using namespace std;

extern NOTIFYICONDATA Icon;

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE WinApiMessage, LPSTR dir, int)
{
	PNGConverter mainClass(instance, dir);

	// Цикл обработки сообщений
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	Shell_NotifyIcon(NIM_DELETE, &Icon);
	return 0;
}