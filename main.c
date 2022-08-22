/*******************************************************************************************
*
*   raylib demo [main.c] - tic-tac-toe
*
*   Demo originally created with raylib 4.2, last time updated with raylib 4.2
*
*   Demo licensed under MIT.
*
*   Copyright (c) 2022 Fatih S. Solmaz (@solmazfs)
*
********************************************************************************************/


#include <stdio.h>
#include "raylib.h"
// simple state machine
#include "sm_simple.h"

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

const int _SCREEN_W = 512;
const int _SCREEN_H = 512;

void UpdateDrawFrame(void);     // Update and Draw one frame

int main() {
    InitWindow(_SCREEN_W,_SCREEN_H,"tic-tac-toe");
    SetWindowState(FLAG_VSYNC_HINT);
    #if defined(PLATFORM_WEB)
        _Init();
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
        SetTargetFPS(60);

        // start here
        Color bg = {207,188,149,255};
        _Init();

        // game loop
        while (!WindowShouldClose()) {
            _Process();

            BeginDrawing();
                ClearBackground(bg);
                _Draw();
                DrawFPS(10,10);
            EndDrawing();
        }
    #endif
    CloseWindow();
    return 0;
}

void UpdateDrawFrame(void) {
    _Process();
    BeginDrawing();
        ClearBackground(RAYWHITE);
        _Draw();
    EndDrawing();
}
