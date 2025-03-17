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
    size_t len;
} Clipboard;

void* xrealloc(void* ptr, size_t ptr_size);
char* retrieve_selection(Sel sel);
void write_selection(Sel sel, const char* str);
void append(char*** arr, char* data, size_t* arr_len, size_t data_len);

#endif // CLIP_H
