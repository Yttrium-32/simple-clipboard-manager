#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define CLAY_IMPLEMENTATION

#include "raylib/raylib.h"
#include "clip.h"
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"

const float ScreenWidth = 1408;
const float ScreenHeight = 792;

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(void) {
    Sel sel = CLIPBOARD;

    char* clip_buffer[256] = {NULL};
    Clipboard clipboard = {
        .data = clip_buffer,
        .length = 0,
        .capacity = 256
    };

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

    Font fonts[1];
    fonts[0] = LoadFont("resources/Metropolis Light.ttf");
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);

    SetTargetFPS(30);

    // main app loop
    while (!WindowShouldClose()) {
        Clay_BeginLayout();
        CLAY({
                .id = CLAY_ID("MainContent"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .padding = {16, 16, 16, 16},
                    .childGap = 16,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
                },
                .backgroundColor = {18, 18, 18, 255},
                .scroll = { .vertical = true }
        }) {}
        Clay_RenderCommandArray render_commands = Clay_EndLayout();

        BeginDrawing();
        ClearBackground(YELLOW);

        // Retrieve data from selection and check if it already exists in clipboard
        char *new_clip_data = retrieve_selection(sel);
        size_t new_clip_data_len = strlen(new_clip_data);
        bool data_exists = false;
        for (size_t i = 0; i < clipboard.length; i++ ) {
            char* cur_clip_data = clipboard_get(clipboard, i);

            if (strlen(cur_clip_data) == new_clip_data_len &&
                strncmp(cur_clip_data, new_clip_data, new_clip_data_len) == 0) {
                    data_exists = true;
                    break;
            }
        }

        // If data doesn't already exist add it clipboard
        if (!data_exists)
            clipboard_append(&clipboard, new_clip_data, new_clip_data_len);

        for (size_t i = 0; i < clipboard.length; i++) {
            if (!clipboard.data[i]) {
                printf("Error: clipboard.data[%zu] is NULL\n", i);
                continue;
            }
            printf("Data %zu: %s\n", i, clipboard.data[i]);
        }
        printf("----------------------------------------------------------------> End of clipboard\n");

        Clay_Raylib_Render(render_commands, fonts);
        EndDrawing();
    }
    return 0;
}

