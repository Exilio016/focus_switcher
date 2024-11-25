#ifndef HYPRLAND_H
#define HYPRLAND_H

typedef struct {
    int sock;
    char sock_path[108];
} WindowManager;

typedef struct {
    int pid;
    char title[256];
} WmWindow;

WindowManager window_manager_init();

WmWindow *get_windows(WindowManager *hypr);

void focus_window(WindowManager *hypr, WmWindow *w);


#endif //HYPRLAND_H
