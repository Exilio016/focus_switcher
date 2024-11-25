#include <windows.h>
#include "window_manager.h"
#include "bfutils_vector.h"

static BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lparam) {
    WmWindow **list_addr = (WmWindow**) lparam;
    WmWindow w = {.pid = (size_t) hWnd};
    if (IsWindowVisible(hWnd)) {
        GetWindowText(hWnd, w.title, 256);
        vector_push(*list_addr, w);
    }

    return TRUE;
}

WindowManager window_manager_init() {
    return (WindowManager) {0};
}

WmWindow *get_windows(WindowManager *w) {
    WmWindow *list = NULL;
    EnumWindows(enumWindowCallback, (LPARAM) &list);
    return list;
}

void focus_window(WindowManager *hypr, WmWindow *w) {
    HWND hWnd = (HWND) w->pid;
    SetForegroundWindow(hWnd);
}
