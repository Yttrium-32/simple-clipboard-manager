#include "raylib/raylib.h"
#include "clip.h"
#include <stdint.h>
#include <stdlib.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"

#include <stdio.h>

const float ScreenWidth = 1408;
const float ScreenHeight = 792;

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(void) {
    Clay_Raylib_Initialize(ScreenWidth, ScreenHeight, "Simple Clipboard Manager", 0);

    uint64_t clay_mem_size = Clay_MinMemorySize();
    Clay_Arena mem_arena = {
        .memory = malloc(clay_mem_size),
        .capacity = clay_mem_size
    };

    Clay_Dimensions dimension = {
        .width = ScreenWidth,
        .height = ScreenHeight
    };

    Clay_Initialize(mem_arena, dimension, (Clay_ErrorHandler) { HandleClayErrors });
    SetTargetFPS(30);

    // main app loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(YELLOW);

        EndDrawing();
    }
    /*Sel sel = CLIPBOARD;*/
    /*char *out = retrieve_selection(sel);*/

    /*puts(out);*/

    /*return 0*/;
}

