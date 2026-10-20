// stealth_clicker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "stealth_clicker.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

///////////////////////////////////////////////////////////////////////////////////////////////

HANDLE p = NULL;
HWND wecosia = NULL;

HANDLE hThread1 = NULL;
HANDLE hThread2 = NULL;
HANDLE hThread3 = NULL;
HANDLE hThread4 = NULL;

int wait1 = 200;
//int wait1 = 2000;
int wait2 = 30;

static HMODULE hmodDll = NULL;
typedef void(__stdcall *pFunc)(void);				//function pointer to call sethook and removehook functions
static pFunc ptrFunc;

static BOOL tstop1 = FALSE;
static BOOL tstop2 = FALSE;
static BOOL tstop3 = FALSE;
static BOOL tstop4 = FALSE;

WCHAR AppPath[2048];
WCHAR AppPathBuf[2048];

static int alpha_value = 1;

///////////////////////////////////////////////////////////////////////////////////////////////

BOOL createProcess(WCHAR* cl, WCHAR* wd)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcessW(NULL,   // No module name (use command line)
		cl,						// Command line
		NULL,					// Process handle not inheritable
		NULL,					// Thread handle not inheritable
		FALSE,					// Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,						// No creation flags
		NULL,					// Use parent's environment block
		//NULL,					// Use parent's starting directory 
		wd,					// Use parent's starting directory 
		&si,					// Pointer to STARTUPINFO structure
		&pi)					// Pointer to PROCESS_INFORMATION structure
		)
	{
		char error[16];
		ZeroMemory(error, 16);
		itoa(GetLastError(), error, 10);
		MessageBoxA(NULL, error, "CreateProcess Error", MB_OK);
		return FALSE;
	}

	// Wait until child process exits.
	//WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	//CloseHandle(pi.hProcess);
	//CloseHandle(pi.hThread);
}

BOOL ShellExecuteSynched(LPWSTR verb, LPWSTR path, LPWSTR params, LPWSTR dir, int nShow, DWORD mil)
{
	// Launch the executable according to the following params 
	SHELLEXECUTEINFOW shex;
	memset(&shex, 0, sizeof(SHELLEXECUTEINFOW));
	shex.cbSize = sizeof(SHELLEXECUTEINFOW);
	shex.fMask = SEE_MASK_NOCLOSEPROCESS;
	shex.lpVerb = verb;
	shex.lpFile = path;
	shex.lpParameters = params;
	shex.lpDirectory = dir;
	shex.nShow = nShow;

	// Check that we were able to successfully launch the administrator utility application 
	if (!ShellExecuteEx(&shex))
	{
		//MessageBoxW(NULL, L"Could not launch executable", L"OTPS Error", MB_OK);
		return FALSE;
	}

	p = shex.hProcess;

	// Wait for the application to finish  
	WaitForSingleObject(shex.hProcess, mil);

	CloseHandle(shex.hProcess);

	return TRUE;

	/*
	//check the return code to make sure the utility app succeeded
	DWORD exitCode;
	if (GetExitCodeProcess(shex.hProcess, &exitCode) == FALSE)
	{
	AfxMessageBox(_T("FAIL TWO!"));
	return;
	}
	if (exitCode != 0)
	{
	AfxMessageBox(_T("FAIL THREE!"));
	}
	*/
}

