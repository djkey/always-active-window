#ifndef WINDOWPINNING_H
#define WINDOWPINNING_H

#include <windows.h>
#include <string>
#include <iostream>
#include <psapi.h>

using namespace std;

class WindowPinning {
public:
    // ����������� ������, �������������� ��� ��������
    WindowPinning(const std::string& processName);

    // ����������� ������, �������������� pid ��������
    WindowPinning(DWORD pid);
    
    // ���������� ������
    ~WindowPinning();

    // ����� ��� ������ ����������� ����
    void StartPinning();

    // ����� ��� ��������� �����������
    void StopPinning();

private:
    // ������� ��� ��������� ����� �������� �� PID
    bool GetProcessName(DWORD pid, string& processName);

    // ����� ��� �������� ���� ����
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

    // ����� ��� �������� ��������� ������������ ����
    void CheckWindowState();

    // ����� ��� �������� ���� �� ������ ������������
    void RemoveWindowFromTop(HWND hwnd);

    // ����� ��� ������ ����������� � ���� ����
    void RemoveAllTopWindows();

    string targetProcessName;   // ��� �������� ��������
    DWORD targetPid = 0;        // PID �������� ��������
    HWND topWindow[128];        // ������ ��� �������� ������������ ����
    int topCount = 0;           // ������� ������������ ����
};

#endif // WINDOWPINNING_H
