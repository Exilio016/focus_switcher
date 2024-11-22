#include "hyprland.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define BFUTILS_VECTOR_IMPLEMENTATION
#include "bfutils_vector.h"
#include <stdio.h>

typedef struct {
    Hyprland *hypr;
    HyprlandWindow *windows;
    char search[128];
    char old_search[128];
    char *titles;
    int active;
    size_t listSize;
    int exit;
} State;

char *calculate_titles(HyprlandWindow *windows, char *titles){
    if (titles != NULL) {
        vector_header(titles)->length = 0;
    }
    for(int i = 0; i < vector_length(windows); i++) {
        string_push_cstr(titles, windows[i].title);
        vector_push(titles, ';');
    }
    titles[vector_length(titles) - 1] = '\0';
    return titles;
}

void handle_inputs(State *state) {
    if (IsKeyPressed(KEY_DOWN) && state->active < (state->listSize - 1)) {
        state->active++;
    }
    if (IsKeyPressed(KEY_UP) && state->active > 0) {
        state->active--;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        focus_window(state->hypr, state->windows + state->active);
        state->exit = 1;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        state->exit = 1;
    }
    if (0 != strcmp(state->search, state->old_search)){
        strcpy(state->old_search, state->search);
        search_window(state->windows, state->search);
        state->titles = calculate_titles(state->windows, state->titles);
    }
}

void update_draw_frame(State *state) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Focus Switcher", 10, 10, 20, DARKGRAY);
        Rectangle searchRect = {10, 40, 780, 30};
        GuiTextBox(searchRect, state->search, 128, 1);
        Rectangle listRect = {10, 80, 780, 350};
        GuiListView(listRect, state->titles, NULL, &state->active);
    EndDrawing();
}


int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    Hyprland hypr =  hyprland_init();
    HyprlandWindow *windows = get_windows(&hypr);
    char *titles = calculate_titles(windows, NULL);
    State state = {.titles = titles, .listSize = vector_length(windows), .hypr = &hypr, .windows = windows };

    InitWindow(screenWidth, screenHeight, "Focus Switcher");
    SetTargetFPS(60); 
    while (!state.exit) {
        handle_inputs(&state);
        update_draw_frame(&state);
    }

    CloseWindow();

    vector_free(titles);
    vector_free(windows);
    return 0;
}

