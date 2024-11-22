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
    char *titles;
    int active;
    size_t listSize;
    int exit;
} State;

void HandleInputs(State *state) {
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
}

void UpdateDrawFrame(State *state) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Focus Switcher", 10, 10, 20, DARKGRAY);
        Rectangle listRect = (Rectangle) {10, 50, 780, 390};
        GuiListView(listRect, state->titles, NULL, &state->active);
    EndDrawing();
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Focus Switcher");
    Hyprland hypr =  hyprland_init();
    HyprlandWindow *windows = get_windows(&hypr);
    char *titles = NULL;
    for(int i = 0; i < vector_length(windows); i++) {
        string_push_cstr(titles, windows[i].title);
        vector_push(titles, ';');
    }
    titles[vector_length(titles) - 1] = '\0';
    State state = {.titles = titles, .listSize = vector_length(windows), .hypr = &hypr, .windows = windows };

    SetTargetFPS(60); 
    while (!state.exit) {
        HandleInputs(&state);
        UpdateDrawFrame(&state);
    }

    CloseWindow();

    vector_free(titles);
    vector_free(windows);
    return 0;
}

