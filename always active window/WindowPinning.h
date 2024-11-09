#ifndef WINDOWPINNING_H
#define WINDOWPINNING_H

#include <windows.h>
#include <string>
#include <iostream>
#include <psapi.h>

using namespace std;

class WindowPinning {
public:
    // Конструктор класса, инициализирует имя процесса
    WindowPinning(const std::string& processName);

    // Конструктор класса, инициализирует pid процесса
    WindowPinning(DWORD pid);
    
    // Деструктор класса
    ~WindowPinning();

    // Метод для начала закрепления окна
    void StartPinning();

    // Метод для остановки закрепления
    void StopPinning();

private:
    // Функция для получения имени процесса по PID
    bool GetProcessName(DWORD pid, string& processName);

    // Метод для перебора всех окон
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

    // Метод для проверки состояния закрепленных окон
    void CheckWindowState();

    // Метод для удаления окна из списка закрепленных
    void RemoveWindowFromTop(HWND hwnd);

    // Метод для снятия закрепления с всех окон
    void RemoveAllTopWindows();

    string targetProcessName;   // Имя целевого процесса
    DWORD targetPid = 0;        // PID целевого процесса
    HWND topWindow[128];        // Массив для хранения закрепленных окон
    int topCount = 0;           // Счётчик закрепленных окон
};

#endif // WINDOWPINNING_H
