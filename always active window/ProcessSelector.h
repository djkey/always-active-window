#pragma once
#include <windows.h>
#include <string>
#include <vector>

// ��������� ��� �������� ���������� �� ����: ����������, ��������� � PID
struct WindowInfo {
    HWND hwnd;           // ���������� ����
    std::wstring title;  // ��������� ����
    DWORD pid;           // PID ��������
};

class ProcessSelector {
public:
    // ����������� ������, �������������� ����������
    ProcessSelector();

    // ���������� ������, ����������� �������
    ~ProcessSelector();

    // ����� ��� ������� ���� ������ �������� � �������� ���������� PID
    DWORD run();
private:
    std::vector<WindowInfo> windows;  // ������ ��� �������� ������ ����
    HWND hwndListBox;                 // ���������� ListBox ��� ����������� ����
    HWND hwndButton;                  // ���������� ������ "OK"
    DWORD selectedPID;                // ��������� PID

    // ����� ��� ���������� ������ ���� � �������
    void GetWindowList();

    // ����������� ������� ��� ��������� ��������� ����
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
