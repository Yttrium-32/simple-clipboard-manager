#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define CLAY_IMPLEMENTATION

#include "raylib/raylib.h"
#include "clip.h"
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector) (Clay_Vector2) { .x = vector.x, .y = vector.y }

const float ScreenWidth = 1408;
const float ScreenHeight = 792;

const uint32_t FONT_ID_BODY_16 = 0;

const Clay_Color COLOR_BLACK = (Clay_Color) { 18, 18, 18, 255 };
const Clay_Color COLOR_GRAY = (Clay_Color) { 63, 63, 63, 255 };
const Clay_Color COLOR_PURPLE = (Clay_Color) { 208, 170, 218, 255 };

void ClipboardElement(uint8_t index, Clay_String text) {
    CLAY({
            .id = CLAY_IDI("ClipboardItem", index),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .padding = { 8, 8, 8, 8 },
                .childGap = 8,
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT() }
            },
            .backgroundColor = COLOR_GRAY,
            .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        CLAY_TEXT(
                text,
                CLAY_TEXT_CONFIG({
                    .fontSize = 24,
                    .textColor = { 255,255,255,255 }
                })
        );
    }
}


Clay_RenderCommandArray CreateLayout(Sel sel, Clipboard clipboard) {
    Clay_BeginLayout();
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

    CLAY({
            .id = CLAY_ID("OuterContainer"),
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                .padding = { 16, 16, 16, 16 }, .childGap = 16
            },
            .backgroundColor = COLOR_BLACK
    }) {
        CLAY({
                .id = CLAY_ID("MainContent"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .padding = {16, 16, 16, 16},
                    .childGap = 16,
                    .sizing = { .width = CLAY_SIZING_GROW(0) } },
                .backgroundColor = COLOR_BLACK,
                .scroll = { .vertical = true },
        }) {
            for (size_t i = 0; i < clipboard.length; i++) {
                Clay_String text = (Clay_String) {
                    .length = clipboard.data[i]->length,
                    .chars = clipboard.data[i]->chars
                };
                ClipboardElement(i, text);
            }
        }

        Clay_ScrollContainerData scrollData = Clay_GetScrollContainerData(Clay_GetElementId(CLAY_STRING("MainContent")));
        if (scrollData.found) {
            CLAY({
                    .id = CLAY_ID("ScrollBar"),
                    .floating = {
                        .attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID,
                        .offset = { .y = -(scrollData.scrollPosition->y / scrollData.contentDimensions.height) * scrollData.scrollContainerDimensions.height },
                        .zIndex = 1,
                        .parentId = Clay_GetElementId(CLAY_STRING("MainContent")).id,
                        .attachPoints = { .element = CLAY_ATTACH_POINT_RIGHT_TOP, .parent = CLAY_ATTACH_POINT_RIGHT_TOP }
                    }
            }) {
                CLAY({ .id = CLAY_ID("ScrollBarButton"),
                    .layout = {
                        .sizing = {CLAY_SIZING_FIXED(12), CLAY_SIZING_FIXED((scrollData.scrollContainerDimensions.height / scrollData.contentDimensions.height) * scrollData.scrollContainerDimensions.height) }
                    },
                    .backgroundColor = Clay_PointerOver(Clay__HashString(CLAY_STRING("ScrollBar"), 0, 0)) ? (Clay_Color){100, 100, 140, 150} : (Clay_Color){120, 120, 160, 150},
                    .cornerRadius = CLAY_CORNER_RADIUS(6)
                }) {}
            }
        }
    }
    return Clay_EndLayout();
}

typedef struct {
    Clay_Vector2 clickOrigin;
    Clay_Vector2 positionOrigin;
    bool mouseDown;
} ScrollbarData;

ScrollbarData scrollbarData = {0};

