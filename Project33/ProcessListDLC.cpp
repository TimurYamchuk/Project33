#include "ProcessListDLC.h"

CProcess_ListDlg* CProcess_ListDlg::ptr = NULL;

CProcess_ListDlg::CProcess_ListDlg(void) {
    ptr = this;
}

CProcess_ListDlg::~CProcess_ListDlg(void) {

}

void CProcess_ListDlg::Cls_OnClose(HWND hwnd) {
    EndDialog(hwnd, 0);
}

DWORD CProcess_ListDlg::GetProcessIDByName(const TCHAR* processName) {
    DWORD processID = 0;
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapShot, &pe32)) {
        do {
            if (_tcscmp(pe32.szExeFile, processName) == 0) {
                processID = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapShot, &pe32));
    }

    CloseHandle(hSnapShot);
    return processID;
}

BOOL CProcess_ListDlg::IsSystemProcess(DWORD processID) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);
    if (hProcess == NULL) {
        return FALSE;
    }

    DWORD processSessionID;
    BOOL isSystemProcess = FALSE;

    if (ProcessIdToSessionId(processID, &processSessionID) && processSessionID == 0) {
        isSystemProcess = TRUE;
    }

    CloseHandle(hProcess);
    return isSystemProcess;
}

void CProcess_ListDlg::UpdateProcessList(HWND hwnd, int id) {
    SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapShot == NULL) {
        hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapShot == INVALID_HANDLE_VALUE) {
            MessageBox(hwnd, TEXT("Ошибка при создании снимка процессов"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
            return;
        }
    }

    PROCESSENTRY32 pe32;
    memset(&pe32, 0, sizeof(PROCESSENTRY32));
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapShot, &pe32)) {
        wchar_t buffer[256];
        wsprintf(buffer, TEXT("Name: %s"), pe32.szExeFile);
        SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)buffer);

        while (Process32Next(hSnapShot, &pe32)) {
            wsprintf(buffer, TEXT("%s"), pe32.szExeFile);
            SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)buffer);

            int count = SendMessage(hwnd, LB_GETCOUNT, 0, 0);
            SendMessage(hwnd, LB_SETCURSEL, count - 1, 0);
            SendMessage(hwnd, LB_SETCARETINDEX, count - 1, TRUE);

            Sleep(1);
        }
    }
    else {
        MessageBox(hwnd, TEXT("Ошибка при создании снимка процессов"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
        return;
    }

    CloseHandle(hSnapShot);
}

void CProcess_ListDlg::GetProcessInfo(HWND hwnd, int id) {
    int selectedIndex = SendMessage(hListAll, LB_GETCURSEL, 0, 0);

    if (selectedIndex != LB_ERR) {
        TCHAR* buffer = new TCHAR[256];
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe32;
        memset(&pe32, 0, sizeof(PROCESSENTRY32));
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapShot, &pe32)) {
            for (int i = 0; i < selectedIndex; ++i) {
                if (!Process32Next(hSnapShot, &pe32)) {
                    delete[] buffer;
                    return;
                }
            }
        }

        wsprintf(buffer, TEXT("ID: %u  Threads: %u  Priority: %u  Name: %s"), pe32.th32ProcessID, pe32.cntThreads, pe32.pcPriClassBase, pe32.szExeFile);
        MessageBox(hwnd, buffer, TEXT("Информация о процессе"), MB_OK | MB_ICONINFORMATION);
        delete[] buffer;
    }
}

void CProcess_ListDlg::TerminateProcess_(HWND hwnd, int id) {
    int selectedIndex = SendMessage(hListAll, LB_GETCURSEL, 0, 0);

    if (selectedIndex != LB_ERR) {
        TCHAR processName[256];
        SendMessage(hListAll, LB_GETTEXT, selectedIndex, (LPARAM)processName);
        DWORD processID = GetProcessIDByName(processName);
        BOOL isSystemProcess = IsSystemProcess(processID);

        if (isSystemProcess) {
            MessageBox(hwnd, TEXT("Ошибка: Невозможно завершить системный процесс."), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
            return;
        }

        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
        if (hProcess == NULL) {
            MessageBox(hwnd, TEXT("Ошибка: Невозможно открыть процесс для завершения."), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
            return;
        }

        if (TerminateProcess(hProcess, 0)) {
            MessageBox(hwnd, TEXT("Процесс успешно завершен."), TEXT("Успешно"), MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBox(hwnd, TEXT("Ошибка: Невозможно завершить процесс."), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
        }

        UpdateProcessList(hwnd, id);
    }
    else {
        MessageBox(hwnd, TEXT("Ошибка: Процесс не выбран."), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
    }
}

void CProcess_ListDlg::CreateProcessFromPath(HWND hwnd, int id) {
    TCHAR filePath[MAX_PATH];
    GetDlgItemText(hwnd, IDC_EDIT_FIELD, filePath, MAX_PATH);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, filePath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        MessageBox(hwnd, TEXT("Ошибка при создании процесса. Проверьте правильность пути к файлу."), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
        return;
    }

    SetTimer(hwnd, 1, 1, NULL);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    UpdateProcessList(hListAll, IDC_BUTTON_CREATE);
    SetDlgItemText(hwnd, IDC_EDIT_FIELD, TEXT(""));
}

void CProcess_ListDlg::Cls_OnTimer(HWND hwnd, UINT id) {
    if (id == 1) {
        SetWindowText(hwnd, TEXT("Процесс успешно запущен"));
        KillTimer(hwnd, 1);
        SetTimer(hwnd, 2, 2000, NULL);
    }
    else if (id == 2) {
        SetWindowText(hwnd, TEXT("Активен список"));
        KillTimer(hwnd, 2);
    }
}

BOOL CProcess_ListDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
    SetWindowText(hwnd, TEXT("Активен список"));
    hListAll = GetDlgItem(hwnd, IDC_LIST_ALL);
    return TRUE;
}

void CProcess_ListDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
    switch (id) {
    case IDC_BUTTON_UPDATE:
        UpdateProcessList(hListAll, id);
        break;
    case IDC_BUTTON_TERMINATE:
        TerminateProcess_(hListAll, id);
        break;
    case IDC_BUTTON_INFO:
        GetProcessInfo(hListAll, id);
        break;
    case  IDC_BUTTON_CREATE:
        CreateProcessFromPath(hwnd, id);
        break;
    }
}

BOOL CALLBACK CProcess_ListDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
        HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
        HANDLE_MSG(hwnd, WM_TIMER, ptr->Cls_OnTimer);
    }
    return FALSE;
}