BOOL ShellExecuteAsynched(LPWSTR verb, LPWSTR path, LPWSTR params, LPWSTR dir, int nShow)
{
	// Launch the executable according to the following params 
	SHELLEXECUTEINFOW shex;
	memset(&shex, 0, sizeof(SHELLEXECUTEINFOW));
	shex.cbSize = sizeof(SHELLEXECUTEINFOW);
	shex.fMask = SEE_MASK_NOCLOSEPROCESS;
	shex.lpVerb = verb;
	shex.lpFile = path;
	shex.lpParameters = params;
	shex.lpDirectory = dir;
	shex.nShow = nShow;

	// Check that we were able to successfully launch the administrator utility application 
	if (!ShellExecuteEx(&shex))
	{
		//MessageBoxW(NULL, L"Could not launch executable", L"OTPS Error", MB_OK);
		return FALSE;
	}

	p = shex.hProcess;

	CloseHandle(shex.hProcess);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void hideAllChromeWindows(BOOL hide)
{
	//HWND fw = FindWindowEx(NULL, NULL, L"MozillaWindowClass", NULL);
	HWND fw = FindWindowEx(NULL, NULL, L"Chrome_WidgetWin_1", NULL);

	BOOL flag = FALSE;

	WCHAR title[4096];
	ZeroMemory(title, 4096 * sizeof WCHAR);

	//if (!wcscmp(title, L"benefind - Mozilla Firefox"))
	//if (!wcscmp(title, L"Websuche - Mozilla Firefox"))
	//if (!wcscmp(title, L"Settings - Manage search engines and site search - Google Chrome"))
	if (!wcscmp(title, L"Websuche - Google Chrome"))
	{
		MessageBox(NULL, L"Ecosia window found!", L"Info", MB_OK);
		wecosia = fw;
	}

	while (fw)
	{
		if (hide)
		{
			/*
			//ShowWindow(fw, SW_MAXIMIZE);
			ShowWindow(fw, SW_HIDE);
			SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) & ~WS_EX_APPWINDOW);
			SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
			*/

			long style = GetWindowLong(fw, GWL_STYLE);
			//style &= ~(WS_VISIBLE);    // this works - window become invisible 

			style &= ~(WS_EX_APPWINDOW);
			style |= WS_EX_TOOLWINDOW;   // flags don't work - windows remains in taskbar

			ShowWindow(fw, SW_HIDE); // hide the window
			SetWindowLong(fw, GWL_STYLE, style); // set the style
			SetWindowLong(fw, GWL_STYLE, style); // set the style
			//Sleep(500);
			ShowWindow(fw, SW_SHOW); // show the window for the new style to come into effect
			//ShowWindow(fw, SW_HIDE); // hide the window so we can't see it
		}
		else
		{
			//ShowWindow(fw, SW_SHOW);
			ShowWindow(fw, SW_MAXIMIZE);
		}

		fw = FindWindowEx(NULL, fw, L"Chrome_WidgetWin_1", NULL);
	}
}

void stealthAllChromeWindows(BOOL stealth)
{
	HWND fw = FindWindowEx(NULL, NULL, L"Chrome_WidgetWin_1", NULL);

	WCHAR title[4096];
	ZeroMemory(title, 4096 * sizeof WCHAR);

	while (fw)
	{
		GetWindowTextW(fw, title, 4095);
		//MessageBox(NULL, title, L"Window Title", MB_OK);

		//if (!wcscmp(title, L"benefind - Mozilla Firefox"))
		//if (!wcscmp(title, L"Websuche - Mozilla Firefox"))
		//if (!wcscmp(title, L"Settings - Manage search engines and site search - Google Chrome"))
		if (!wcscmp(title, L"Websuche - Google Chrome"))
		{
			//MessageBox(NULL, L"Ecosia window found!", L"Info", MB_OK);
			wecosia = fw;
		}

		if (stealth)
		{
			SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(fw, 0, alpha_value, LWA_ALPHA);
		}
		else
		{
			SetLayeredWindowAttributes(fw, 0, 255, LWA_ALPHA);
			SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_APPWINDOW);
			SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) & ~WS_EX_TOOLWINDOW);
		}

		fw = FindWindowEx(NULL, fw, L"Chrome_WidgetWin_1", NULL);
	}
}

