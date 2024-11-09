#include "WindowPinning.h"

WindowPinning::WindowPinning(const std::string& processName) : targetProcessName(processName), targetPid(0) {}
// Конструктор для использования PID процесса

WindowPinning::WindowPinning(DWORD pid) : targetProcessName(""), targetPid(pid) {}

// Метод для начала закрепления окна
void WindowPinning::StartPinning() {
    // Перебираем все окна и закрепляем те, которые соответствуют имени процесса
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));

    // Бесконечный цикл, который будет постоянно проверять окна
    while (true) {
        // Проверяем состояние закрепленных окон
        CheckWindowState();

        // Пауза, чтобы не перегружать процессор
        Sleep(500);

        // Получаем события окон (например, окно может быть закрыто)
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_CLOSE) {
                // Если окно закрыто, удаляем его из списка закрепленных
                RemoveWindowFromTop(msg.hwnd);
            }
        }
    }
}

// Метод для остановки закрепления (в данном случае просто сбросим список окон)
void WindowPinning::StopPinning() {
    topCount = 0;  // Очищаем список закрепленных окон
}

WindowPinning::~WindowPinning() {
    // Снимаем закрепление с всех окон
    RemoveAllTopWindows();
}

// Функция для получения имени процесса по PID
bool WindowPinning::GetProcessName(DWORD pid, string& processName) {
    wchar_t buffer[MAX_PATH];  // Используем wchar_t для Unicode строк
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
        if (K32GetModuleFileNameExW(hProcess, NULL, buffer, MAX_PATH)) {
            // Преобразуем wchar_t в string
            processName = std::string(buffer, buffer + wcslen(buffer));
            CloseHandle(hProcess);
            return true;
        }
        CloseHandle(hProcess);
    }
    return false;
}

// Метод для перебора всех окон
BOOL CALLBACK WindowPinning::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    WindowPinning* pinning = reinterpret_cast<WindowPinning*>(lParam);  // Получаем объект WindowPinning

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);  // Получаем PID окна

    string processName;
    if (pinning) {
        bool shouldPin = false;
        if (pinning->targetPid != 0) {
            if (pid == pinning->targetPid) {  // Если PID совпадает
                shouldPin = true;
            }
        }
        else if (pinning->GetProcessName(pid, processName)) {
            if (processName.find(pinning->targetProcessName) != string::npos) {  // Если имя процесса совпадает
                shouldPin = true;
            }
        }

        if (shouldPin) {
            // Окно найдено, закрепляем его
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

            // Пытаемся показать окно, если оно скрыто
            ShowWindow(hwnd, SW_RESTORE);

            // Приводим окно на передний план
            BringWindowToTop(hwnd);

            // Возвращаем фокус окну
            SetForegroundWindow(hwnd);

            // Сохраняем окно в список закрепленных
            pinning->topWindow[pinning->topCount] = hwnd;
            pinning->topCount++;
            return FALSE;  // Останавливаем перебор окон
        }
    }
    return TRUE;  // Продолжаем перебор окон
}
// Метод для проверки состояния закрепленных окон
void WindowPinning::CheckWindowState() {
    for (int i = 0; i < topCount; i++) {
        HWND hwnd = topWindow[i];

        // Проверка, свернуто ли окно
        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);  // Восстанавливаем окно, если оно свернуто
        }

        // Проверка, чтобы окно оставалось поверх
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        // Проверка, чтобы окно было активным
        SetForegroundWindow(hwnd);
    }
}

// Метод для удаления окна из списка закрепленных
void WindowPinning::RemoveWindowFromTop(HWND hwnd) {
    // Убираем окно из списка закрепленных
    for (int i = 0; i < topCount; i++) {
        if (topWindow[i] == hwnd) {
            topWindow[i] = topWindow[topCount - 1];  // Перемещаем последнее окно на место удаленного
            topCount--;
            break;
        }
    }
}

// Метод для снятия закрепления с всех окон
void WindowPinning::RemoveAllTopWindows() {
    for (int i = 0; i < topCount; i++) {
        HWND hwnd = topWindow[i];
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);  // Снимаем закрепление с окна
    }
}