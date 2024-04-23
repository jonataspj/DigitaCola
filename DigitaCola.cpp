#include <Windows.h>
#include <iostream>
#include <shellapi.h>

HWND hWndHidden;
HICON hAppIcon;

#define BOTAO_SAIR 1001

DWORD WINAPI digitar(LPVOID data) {
	Sleep(500);
	if (!OpenClipboard(NULL)) return -1;
	HANDLE hndClipboardData = GetClipboardData(CF_UNICODETEXT);
	if (hndClipboardData == NULL) return -1;
	wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hndClipboardData));
	if (pszText == NULL) return -1;
	wchar_t* text(pszText);
	GlobalUnlock(hndClipboardData);
	CloseClipboard();
	for (wchar_t c : std::wstring(text)) {
		INPUT ip;
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = c;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;
		ip.ki.wVk = 0;
		ip.ki.dwFlags = KEYEVENTF_UNICODE;;
		SendInput(1, &ip, sizeof(INPUT));
		ip.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));
		Sleep(40);
	}
	
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case (WM_USER + 1): 
		{
			switch (LOWORD(lParam))
			{
			case WM_RBUTTONDOWN:
			{
				HMENU hPopupMenu = CreatePopupMenu();
				AppendMenu(hPopupMenu, MF_STRING, BOTAO_SAIR, L"Sair");
				POINT pt;
				GetCursorPos(&pt);
				TrackPopupMenu(hPopupMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWndHidden, NULL);
				DestroyMenu(hPopupMenu);
				break;
			}
			}
			break;
		}
		case WM_COMMAND:
		{
			exit(0);
			switch (LOWORD(wParam)) {
			case BOTAO_SAIR: {
				PostQuitMessage(0);
				break;
			}
					
			}
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);

	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"DigitaColaClass";
	RegisterClass(&wc);
	hWndHidden = CreateWindowEx(0, L"DigitaColaClass", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
	hAppIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWndHidden;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_USER + 1;
	nid.hIcon = hAppIcon;
	lstrcpy(nid.szTip, L"DigitaCola");
	Shell_NotifyIcon(NIM_ADD, &nid);

	if (!RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT | MOD_SHIFT, 0x56)) {
		return -1;
	}


	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY) {
			std::cout << "Hotkey pressed" << std::endl;
			HANDLE hnd = CreateThread(NULL, 0, digitar, NULL, 0, NULL);
			if (hnd != 0) {
				WaitForSingleObject(hnd, INFINITE);
				CloseHandle(hnd);
			}

		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}