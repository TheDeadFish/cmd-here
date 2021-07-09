#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>

WCHAR cmdExe[] = L"cmd.exe";

void start_cmd(WCHAR* path)
{
	if(GetKeyState(VK_SHIFT) < 0) {
		wcscat(path, L"\""); *--path = '\"';
		ShellExecuteW(NULL, NULL, L"explorer.exe", path, 0, SW_SHOW); 
	} else {
		ShellExecuteW(NULL, NULL, L"cmd.exe", 0, path, SW_SHOW); 
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
	if(atrb >= 0) { start_cmd(text);
	return FALSE; }}} return TRUE;
}


int wmain()
{
	if(GetKeyState(VK_CONTROL) < 0) { L1:
	WCHAR path[MAX_PATH]; SHGetFolderPathW(
		0, 0x10, NULL, 0, path);
	start_cmd(path);
	} else { if(EnumWindows(EnumWindowsProc, 0))
		goto L1; }
	return 0;
}
