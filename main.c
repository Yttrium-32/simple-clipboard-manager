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

const Clay_Color COLOR_BLACK = (Clay_Color) { 18, 18, 18, 255 };
const Clay_Color COLOR_GRAY = (Clay_Color) { 63, 63, 63, 255 };
const Clay_Color COLOR_PURPLE = (Clay_Color) { 208, 170, 218, 255 };

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(void) {
    Sel sel = CLIPBOARD;

    String* clip_buffer[256] = {NULL};
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
                .backgroundColor = COLOR_BLACK,
                .scroll = { .vertical = true }
        }) {

            CLAY({
                    .id = CLAY_ID("ClipboardItem"),
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .padding = { 8, 8, 8, 8 },
                        .childGap = 8,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }
                    },
                    .backgroundColor = COLOR_GRAY
            }) {}
        }

        Clay_RenderCommandArray render_commands = Clay_EndLayout();

        BeginDrawing();
        ClearBackground(YELLOW);

        // Retrieve data from selection and check if it already exists in clipboard
        String* new_clip_data = retrieve_selection(sel);

        bool data_exists = false;
        for (size_t i = 0; i < clipboard.length; i++ ) {
            String* cur_clip_data = clipboard_get(clipboard, i);

            if (cur_clip_data->length == new_clip_data->length &&
                strncmp(cur_clip_data->chars, new_clip_data->chars, new_clip_data->length) == 0) {
                    data_exists = true;
                    break;
            }
        }

        // If data doesn't already exist add it clipboard
        if (!data_exists)
            clipboard_append(&clipboard, new_clip_data);

        clipboard_print(clipboard);

        Clay_Raylib_Render(render_commands, fonts);
        EndDrawing();
    }
    return 0;
}

