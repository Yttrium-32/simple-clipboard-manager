#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define CLAY_IMPLEMENTATION

#include "raylib/raylib.h"
#include "clip.h"
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"

#define CLIP_SIZE 256

const float screen_width = 1408;
const float screen_height = 792;

const uint32_t FONT_ID_BODY_16 = 0;

const Clay_Color COLOR_BLACK = (Clay_Color) { 18, 18, 18, 255 };
const Clay_Color COLOR_GRAY = (Clay_Color) { 63, 63, 63, 255 };
const Clay_Color COLOR_LIGHT_GRAY = (Clay_Color) { 87, 87, 87, 255 };
const Clay_Color COLOR_WHITE = (Clay_Color) { 255, 255, 255, 255};
const Clay_Color COLOR_PURPLE = (Clay_Color) { 208, 170, 218, 255 };

Clipboard clipboard = {
    .data = (String*[CLIP_SIZE]){NULL},
    .length = 0,
    .capacity = CLIP_SIZE
};

Texture2D copy_button;
Texture2D remove_button;

void remove_button_handler(Clay_ElementId _, Clay_PointerData pointer, intptr_t user_data) {
    if (pointer.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        ClipboardInfo *clipboard_info = (ClipboardInfo*)user_data;
        uint32_t idx = clipboard_info->cur_idx;
        printf("INFO: Removing element: %d\n", idx);

        clipboard_remove(&clipboard, idx - 1);
    }
}

void copy_button_handler(Clay_ElementId _, Clay_PointerData mouse_pointer, intptr_t user_data) {
    if (mouse_pointer.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        ClipboardInfo *clipboard_info = (ClipboardInfo*)user_data;
        printf("INFO: Copy to clipboard invoked\n");
        printf("INFO: sel = %d\n", clipboard_info->sel);
        printf("INFO: _text = %.*s\n", (int)clipboard_info->text.length, clipboard_info->text.chars);

        write_selection(clipboard_info->sel, clipboard_info->text);
    }
}

void ClipboardElement(uint8_t index, Sel sel, String _text) {
    Clay_String text = (Clay_String) {
        .length = _text.length,
        .chars = _text.chars
    };

    ClipboardInfo *clip_info_ptr = (ClipboardInfo*)malloc(sizeof(ClipboardInfo));
    *clip_info_ptr = (ClipboardInfo) {
        .sel = sel,
        .text = _text,
        .cur_idx = index
    };

    CLAY({
            .id = CLAY_IDI("ClipboardItem", index),
            .backgroundColor = COLOR_GRAY,
            .cornerRadius = CLAY_CORNER_RADIUS(5),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .padding = { 8, 8, 8, 8 },
                .childGap = 8,
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
            }
    }) {
        CLAY({
                .id = CLAY_IDI("TextItem", index),
                .backgroundColor = COLOR_GRAY,
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0) }
                }
        })
        CLAY_TEXT(
                text,
                CLAY_TEXT_CONFIG({
                    .fontSize = 24,
                    .textColor = COLOR_WHITE
                })
        );
        CLAY({
                .id = CLAY_IDI("Buttons", index),
                .backgroundColor = COLOR_GRAY,
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .padding = { 8, 8, 8, 8 },
                    .childGap = 8,
                    .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0) }
                }
        }) {
            CLAY({
                    .id = CLAY_IDI("CopyButtonBox", index),
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 4, 4, 4, 4 },
                    },
                    .backgroundColor = Clay_Hovered() ? COLOR_LIGHT_GRAY : COLOR_GRAY,
                    .cornerRadius = CLAY_CORNER_RADIUS(5),
            }) {
                Clay_OnHover(copy_button_handler, (intptr_t)clip_info_ptr);
                CLAY({
                        .id = CLAY_IDI("CopyButton", index),
                        .layout = { .sizing = { .width = CLAY_SIZING_FIXED(36), .height = CLAY_SIZING_FIXED(36)} },
                        .image = {
                            .imageData = &copy_button,
                            .sourceDimensions = { 120, 120 }
                        }
                        }){}
            }
            CLAY({
                    .id = CLAY_IDI("RemoveButtonBox", index),
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 4, 4, 4, 4 },
                    },
                    .backgroundColor = Clay_Hovered() ? COLOR_LIGHT_GRAY : COLOR_GRAY,
                    .cornerRadius = CLAY_CORNER_RADIUS(5),
            }) {
                Clay_OnHover(remove_button_handler, (intptr_t)clip_info_ptr);
                CLAY({
                        .id = CLAY_IDI("RemoveButton", index),
                        .layout = { .sizing = { .width = CLAY_SIZING_FIXED(36), .height = CLAY_SIZING_FIXED(36)} },
                        .image = {
                        .imageData = &remove_button,
                        .sourceDimensions = { 120, 120 }
                        }
                        }){}
            }
        }
    }
}

