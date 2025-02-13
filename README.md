# Simple Clipboard manager
A very simple and minimal clipboard manager written in `C` and `GTK4` for Linux, that does one thing
and one thing well. Relies on `xclip` to modify system clipboard.

## Build instructions
Ensure `xclip`, `meson`, `bear` and `gcc` are setup on your system

- Building binary:
    - If you only need to run the binary:
        ```
        make run
        ```

    - But to ensure that LSP's work correctly, generate compilation cache:
        ```
        bear -- make
        ```

- Run the binary:
  ```./build/clipboard-manager```

