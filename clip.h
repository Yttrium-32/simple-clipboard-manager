#include <stdlib.h>

#ifndef CLIP_H
#define CLIP_H

typedef enum {
    PRIMARY,
    SECONDARY,
    CLIPBOARD
} Sel;

void* xrealloc(void* ptr, size_t ptr_size);
char* retrieve_selection(Sel sel);
void write_selection(Sel sel, const char* str);

#endif // CLIP_H
