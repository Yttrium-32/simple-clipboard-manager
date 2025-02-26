#include "raylib/raylib.h"
#include "clip.h"

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"

#include <stdio.h>

int main(void) {
    Sel sel = CLIPBOARD;
    char *out = retrieve_selection(sel);

    puts(out);

    return 0;
}

