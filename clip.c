#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

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

void clipboard_print(ClipboardList clipboard) {
    for (size_t i = 0; i < clipboard.length; i++) {
        ClipboardElement item = clipboard.data_list[i];
        printf("%zu:\t%.*s\n", i, (int)item.data->length, item.data->chars);
    }

    printf("-----------------End of clipboard;\n\n");
}
 
int clipboard_append(ClipboardList* clipboard, String* data) {
    if (clipboard->length >= clipboard->capacity) {
        for (size_t i = 0; i < clipboard->capacity - 1; i++)
            clipboard->data_list[i].data = clipboard_get(*clipboard, i + 1);

        clipboard->data_list[clipboard->length - 1].data = copy_string(*data);
        return 0;
    }

    clipboard->data_list[clipboard->length].data = copy_string(*data);
    clipboard->length++;
    return 0;
}

String* clipboard_get(ClipboardList clipboard, size_t index) {
    if (index >= 0 && index < clipboard.length)
        return clipboard.data_list[index].data;

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
        fprintf(stderr, "ERR: popen() failed with %s\n", strerror(err));
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

void write_selection(Sel sel, String str) {
    char* sel_str_arr[] = {"primary", "secondary", "clipboard"};
    const char* selection = sel_str_arr[sel];

    int32_t fildes[2];
    if (pipe(fildes) == -1) {
        int32_t err = errno;
        fprintf(stderr, "ERR: pipe failed with: %s\n", strerror(err));
        return;
    }

    switch (fork()) {
        case -1: {
            int32_t err = errno;
            fprintf(stderr, "ERR: fork failed with: %s\n", strerror(err));
            close(fildes[0]);
            close(fildes[1]);
            return;
        }
        case 0: {
            close(fildes[1]);
            dup2(fildes[0], STDIN_FILENO);
            close(fildes[0]);
            execlp("xclip", "xclip", "-selection", selection, NULL);

            int32_t err = errno;
            fprintf(stderr, "ERR: execlp failed with: %s\n", strerror(err));
            _exit(127);
        }

        default: {
            close(fildes[0]);

            if (write(fildes[1], str.chars, str.length) < 0) {
                int32_t err = errno;
                fprintf(stderr, "ERR: write failed with: %s\n", strerror(err));
            }

            close(fildes[1]);
            int32_t status;
            if (wait(&status) == -1) {
                int32_t err = errno;
                fprintf(stderr, "ERR wait failed with: %s\n", strerror(err));
            } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "ERR: xclip exited with code %d\n", WEXITSTATUS(status));
            }
            break;
        }
    }
}