void UpdateDrawFrame(Font* fonts, Sel sel, Clipboard clipboard) {
    Vector2 mouseWheelDelta = GetMouseWheelMoveV();
    float mouseWheelX = mouseWheelDelta.x;
    float mouseWheelY = mouseWheelDelta.y;

    //----------------------------------------------------------------------------------
    // Handle scroll containers
    Clay_Vector2 mousePosition = RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition());
    Clay_SetPointerState(mousePosition, IsMouseButtonDown(0) && !scrollbarData.mouseDown);
    Clay_SetLayoutDimensions((Clay_Dimensions) { (float)GetScreenWidth(), (float)GetScreenHeight() });
    if (!IsMouseButtonDown(0)) {
        scrollbarData.mouseDown = false;
    }

    if (IsMouseButtonDown(0) && !scrollbarData.mouseDown && Clay_PointerOver(Clay__HashString(CLAY_STRING("ScrollBar"), 0, 0))) {
        Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay__HashString(CLAY_STRING("MainContent"), 0, 0));
        scrollbarData.clickOrigin = mousePosition;
        scrollbarData.positionOrigin = *scrollContainerData.scrollPosition;
        scrollbarData.mouseDown = true;
    } else if (scrollbarData.mouseDown) {
        Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay__HashString(CLAY_STRING("MainContent"), 0, 0));
        if (scrollContainerData.contentDimensions.height > 0) {
            Clay_Vector2 ratio = (Clay_Vector2) {
                scrollContainerData.contentDimensions.width / scrollContainerData.scrollContainerDimensions.width,
                scrollContainerData.contentDimensions.height / scrollContainerData.scrollContainerDimensions.height,
            };

            if (scrollContainerData.config.vertical)
                scrollContainerData.scrollPosition->y = scrollbarData.positionOrigin.y + (scrollbarData.clickOrigin.y - mousePosition.y) * ratio.y;

            if (scrollContainerData.config.horizontal)
                scrollContainerData.scrollPosition->x = scrollbarData.positionOrigin.x + (scrollbarData.clickOrigin.x - mousePosition.x) * ratio.x;

        }
    }

    Clay_UpdateScrollContainers(true, (Clay_Vector2) {mouseWheelX, mouseWheelY}, GetFrameTime());
    // Generate the auto layout for rendering
    double currentTime = GetTime();
    Clay_RenderCommandArray renderCommands = CreateLayout(sel, clipboard);
    printf("layout time: %f microseconds\n", (GetTime() - currentTime) * 1000 * 1000);
    // RENDERING ---------------------------------
//    currentTime = GetTime();
    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();
//    printf("render time: %f ms\n", (GetTime() - currentTime) * 1000);

    //----------------------------------------------------------------------------------
}

bool reinitializeClay = false;

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
    if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
        reinitializeClay = true;
        Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
    } else if (errorData.errorType == CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
        reinitializeClay = true;
        Clay_SetMaxMeasureTextCacheWordCount(Clay_GetMaxMeasureTextCacheWordCount() * 2);
    }
}

int main(void) {
    Sel sel = CLIPBOARD;

    String* clip_buffer[256] = {NULL};
    Clipboard clipboard = {
        .data = clip_buffer,
        .length = 0,
        .capacity = 256
    };

    Clay_Dimensions dimension = {
        .width = ScreenWidth,
        .height = ScreenHeight
    };

    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
    Clay_Initialize(clayMemory, dimension, (Clay_ErrorHandler) { HandleClayErrors, 0 });
    Clay_Raylib_Initialize(ScreenWidth, ScreenHeight, "Simple Clipboard Manager", 0);

    Font fonts[1];
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Metropolis Light.ttf", 32, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    SetTargetFPS(60);

    // main app loop
    while (!WindowShouldClose()) {
        if (reinitializeClay) {
            Clay_SetMaxElementCount(8192);
            totalMemorySize = Clay_MinMemorySize();
            clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
            Clay_Initialize(clayMemory, (Clay_Dimensions) { (float)GetScreenWidth(), (float)GetScreenHeight() }, (Clay_ErrorHandler) { HandleClayErrors, 0 });
            reinitializeClay = false;
        }
        UpdateDrawFrame(fonts, sel, clipboard);
    }
    return 0;
}

