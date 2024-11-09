#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct WindowInfo {
    HWND hwnd;
    std::wstring title;
    DWORD pid;
};

class ProcessSelector {
public:
    ProcessSelector();
    ~ProcessSelector();
    DWORD run();  // Метод для запуска и выбора окна

private:
    std::vector<WindowInfo> windows;
    HWND hwndListBox;
    HWND hwndButton;
    DWORD selectedPID;

    void GetWindowList();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
