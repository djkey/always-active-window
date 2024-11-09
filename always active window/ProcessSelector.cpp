#include "ProcessSelector.h"
#include <windows.h>
#include <string>
#include <vector>
#include <commctrl.h> // Для работы с ListBox

// Конструктор
ProcessSelector::ProcessSelector() : hwndListBox(NULL), hwndButton(NULL), selectedPID(0) {
    // Инициализация окна
    GetWindowList();
}

// Деструктор
ProcessSelector::~ProcessSelector() {
    // Освобождение ресурсов
}

// Метод получения списка окон
void ProcessSelector::GetWindowList() {
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        ProcessSelector* selector = reinterpret_cast<ProcessSelector*>(lParam);

        // Проверка, является ли окно видимым (не скрыто)
        if (IsWindowVisible(hwnd)) {
            wchar_t title[256];
            // Получаем заголовок окна
            if (GetWindowText(hwnd, title, sizeof(title) / sizeof(title[0])) > 0) {
                DWORD pid;
                GetWindowThreadProcessId(hwnd, &pid);

                // Добавляем окно в список
                selector->windows.push_back({ hwnd, title, pid });
            }
        }

        return TRUE; // Продолжаем перечисление окон
        }, reinterpret_cast<LPARAM>(this));
}

// Метод для создания и отображения окна с процессами
DWORD ProcessSelector::run() {
    // Регистрируем класс окна
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ProcessSelectorWindow";
    RegisterClass(&wc);

    // Создаем окно
    HWND hwnd = CreateWindowEx(0, L"ProcessSelectorWindow", L"Select Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, wc.hInstance, this);

    // Создаем ListBox
    hwndListBox = CreateWindowEx(0, WC_LISTBOX, NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL,
        20, 20, 350, 200, hwnd, NULL, wc.hInstance, NULL);

    // Добавляем элементы в ListBox
    for (const auto& window : windows) {
        // Добавляем только видимые окна с текстом
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)window.title.c_str());
    }

    // Создаем кнопку "OK"
    hwndButton = CreateWindowEx(0, L"BUTTON", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 230, 100, 30, hwnd, (HMENU)1, wc.hInstance, NULL);

    // Отображаем окно
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Запускаем цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DestroyWindow(hwnd);
    return selectedPID;  // Возвращаем PID выбранного процесса
}

// Процедура обработки сообщений для окна
LRESULT CALLBACK ProcessSelector::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ProcessSelector* selector;

    if (uMsg == WM_CREATE) {
        selector = reinterpret_cast<ProcessSelector*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)selector);
    }
    else {
        selector = reinterpret_cast<ProcessSelector*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    switch (uMsg) {
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) {  // Если нажата кнопка "OK"
            int selectedIndex = SendMessage(selector->hwndListBox, LB_GETCURSEL, 0, 0);
            if (selectedIndex != LB_ERR) {
                selector->selectedPID = selector->windows[selectedIndex].pid;
                PostMessage(hwnd, WM_CLOSE, 0, 0); // Закрываем окно после выбора
            }
        }
        break;
    }
    case WM_CLOSE:
        PostQuitMessage(0); // Закрытие окна
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
