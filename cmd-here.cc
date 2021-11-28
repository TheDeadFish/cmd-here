#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>

struct WndList {
	HWND hwnd;
	WCHAR* path;
};

bool flag_duplicate;
bool flag_explorer;
LPCSTR wndClass = "ConsoleWindowClass";
WCHAR path[MAX_PATH];
WndList wndList[256];
int nWndList;

void start_cmd(WCHAR* path)
{
	if(flag_explorer) {
		wcscat(path, L"\""); *--path = '\"';
		ShellExecuteW(NULL, NULL, L"explorer", path, 0, SW_SHOW); 
	} else {
		ShellExecuteW(NULL, NULL, L"cmd", 0, path, SW_SHOW); 
	}
}

void add_path(WCHAR* str, HWND hwnd)
{
	if(path[0] == 0)
		wcscpy(path, str);

	char buff[100];
	GetClassNameA(hwnd, buff, 100);
	if(!strcmp(buff, wndClass)) {
		wndList[nWndList].hwnd = hwnd;
		wndList[nWndList].path = wcsdup(str);
		nWndList++;
	}
}

static bool isPathSep(int ch) {
	return (ch=='\\')||(ch=='/'); }

BOOL CALLBACK EnumWindowsProc(
	HWND   hwnd, LPARAM lParam)
{
	WCHAR text_[MAX_PATH+1];

	if((IsWindowVisible(hwnd))
	&&(!IsIconic(hwnd))) { WCHAR* text = text_;
	GetWindowTextW(hwnd, text, MAX_PATH);
	
	// locate valid path base
	for(; *text; text++) {
		if((isPathSep(text[0])&&isPathSep(text[1]))
		||((text[1] == ':')&& isPathSep(text[2])))
		goto FOUND_BASE; } if(0) { FOUND_BASE: 
	
	// get valid path/name
	int atrb; size_t len = wcslen(text); 
	while(len) { atrb = GetFileAttributesW(text);
	if(atrb >= 0) break;
	while(len && !isspace(text[len-1])) len--;
	while(len && isspace(text[len-1])) len--;
	text[len] = 0; }
	
	// get valid path
	if((atrb & 0x10) == 0) {
	while(len) { len--; if((text[len] == '\\')
	|| (text[len] == '/')) break; }
	text[len] = 0; }
	atrb = GetFileAttributesW(text);
	if(atrb >= 0) add_path(text, hwnd); }}
	return TRUE;
}

int wmain()
{
	flag_duplicate = GetAsyncKeyState(VK_MBUTTON) < 0;
	flag_explorer = GetKeyState(VK_SHIFT) < 0;
	if(flag_explorer)
		wndClass = "CabinetWClass";

	if(GetKeyState(VK_CONTROL) < 0)
		SHGetFolderPathW(0, 0x10, NULL, 0, path);
	EnumWindows(EnumWindowsProc, 0);
	if(path[0] == 0)
		SHGetFolderPathW(0, 0x10, NULL, 0, path);

	if(!flag_duplicate) {
		for(int i = 0; wndList[i].hwnd; i++) {
			if(!lstrcmpW(wndList[i].path, path)) {
				SetForegroundWindow(wndList[i].hwnd);
				return 0;
			}
		}
	}

	start_cmd(path);
	return 0;
}
