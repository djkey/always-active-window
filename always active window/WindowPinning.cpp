#include "WindowPinning.h"

WindowPinning::WindowPinning(const std::string& processName) : targetProcessName(processName), targetPid(0) {}

WindowPinning::WindowPinning(DWORD pid) : targetProcessName(""), targetPid(pid) {}

void WindowPinning::StartPinning() {
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));

    while (true) {
        CheckWindowState();
        Sleep(500);

        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_CLOSE) {
                RemoveWindowFromTop(msg.hwnd);
            }
        }
    }
}

void WindowPinning::StopPinning() {
    topCount = 0;  
}

WindowPinning::~WindowPinning() {
    RemoveAllTopWindows();
}

bool WindowPinning::GetProcessName(DWORD pid, string& processName) {
    wchar_t buffer[MAX_PATH];  
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
        if (K32GetModuleFileNameExW(hProcess, NULL, buffer, MAX_PATH)) {
            processName = std::string(buffer, buffer + wcslen(buffer));
            CloseHandle(hProcess);
            return true;
        }
        CloseHandle(hProcess);
    }
    return false;
}

BOOL CALLBACK WindowPinning::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    WindowPinning* pinning = reinterpret_cast<WindowPinning*>(lParam);

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    string processName;
    if (pinning) {
        bool shouldPin = false;
        if (pinning->targetPid != 0) {
            if (pid == pinning->targetPid) {
                shouldPin = true;
            }
        }
        else if (pinning->GetProcessName(pid, processName)) {
            if (processName.find(pinning->targetProcessName) != string::npos) {
                shouldPin = true;
            }
        }

        if (shouldPin) {
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            ShowWindow(hwnd, SW_RESTORE);
            BringWindowToTop(hwnd);
            SetForegroundWindow(hwnd);
            pinning->topWindow[pinning->topCount] = hwnd;
            pinning->topCount++;
            return FALSE;
        }
    }
    return TRUE;
}
void WindowPinning::CheckWindowState() {
    for (int i = 0; i < topCount; i++) {
        HWND hwnd = topWindow[i];

        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        }
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetForegroundWindow(hwnd);
    }
}

void WindowPinning::RemoveWindowFromTop(HWND hwnd) {
    for (int i = 0; i < topCount; i++) {
        if (topWindow[i] == hwnd) {
            topWindow[i] = topWindow[topCount - 1];
            topCount--;
            break;
        }
    }
}

void WindowPinning::RemoveAllTopWindows() {
    for (int i = 0; i < topCount; i++) {
        HWND hwnd = topWindow[i];
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}