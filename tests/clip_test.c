#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../clip.h"

void test_append() {
    // Test 1
    String* buffer_1[3] = {NULL};

    Clipboard clipboard_1 = {
        .data = buffer_1,
        .length = 0,
        .capacity = 3
    };

    clipboard_append(&clipboard_1, &(String){ "Hello,", 6 });
    clipboard_append(&clipboard_1, &(String){ "World!", 6 });
    clipboard_append(&clipboard_1, &(String){ "Your mom", 8 });

    printf("clipboard_1.length = %zu\n", clipboard_1.length);
    printf("clipboard_1.capacity = %zu\n", clipboard_1.capacity);
    clipboard_print(clipboard_1);

    assert(clipboard_1.length == 3);

    assert(strcmp(clipboard_1.data[0]->chars, "Hello,") == 0);
    assert(strcmp(clipboard_1.data[1]->chars, "World!") == 0);
    assert(strcmp(clipboard_1.data[2]->chars, "Your mom") == 0);


    // Test 2
    String* buffer_2[3] = {NULL};

    Clipboard clipboard_2 = {
        .data = buffer_2,
        .length = 0,
        .capacity = 3
    };

    clipboard_append(&clipboard_2, &(String){ "test 1", 6 });
    clipboard_append(&clipboard_2, &(String){ "test 2", 6 });
    clipboard_append(&clipboard_2, &(String){ "test 3", 6 });
    clipboard_append(&clipboard_2, &(String){ "test 4", 6 });
    clipboard_append(&clipboard_2, &(String){ "test 5", 6 });

    printf("clipboard_2.length = %zu\n", clipboard_2.length);
    printf("clipboard_2.capacity = %zu\n", clipboard_2.capacity);
    clipboard_print(clipboard_2);

    assert(clipboard_1.length == 3);

    assert(strcmp(clipboard_2.data[0]->chars, "test 3") == 0);
    assert(strcmp(clipboard_2.data[1]->chars, "test 4") == 0);
    assert(strcmp(clipboard_2.data[2]->chars, "test 5") == 0);

    printf("Append passed all tests.\n");
}

int main(void) {
    test_append();
    exit(EXIT_SUCCESS);
}
