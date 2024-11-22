#ifndef HYPRLAND_H
#define HYPRLAND_H

#include <sys/socket.h>

typedef struct {
    int sock;
    char sock_path[108];
} Hyprland;

typedef struct {
    int pid;
    char title[256];
} HyprlandWindow;

Hyprland hyprland_init();

HyprlandWindow *get_windows(Hyprland *hypr);

void focus_window(Hyprland *hypr, HyprlandWindow *w);

#endif //HYPRLAND_H