void findEcosia()
{
	HWND fw = FindWindowEx(NULL, NULL, L"Chrome_WidgetWin_1", NULL);

	WCHAR title[4096];
	ZeroMemory(title, 4096 * sizeof WCHAR);

	while (fw)
	{
		GetWindowTextW(fw, title, 4095);
		//MessageBox(NULL, title, L"Window Title", MB_OK);

		//if (!wcscmp(title, L"benefind - Mozilla Firefox"))
		//if (!wcscmp(title, L"Websuche - Mozilla Firefox"))
		//if (!wcscmp(title, L"Settings - Manage search engines and site search - Google Chrome"))
		if (!wcscmp(title, L"Websuche - Google Chrome"))
		{
			//MessageBox(NULL, L"Ecosia window found!", L"Info", MB_OK);
			wecosia = fw;
		}

		//fw = FindWindowEx(NULL, fw, L"MozillaWindowClass", NULL);
		fw = FindWindowEx(NULL, fw, L"Chrome_WidgetWin_1", NULL);
	}
}

void stealthAllChromeWindows2()
{
	HWND fw = FindWindowEx(NULL, NULL, L"Chrome_WidgetWin_1", NULL);

	while (fw)
	{
		SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(fw, 0, alpha_value, LWA_ALPHA);

		fw = FindWindowEx(NULL, fw, L"MozillaWindowClass", NULL);
	}
}

void stealthAllChromeWindows3(HWND parent)
{
	HWND fw = FindWindowEx(parent, NULL, NULL, NULL);

	while (fw)
	{
		SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(fw, 0, alpha_value, LWA_ALPHA);

		fw = FindWindowEx(parent, fw, NULL, NULL);
	}
}

void stealthAllChromeWindows4()
{
	HWND fw = FindWindowEx(NULL, NULL, L"MozillaDropShadowWindowClass", NULL);

	while (fw)
	{
		SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(fw, 0, alpha_value, LWA_ALPHA);

		fw = FindWindowEx(NULL, fw, L"MozillaDropShadowWindowClass", NULL);
	}
}

void unstealthAllChromeWindows4()
{
	HWND fw = FindWindowEx(NULL, NULL, L"MozillaDropShadowWindowClass", NULL);

	while (fw)
	{
		SetLayeredWindowAttributes(fw, 0, 255, LWA_ALPHA);
		//SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_APPWINDOW);
		//SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) & ~WS_EX_TOOLWINDOW);

		fw = FindWindowEx(NULL, fw, L"MozillaDropShadowWindowClass", NULL);
	}
}

void stealthAllMozillaWindows5()
{
	HWND fw = FindWindowEx(NULL, NULL, L"SysShadow", NULL);

	while (fw)
	{
		SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(fw, 0, alpha_value, LWA_ALPHA);

		fw = FindWindowEx(NULL, fw, L"SysShadow", NULL);
	}
}

