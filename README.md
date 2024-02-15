# Simple Clipboard manager
A very simple and minimal clipboard manager written in C and gtk4 for Linux, that does one thing
and one thing well. Relies on xclip to modify system clipboard.

## Build instructions
Ensure `meson`, `bear` and `gcc` are setup on your system

- Setup build directory
    ```meson setup build```

- Building binary:
    - If you only need to run the binary:
        ```
        cd build/
        meson compile
        ```

    - But to ensure that LSP's work correctly, generate compilation cache:
        ```
        cd build/
        bear -- meson compile
        ```

- Run the binary:
  ```./build/clipboard-manager```






