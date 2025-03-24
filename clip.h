#include <stdlib.h>

#ifndef CLIP_H
#define CLIP_H

typedef enum {
    PRIMARY,
    SECONDARY,
    CLIPBOARD
} Sel;

typedef struct {
    char** data;
    size_t length;
    size_t capacity;
} Clipboard;

void* xrealloc(void* ptr, size_t ptr_size);
char* retrieve_selection(Sel sel);
void write_selection(Sel sel, const char* str);
int clipboard_append(Clipboard* clipboard, char* data, size_t data_len);
char* clipboard_get(Clipboard clipboard, size_t index);
void clipboard_print(Clipboard clipboard);

#endif // CLIP_H
