#pragma once
#include "header.h"
// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

class CProcess_ListDlg
{
public: CProcess_ListDlg(void);
	  ~CProcess_ListDlg(void);
	  static BOOL CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	  static CProcess_ListDlg* ptr;
	  void Cls_OnClose(HWND hwnd);
	  BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	  void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	  HWND hListAll, hEditField, hUpdate, hTerminate, hInfo, hCreate;
	  void UpdateProcessList(HWND hwnd, int id);
	  void GetProcessInfo(HWND hwnd, int id);
	  void TerminateProcess_(HWND hwnd, int id);
	  void CreateProcessFromPath(HWND hwnd, int id);
	  void Cls_OnTimer(HWND hwnd, UINT id);
	  DWORD GetProcessIDByName(const TCHAR* processName);
	  BOOL IsSystemProcess(DWORD processID);
};
