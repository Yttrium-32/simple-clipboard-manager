#include <stdio.h>
#include <stdlib.h>

typedef enum {
    PRIMARY,
    SECONDARY,
    CLIPBOARD
} Sel;

// Reallocate or die
void* xrealloc(void* ptr, size_t ptr_size) {
    void* tmp = realloc(ptr, ptr_size);
    if (tmp == NULL) {
        // Realloc failed, kill program
        free(ptr);
        exit(EXIT_FAILURE);
    } else return tmp;
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
    char buffer;
    char *buf = NULL;
    size_t out_size = 0;
    int last_idx = 0;

    FILE* fp = popen(cmd, "r");

    // Listen indefinitely for characters until EOF
    while ((buffer = getc(fp)) != EOF) {
        // The size of buffer required to hold text
        out_size += sizeof(char);

        // Allocate space for the new character
        buf = xrealloc(buf, out_size);

        buf[last_idx++] = buffer;
    }

    // Strings are always null terminated
    buf[last_idx] = '\0';
    return buf;
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

int main(void) {
    Sel sel = CLIPBOARD;
    char *out = retrieve_selection(sel);

    puts(out);

    return 0;
}