void handle_clay_errors(Clay_ErrorData errorData) {
    perror(errorData.errorText.chars);
}

int main(void) {
    Sel sel = CLIPBOARD;

    Clay_Raylib_Initialize(screen_width, screen_height, "Simple Clipboard Manager", FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);

    uint64_t clay_mem_size = Clay_MinMemorySize();
    Clay_Arena mem_arena = {
        .memory = malloc(clay_mem_size),
        .capacity = clay_mem_size
    };

    Clay_Dimensions dimension = {
        .width = screen_width,
        .height = screen_height
    };

    Clay_Initialize(mem_arena, dimension, (Clay_ErrorHandler) { handle_clay_errors });

    Font fonts[1];
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Metropolis Light.ttf", 32, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    copy_button = LoadTexture("resources/copy.png");
    remove_button = LoadTexture("resources/remove.png");

    // main app loop
    while (!WindowShouldClose()) {
        // Update layout dimension each frame to make window responsive
        Clay_SetLayoutDimensions((Clay_Dimensions) {
            .width = GetScreenWidth(),
            .height = GetScreenHeight()
        });

        // Update scroll each frame
        Vector2 mousePosition = GetMousePosition();
        Vector2 mouseWheelMove = GetMouseWheelMoveV();
        Vector2 scrollDelta = (Vector2) { .x = mouseWheelMove.x * 3, .y = mouseWheelMove.y * 3 };
        Clay_SetPointerState(
                (Clay_Vector2) { mousePosition.x, mousePosition.y },
                IsMouseButtonDown(0)
        );
        Clay_UpdateScrollContainers(
                true,
                (Clay_Vector2) { scrollDelta.x, scrollDelta.y },
                GetFrameTime()
        );

        Clay_BeginLayout();

        // Retrieve data from selection and check if it already exists in clipboard
        String* new_clip_data __attribute__((__cleanup__(free_string))) = retrieve_selection(sel);

        bool data_exists = false;
        for (size_t i = 0; i < clipboard.length; i++ ) {
            String* cur_clip_data = clipboard_get(clipboard, i);

            if (cur_clip_data->length == new_clip_data->length &&
                strncmp(cur_clip_data->chars, new_clip_data->chars, new_clip_data->length) == 0) {
                    data_exists = true;
                    break;
            }
        }

        // If data doesn't already exist, add it to the clipboard
        if (!data_exists)
            clipboard_append(&clipboard, new_clip_data);

        /*clipboard_print(clipboard);*/

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
            for (size_t i = clipboard.length - 1; i > 0; i--) {
                String* _text = clipboard_get(clipboard, i);
                ClipboardElement(i, sel, *_text);
            }
        }

        Clay_RenderCommandArray render_commands = Clay_EndLayout();

        BeginDrawing();
        ClearBackground(CLAY_COLOR_TO_RAYLIB_COLOR(COLOR_BLACK));

        Clay_Raylib_Render(render_commands, fonts);
        EndDrawing();
    }
    Clay_Raylib_Close();
}

