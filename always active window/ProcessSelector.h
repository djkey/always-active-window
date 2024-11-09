#pragma once
#include <windows.h>
#include <string>
#include <vector>

// Структура для хранения информации об окне: дескриптор, заголовок и PID
struct WindowInfo {
    HWND hwnd;           // Дескриптор окна
    std::wstring title;  // Заголовок окна
    DWORD pid;           // PID процесса
};

class ProcessSelector {
public:
    // Конструктор класса, инициализирует компоненты
    ProcessSelector();

    // Деструктор класса, освобождает ресурсы
    ~ProcessSelector();

    // Метод для запуска окна выбора процесса и возврата выбранного PID
    DWORD run();
private:
    std::vector<WindowInfo> windows;  // Вектор для хранения списка окон
    HWND hwndListBox;                 // Дескриптор ListBox для отображения окон
    HWND hwndButton;                  // Дескриптор кнопки "OK"
    DWORD selectedPID;                // Выбранный PID

    // Метод для заполнения списка окон в системе
    void GetWindowList();

    // Статическая функция для обработки сообщений окна
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
