#include "bfutils_vector.h"
#include "window_manager.h"
#include "fuzzy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

char *read_from_socket(int sock){
    char buf[512];
    char *res = NULL;

    int size = 0;
    while((size = read(sock, buf, 512)) > 0) {
        for(int i = 0; i < size; i++) {
            vector_push(res, buf[i]);
        }
    }
    vector_push(res, '\0');
    return res;
}

void reconnect(WindowManager *hypr) {
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, hypr->sock_path);

    hypr->sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (hypr->sock < 0) {
        perror("socket");
        exit(1);
    }

    if (connect(hypr->sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }
}

WindowManager window_manager_init() {
    char *xdg_runtime_dir = getenv("XDG_RUNTIME_DIR"); 
    char *his = getenv("HYPRLAND_INSTANCE_SIGNATURE");

    WindowManager hypr =  {0};
    snprintf(hypr.sock_path, 108, "%s/hypr/%s/.socket.sock", xdg_runtime_dir, his);

    return hypr;
}

WmWindow *get_windows(WindowManager *hypr){
    WmWindow *list = NULL;
    reconnect(hypr);

    char *command = "/clients";
    if (write(hypr->sock, command, strlen(command)) < 0) {
        perror("write");
    }

    char *res = read_from_socket(hypr->sock);
    char *line = strtok(res, "\n");
    char *title_prefix =  "\ttitle: ";
    char *pid_prefix = "\tpid:";
    char *window_prefix = "Window ";
    WmWindow window = {0};
    int need_to_push = 0;

    while (line != NULL) {
        if (0 == strncmp(line, title_prefix, strlen(title_prefix))) {
            strncpy(window.title, (line + strlen(title_prefix)), 255);
        }
        else if (0 == strncmp(line, pid_prefix, strlen(pid_prefix))) {
            char *pid_str = malloc(strlen(line) - strlen(pid_prefix) + 1);
            strcpy(pid_str, (line + strlen(pid_prefix)));
            window.pid = atoi(pid_str);
            free(pid_str);
        }
        else if (0 == strncmp(line, window_prefix, strlen(window_prefix))) {
            if (need_to_push) {
                vector_push(list, window);
                window = (WmWindow) {0};
            }
            need_to_push = 1;
        }
        line = strtok(NULL, "\n");
    }
    vector_push(list, window);

    close(hypr->sock);
    vector_free(res);
    return list;
}

void focus_window(WindowManager *hypr, WmWindow *w) {
    char command[256];
    reconnect(hypr);

    snprintf(command, 255, "/dispatch focuswindow pid:%d", w->pid);
    if(write(hypr->sock, command, strlen(command)) < 0) {
        perror("write");
    }
    char *res = read_from_socket(hypr->sock);
    close(hypr->sock);
    vector_free(res);
}

