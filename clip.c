#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void free_string(String** str_pointer) {
    if (str_pointer && *str_pointer) {
        free((*str_pointer)->chars);
        free(*str_pointer);
    }
}

String* copy_string(String string) {
    String* new_string = (String*)malloc(sizeof(String));

    *new_string = (String) {
        .chars = malloc(string.length),
        .length = string.length
    };

    strncpy(new_string->chars, string.chars, string.length);

    return new_string;
}

void clipboard_print(Clipboard clipboard) {
    for (size_t i = 0; i < clipboard.length;i++)
        printf("%zu:\t%.*s\n", i, (int)clipboard.data[i]->length + 1, clipboard.data[i]->chars);
    printf("-----------------End of clipboard;\n\n");
}
 
int clipboard_append(Clipboard* clipboard, String* data) {
    if (clipboard->length >= clipboard->capacity) {
        for (size_t i = 0; i < clipboard->capacity - 1; i++)
            clipboard->data[i] = clipboard_get(*clipboard, i + 1);

        clipboard->data[clipboard->length - 1] = copy_string(*data);
        return 0;
    }

    clipboard->data[clipboard->length] = copy_string(*data);
    clipboard->length++;
    return 0;
}

String* clipboard_get(Clipboard clipboard, size_t index) {
    if (index >= 0 && index < clipboard.length) {
        return clipboard.data[index];
    }

    raise(SIGTRAP);
    return (String*)NULL;
}

String* retrieve_selection(Sel sel) {
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

    String* char_buf = malloc(sizeof(String));
    *char_buf = (String){
        .chars = NULL,
        .length = 0
    };

    int32_t last_idx = 0;

    FILE* fp = popen(cmd, "r");

    if (!fp) {
        int err = errno;
        fprintf(stderr, "popen() failed with %s\n", strerror(err));
        exit(EXIT_FAILURE);
    }

    // Listen indefinitely for characters until EOF
    uint32_t ch;
    while ((ch = getc(fp)) != EOF) {
        char_buf->length++;

        // Allocate space for the new character
        char_buf->chars = xrealloc(char_buf->chars, char_buf->length);

        char_buf->chars[last_idx++] = ch;
    }

    pclose(fp);

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

