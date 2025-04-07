#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef CLIP_H
#define CLIP_H

typedef enum {
    PRIMARY,
    SECONDARY,
    CLIPBOARD
} Sel;

typedef struct {
    char* chars;
    size_t length;
} String;

typedef struct {
    String** data;
    size_t length;
    size_t capacity;
} Clipboard;

void* xrealloc(void* ptr, size_t ptr_size);

String* retrieve_selection(Sel sel);
void write_selection(Sel sel, const char* str);

int clipboard_append(Clipboard* clipboard, String* data);
String* clipboard_get(Clipboard clipboard, size_t index);
void clipboard_print(Clipboard clipboard);

void free_string(String** str_pointer);

#endif // CLIP_H
