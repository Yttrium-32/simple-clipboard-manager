#define _GNU_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "clip.h"

// Reallocate or die
void* xrealloc(void* ptr, size_t ptr_size) {
    void* tmp = realloc(ptr, ptr_size);
    if (tmp == NULL) {
        // Realloc failed, kill program
        free(ptr);
        exit(EXIT_FAILURE);
    } else
        return tmp;
}

char* copy_string(char* data, size_t data_len) {
    char* new_string = strndup(data, data_len);
    if (new_string == NULL) {
        perror("Failed to copy string!");
        exit(EXIT_FAILURE);
    }
    return new_string;
}

void clipboard_print(Clipboard clipboard) {
    for (size_t i = 0; i < clipboard.length;i++)
        printf("%s\n", clipboard.data[i]);
    printf("-----------------End of clipboard;\n\n");
}
 
int clipboard_append(Clipboard* clipboard, char* data, size_t data_len) {
    if (clipboard->length >= clipboard->capacity) {
        for (size_t i = 0; i < clipboard->capacity - 1; i++)
            clipboard->data[i] = clipboard_get(*clipboard, i + 1);

        clipboard->data[clipboard->length - 1] = copy_string(data, data_len);
        return 0;
    }

    clipboard->data[clipboard->length] = copy_string(data, data_len);
    clipboard->length++;
    return 0;
}

char* clipboard_get(Clipboard clipboard, size_t index) {
    if (index >= 0 && index < clipboard.length) {
        return clipboard.data[index];
    }

    raise(SIGTRAP);
    return "\0";
}

char* retrieve_selection(Sel sel) {
    char* cmd;

    switch (sel) {
        case PRIMARY:
            cmd = "xclip -selection p -o";
            break;
        case SECONDARY:
            cmd = "xclip -selection s -o";
            break;
        case CLIPBOARD:
        default:
            cmd = "xclip -selection c -o";
    }

    int32_t ch;
    char *char_buf = NULL;
    size_t out_size = 0;
    int32_t last_idx = 0;

    FILE* fp = popen(cmd, "r");

    // Listen indefinitely for characters until EOF
    while ((ch = getc(fp)) != EOF) {
        // The size of buffer required to hold text
        out_size++;

        // Allocate space for the new character
        char_buf = xrealloc(char_buf, out_size);

        char_buf[last_idx++] = ch;
    }

    // Strings are always null terminated
    // Allocate space for \0
    char_buf = xrealloc(char_buf, out_size + 1);
    char_buf[last_idx] = '\0';
    return char_buf;
}

void write_selection(Sel sel, const char* str) {
    char* cmd;

    switch (sel) {
        case PRIMARY:
            asprintf(&cmd, "echo -n %s | xclip -selection p", str);
            break;
        case SECONDARY:
            asprintf(&cmd, "echo -n %s | xclip -selection s", str);
            break;
        case CLIPBOARD:
        default:
            asprintf(&cmd, "echo -n %s | xclip -selection c", str);
    }

    if (system(cmd) != 0) {
        perror("Write to selection failed!");
    };
}