void stealthAllMozillaWindows6()
{
	HWND fw = FindWindowEx(NULL, NULL, L"MozillaDialogClass", NULL);

	while (fw)
	{
		SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(fw, 0, alpha_value, LWA_ALPHA);

		fw = FindWindowEx(NULL, fw, L"MozillaDialogClass", NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void SetClipboardText2(HWND hwnd, WCHAR * szText)
{
	HGLOBAL globalHandle = GlobalAlloc(GHND | GMEM_SHARE, (wcslen(szText) + 1) * sizeof(WCHAR));
	//HGLOBAL globalHandle = GlobalAlloc(GHND | GMEM_SHARE, (wcslen(szText) + 1) * sizeof(TCHAR));

	WCHAR * szClipboard = (WCHAR*)GlobalLock(globalHandle);
	wcscpy(szClipboard, szText);
	GlobalUnlock(globalHandle);
	OpenClipboard(hwnd);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, globalHandle);
	CloseClipboard();
}

void SetClipboardText3(HWND hwnd, WCHAR * szText)
{
	HGLOBAL globalHandle = GlobalAlloc(GHND | GMEM_SHARE, (wcslen(szText) + 1) * sizeof(WCHAR));
	//HGLOBAL globalHandle = GlobalAlloc(GHND | GMEM_SHARE, (wcslen(szText) + 1) * sizeof(TCHAR));

	WCHAR * szClipboard = (WCHAR*)GlobalLock(globalHandle);
	wcscpy(szClipboard, szText);
	GlobalUnlock(globalHandle);
	OpenClipboard(hwnd);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, globalHandle);
	CloseClipboard();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void hideThread1(void* pArguments)
{
	//MessageBox(NULL, L"Thread Running!", L"Info", MB_OK);
	
	/*
	//for (int i = 0; i < 300; i++)
	while (!tstop1)
	{
		hideAllChromeWindows(TRUE);
	}
	*/

	while (!tstop1)
	{
		//----TRUE für unsichtbar
		hideAllChromeWindows(TRUE);
		//Sleep(50);
		Sleep(1);
		//----TRUE für unsichtbar
		stealthAllChromeWindows(TRUE);
		Sleep(1);
	}

	//MessageBox(NULL, L"Thread 1 stopping!", L"Info", MB_OK);
}

void hideThread1b(void* pArguments)
{
	//MessageBox(NULL, L"Thread Running!", L"Info", MB_OK);

	//for (int i = 0; i < 300; i++)
	while (!tstop1)
	{
		//----TRUE für unsichtbar
		stealthAllChromeWindows(TRUE);
	}

	//MessageBox(NULL, L"Thread 1 stopping!", L"Info", MB_OK);
}

void hideThread2(void* pArguments)
{
	//MessageBox(NULL, L"Thread Running!", L"Info", MB_OK);

	//for (int i = 0; i < 300; i++)
	while (!tstop2)
	{
		////stealthAllMozillaWindows4();
		////stealthAllMozillaWindows5();
	}
}


void hideThread3(void* pArguments)
{
	//MessageBox(NULL, L"Thread Running!", L"Info", MB_OK);

	//for (int i = 0; i < 300; i++)
	while (!tstop3)
	{
		stealthAllMozillaWindows6();
	}
}

void hideThread4(void* pArguments)
{
	//MessageBox(NULL, L"Thread Running!", L"Info", MB_OK);

	//for (int i = 0; i < 300; i++)
	while (!tstop4)
	{
		//stealthAllMozillaWindows2();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LONG lParam)
{
	char wclass[256];
	ZeroMemory(wclass, 256);

	char rwclass[256];
	ZeroMemory(rwclass, 256);

	GetClassNameA(hwnd, wclass, 255);
	RealGetWindowClassA(hwnd, rwclass, 255);

	FILE* f = fopen("windows.txt", "a");

	fprintf(f, "classname: %s, realclass: %s\n", wclass, rwclass);

	fflush(f);
	fclose(f);

	return TRUE;
}

void recordAllTopLevelWindows()
{
	EnumWindows(EnumWindowsProc, (LPARAM)0);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void activateWindow(HWND hwnd)
{
	if (!hwnd)
	{
		MessageBox(NULL, L"HWND to activate is NULL!", L"Error", MB_OK);
		return;
	}

	SetForegroundWindow(hwnd);
	SetActiveWindow(hwnd);
	BringWindowToTop(hwnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void leftMouseDown(int x, int y)
{
	INPUT input;
	ZeroMemory(&input, sizeof(input));
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.dx = x*(65536 / GetSystemMetrics(SM_CXSCREEN));//x being coord in pixels
	input.mi.dy = y*(65536 / GetSystemMetrics(SM_CYSCREEN));//y being coord in pixels
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(input));
}

void leftMouseUp(int x, int y)
{
	INPUT input;
	ZeroMemory(&input, sizeof(input));
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	//input.mi.dx = x*(65536 / GetSystemMetrics(SM_CXSCREEN));//x being coord in pixels
	//input.mi.dy = y*(65536 / GetSystemMetrics(SM_CYSCREEN));//y being coord in pixels
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(input));
}

void moveMouse(int x, int y)
{
	INPUT input;
	ZeroMemory(&input, sizeof(input));
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.dx = x*(65536 / GetSystemMetrics(SM_CXSCREEN));//x being coord in pixels
	input.mi.dy = y*(65536 / GetSystemMetrics(SM_CYSCREEN));//y being coord in pixels
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	SendInput(1, &input, sizeof(input));
}

///////////////////////////////////////////////////////////////////////////////////////////////

void SetKeyUp(BOOL bState)
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);
	if ((bState && !(keyState[VK_UP] & 1)) ||
		(!bState && (keyState[VK_UP] & 1)))
	{
		// Simulate a key press
		keybd_event(VK_UP,
			0x45,
			KEYEVENTF_EXTENDEDKEY | 0,
			0);

		Sleep(wait2);

		// Simulate a key release
		keybd_event(VK_UP,
			0x45,
			KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
			0);
	}
}

void MessageBoxInt(int myint)
{
	//int index1 = 1;
	char buf[10];
	itoa(myint, buf, 10);
	MessageBoxA(NULL, buf, "Integer", MB_OK);
}

void addNewSearchProvider_old()
{
	//MessageBox(NULL, L"Will add new search provider!", L"Info", MB_OK);

	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press CTRL+L
	// Press the "Ctrl" key first time
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "L" key
	ip.ki.wVk = 'L';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "L" key
	ip.ki.wVk = 'L';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	
	// Press the "TAB" key first time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	

	/*hThread3 = (HANDLE)_beginthread(hideThread3, 0, NULL);
	SetThreadPriority(hThread3, THREAD_PRIORITY_HIGHEST);*/

	
	// Press the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	//return;
	
	//SetKeyUp(TRUE);
	//Sleep(wait1);

	//tst5op3 = TRUE;
	
	//MessageBoxInt(GetSystemMetrics(SM_CXSCREEN));

	
	int myy[10];

	int myx = (GetSystemMetrics(SM_CXSCREEN) * 4) /5;
	myy[0] = (GetSystemMetrics(SM_CYSCREEN) * 2) /5;
	myy[1] = (GetSystemMetrics(SM_CYSCREEN) * 1.90) / 5;
	myy[2] = (GetSystemMetrics(SM_CYSCREEN) * 1.80) / 5;
	myy[3] = (GetSystemMetrics(SM_CYSCREEN) * 1.70) / 5;
	myy[4] = (GetSystemMetrics(SM_CYSCREEN) * 1.60) / 5;
	myy[5] = (GetSystemMetrics(SM_CYSCREEN) * 1.50) / 5;
	myy[6] = (GetSystemMetrics(SM_CYSCREEN) * 1.40) / 5;
	myy[7] = (GetSystemMetrics(SM_CYSCREEN) * 1.30) / 5;
	myy[8] = (GetSystemMetrics(SM_CYSCREEN) * 1.20) / 5;
	myy[9] = (GetSystemMetrics(SM_CYSCREEN) * 1.10) / 5;

	//MessageBoxInt(myx);
	//moveMouse(myx, myy[9]);

	/*
	for (int i = 0; i < 10; i++)
	{
		leftMouseDown(myx, myy[i]);
		Sleep(wait2);
		leftMouseUp(myx, myy[i]);
		Sleep(wait2);
	}
	*/

	
	leftMouseDown(myx, myy[9]);
	Sleep(wait2);
	leftMouseUp(myx, myy[9]);
	Sleep(wait2);
	
	Sleep(wait1);
	

	/*
	// Press the "UP" key one time
	ip.ki.wVk = VK_UP;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(29);

	// Release the "UP" key
	ip.ki.wVk = VK_UP;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	*/

	/*
	// Press the "UP" key one time
	ip.ki.wVk = VK_UP;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "UP" key
	ip.ki.wVk = VK_UP;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	
	// Press the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	*/
}

void addNewSearchProvider()
{
	//MessageBox(NULL, L"Will add new search provider!", L"Info", MB_OK);

	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press CTRL+L
	// Press the "Ctrl" key first time
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "L" key
	ip.ki.wVk = 'L';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "L" key
	ip.ki.wVk = 'L';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key first time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	/*hThread3 = (HANDLE)_beginthread(hideThread3, 0, NULL);
	SetThreadPriority(hThread3, THREAD_PRIORITY_HIGHEST);*/

	hThread4 = (HANDLE)_beginthread(hideThread4, 0, NULL);
	SetThreadPriority(hThread4, THREAD_PRIORITY_HIGHEST);

	// Press the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	tstop4 = TRUE;

	/*
	// Press CTRL+L
	// Press the "Ctrl" key first time
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "L" key
	ip.ki.wVk = 'L';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "L" key
	ip.ki.wVk = 'L';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	SetClipboardText2(NULL, L"about:preferences");

	// Press CTRL+V to paste URL into URL text box ///////////////////////
	// Press the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	SetClipboardText3(NULL, L"Standardsuchmaschine");

	// Press CTRL+V to paste URL into URL text box ///////////////////////
	// Press the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Press the "END" key
	ip.ki.wVk = VK_END;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "END" key
	ip.ki.wVk = VK_END;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	*/
}

void addNewSearchProviderChrome()
{
	//MessageBox(NULL, L"Will add new search provider!", L"Info", MB_OK);

	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	//////////////////////////////////////////////////////////////////////////////////

	// Press CTRL+L Adressleiste fokusieren
	// Press the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "V" key
	ip.ki.wVk = 'L';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "V" key
	ip.ki.wVk = 'L';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	Sleep(1000);
	//////////////////////////////////////////////////////////////////////////////////

	SetClipboardText3(NULL, L"chrome://settings/searchEngines");

	// Press CTRL+V to paste URL into URL text box 
	// Press the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	//////////////////////////////////////////////////////////////////////////////////
	
	// Press the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	Sleep(1000);
	
	//////////////////////////////////////////////////////////////////////////////////
	
	// Press the "TAB" key first time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	//////////////////////////////////////////////////////////////////////////////////
	
	SetClipboardText3(NULL, L"youtube");

	// Press CTRL+V to paste URL into URL text box 
	// Press the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	//////////////////////////////////////////////////////////////////////////////////
	
	// Press the "TAB" key first time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	// Press the "TAB" key first time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	// Press the "TAB" key first time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	// Press the "TAB" key first time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	// Press the "TAB" key first time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	//////////////////////////////////////////////////////////////////////////////////
	
	// Press the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	// Press the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "ENTER" key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	//////////////////////////////////////////////////////////////////////////////////

}

void closeCurrentTabs()
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// ALT+F4

	// ALT drücken
	ip.ki.wVk = VK_MENU;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// F4 drücken
	ip.ki.wVk = VK_F4;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// F4 loslassen
	ip.ki.wVk = VK_F4;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);

	// ALT loslassen
	ip.ki.wVk = VK_MENU;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////

BOOL setHook()
{
	//FILE* f = fopen("log.txt", "w");
	//FILE* f = fopen("F:\\ALEX\\___WORK___\\__eWork__\\upwork\\Mozila_Silent_Search_Provider_Mod\\StealthClicker_r3\\bin\\log.txt", "w");

	hmodDll = LoadLibraryW(L"hookDll.dll");

	ptrFunc = (pFunc)GetProcAddress(hmodDll, "SetHook");		//get the address of SetHook function

	ptrFunc();													//call sethook function

	return TRUE;
}

BOOL removeHook()
{
	if (hmodDll == NULL) return FALSE;

	ptrFunc = (pFunc)GetProcAddress(hmodDll, "RemoveHook");		//get the address of RemoveHook function

	if (ptrFunc == NULL) return FALSE;

	ptrFunc();													//Call Remove hook

	FreeLibrary(hmodDll);										//unload our hook dll

	ptrFunc = NULL;
	hmodDll = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////

PWCHAR AppPathCat(PWCHAR path)
{
	wcscpy(AppPathBuf, AppPath);
	wcscat(AppPathBuf, L"\\");
	wcscat(AppPathBuf, path);

	return AppPathBuf;
}

BOOL getAppPath(PWCHAR AppPath)
{
	if (!GetModuleFileNameW(NULL, AppPath, 2048))
	{
		return FALSE;
	}

	PWCHAR pos = wcsrchr(AppPath, L'\\');

	*pos = L'\0';

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);

	//MessageBox(NULL, L"Will execute!", L"Info", MB_OK);

	if (!getAppPath(AppPath))
	{
		//MessageBox(NULL, L"Could not get app path", L"Error", MB_OK);
		return 1;
	}

	/*
	if (!IsSupportedOS())
	{
	//MessageBox(NULL, L"Not Win8.1+", L"Info", MB_OK);
	return 2;
	}
	*/

	//ShellExecuteAsynched(L"runas", L"taskkill", L"/F /T /IM firefox.exe", NULL, SW_HIDE);

	//MessageBox(NULL, L"Done checks!", L"Info", MB_OK);

	hThread1 = (HANDLE)_beginthread(hideThread1, 0, NULL);
	SetThreadPriority(hThread1, THREAD_PRIORITY_HIGHEST);
	

	/*
	hThread1 = (HANDLE)_beginthread(hideThread1b, 0, NULL);
	SetThreadPriority(hThread1, THREAD_PRIORITY_HIGHEST);
	*/

	//Sleep(5000);

	//C:\Program Files(x86)\Mozilla Firefox
	//ShellExecuteAsynched(L"runas", L"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe", L"https://www.ecosia.org/", L"C:\\Program Files (x86)\\Mozilla Firefox", SW_MAXIMIZE);
	//ShellExecuteAsynched(L"runas", L"firefox.exe", L"https://www.ecosia.org/", NULL, SW_MAXIMIZE);
	//if (!ShellExecuteAsynched(L"runas", L"lala.exe", L"https://www.benefind.de/", NULL, SW_HIDE))
	//if (!ShellExecuteAsynched(L"runas", L"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe", L"https://www.ecosia.org/", L"C:\\Program Files (x86)\\Mozilla Firefox", SW_MAXIMIZE))
	BlockInput(TRUE);
	if (!ShellExecuteAsynched(L"runas", L"chrome.exe", L"https://www.youtube.com", NULL, SW_MAXIMIZE))
	//if (!ShellExecuteAsynched(L"runas", L"chrome.exe", L"chrome://settings/searchEngines", NULL, SW_MAXIMIZE))
	{
		tstop1 = TRUE;
		Sleep(500);
		//hideAllChromeWindows(FALSE);
		return 3;
	}

	Sleep(500);

	//createProcess(L"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe", L"C:\\Program Files (x86)\\Mozilla Firefox");
	//createProcess(L"firefox.exe");
	//createProcess(L"C:\\Windows\\notepad.exe", L"C:\\Windows");
	//createProcess(L"putty.exe", NULL);

	/*
	GUITHREADINFO gti;
	GetGUIThreadInfo(GetThreadId(p), &gti);
	HANDLE aw = gti.hwndActive;
	*/

	
	for (int i = 0; i < 2; i++)
	{
		//----TRUE für unsichtbar
		hideAllChromeWindows(TRUE);
		Sleep(500);
		//----TRUE für unsichtbar
		stealthAllChromeWindows(TRUE);
		Sleep(1);
	}

	//Sleep(500);
	

	/*
	for (int i = 0; i < 300; i++)
	{
	hideAllMozillaWindows(TRUE);
	Sleep(10);
	}
	*/

	//SetWindowLong(wecosia, GWL_EXSTYLE, GetWindowLong(wecosia, GWL_EXSTYLE) & ~WS_EX_APPWINDOW);
	//SetWindowLong(wecosia, GWL_EXSTYLE, GetWindowLong(wecosia, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
	//ShowWindow(wecosia, SW_MAXIMIZE);

	
	Sleep(300);
	tstop1 = TRUE;
	Sleep(1000);

	findEcosia();

	ShowWindow(wecosia, SW_SHOW);
	ShowWindow(wecosia, SW_MAXIMIZE);

	//findEcosia();
	////stealthAllChromeWindows(TRUE);

	UINT my_id = GetCurrentThreadId();
	UINT win_id = GetWindowThreadProcessId(wecosia, NULL);
	AttachThreadInput(my_id, win_id, true);

	/*
	// TEST SendMessage() ///////////////////////////////
	hideAllMozillaWindows(FALSE);
	stealthAllMozillaWindows(FALSE);
	Sleep(1000);

	UINT scan1 = MapVirtualKey(VK_CONTROL, 0);
	UINT scan2 = MapVirtualKey(0x4B, 0); // 'K'

	// Build the generic lparam to be used for WM_KEYDOWN/WM_KEYUP/WM_CHAR
	LPARAM lparam1 = (0x00000001 | (LPARAM)(scan1 << 16)); //| 0x01000000;         // Scan code, repeat=1
	LPARAM lparam2 = 0x00000001 | (LPARAM)(scan2 << 16);                        // Scan code, repeat=1

	//SendMessage(wecosia, WM_CHAR, VK_CONTROL, 0);
	//SendMessage(wecosia, WM_CHAR, 0x4B, 0);
	//return 1;
	SendMessage(wecosia, WM_KEYDOWN, VK_CONTROL, lparam1);
	SendMessage(wecosia, WM_KEYDOWN, 0x4B, lparam2);
	Sleep(500);
	SendMessage(wecosia, WM_KEYUP, 0x4B, lparam2);
	SendMessage(wecosia, WM_KEYUP, VK_CONTROL, lparam1);
	return 1;
	/////////////////////////////////////////////////////
	*/

	//SetWindowLong(wecosia, GWL_EXSTYLE, GetWindowLong(wecosia, GWL_EXSTYLE) | WS_EX_TOPMOST);
	//SetWindowPos(wecosia, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

	//ShowWindow(wecosia, SW_RESTORE);
	//activateWindow(wecosia);
	//SwitchToThisWindow(wecosia, TRUE);

	// OPERATION START //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BlockInput(TRUE);

	//Sleep(1000);

	////hideAllChromeWindows(FALSE);

	//Sleep(1000);

	////setHook();
	SetFocus(wecosia);
	PostMessage(wecosia, WM_SETFOCUS, NULL, NULL);

	//UINT my_id = GetCurrentThreadId();
	//UINT win_id = GetWindowThreadProcessId(wecosia, NULL);
	//AttachThreadInput(my_id, win_id, true);
	SendMessage(wecosia, WM_SETFOCUS, NULL, NULL);
	//Sleep(1000);
	//Sleep(10000);
	//Sleep(5000);
	//removeHook();

	//ShellExecuteAsynched(L"runas", L"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe", L"about:blank", L"C:\\Program Files (x86)\\Mozilla Firefox", SW_HIDE);
	//stealthAllMozillaWindows(FALSE);

	/*
	Sleep(300);

	//Click on injected invisible popup that gives the FF window focus
	leftMouseDown(500, 300);
	Sleep(50);
	leftMouseUp(500, 300);

	Sleep(250);
	//removeHook();
	*/

	//SetWindowPos(wecosia, HWND_TOP, GetSystemMetrics(SM_CXSCREEN) - 200, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);


	////hThread2 = (HANDLE)_beginthread(hideThread2, 0, NULL);
	////SetThreadPriority(hThread2, THREAD_PRIORITY_HIGHEST);


	//Perform series of keystrokes to add a new search provider as default
	//addNewSearchProvider();
	addNewSearchProviderChrome();
	//makeLastSearchProviderDefault();

	////tstop2 = TRUE;

	//SetWindowPos(wecosia, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

	//Send CTRL+W twice to close the two latest FF tabs (Ecosia + Search engine manager page)
	closeCurrentTabs();
	//Sleep(1000);

	BlockInput(FALSE);
	// OPERATION END //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Wait 500ms
	//Sleep(500);

	//Make any remaining Mozilla windows visible again


	//stealthAllMozillaWindows(FALSE);
	//unstealthAllMozillaWindows4();

	//int retval = ::_tsystem(_T("taskkill /F /T /IM firefox.exe"));

	ShellExecuteSynched(L"runas", L"taskkill", L"/F /T /IM chrome.exe", NULL, SW_HIDE, 5000);
	ShellExecuteSynched(L"runas", L"taskkill", L"/F /T /IM msedge.exe", NULL, SW_HIDE, 5000);
	
	return 1;
}
