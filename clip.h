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

typedef void (*ButtonCallback)(void);

typedef struct {
    char* chars;
    size_t length;
} String;

typedef struct {
    String* data;
    uint32_t id;
} ClipboardElement;

typedef struct {
    ClipboardElement* data_list;
    size_t length;
    size_t capacity;
} ClipboardList;


typedef struct {
    Sel sel;
    String text;
} ClipboardInfo;

void* xrealloc(void* ptr, size_t ptr_size);

String* retrieve_selection(Sel sel);
void write_selection(Sel sel, String str);

int clipboard_append(ClipboardList* clipboard, String* data);
String* clipboard_get(ClipboardList clipboard, size_t index);
void clipboard_print(ClipboardList clipboard);

void free_string(String** str_pointer);

#endif // CLIP_H
