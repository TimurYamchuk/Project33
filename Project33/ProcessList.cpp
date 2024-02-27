// stdafx.cpp : source file that includes just the standard includes
// ProcessList.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "ProcessListDlg.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
	CProcess_ListDlg dlg;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)CProcess_ListDlg::DlgProc);
}