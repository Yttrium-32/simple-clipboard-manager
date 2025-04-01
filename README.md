# Simple Clipboard manager
A very simple and minimal clipboard manager written in `C` and the excellent
layout library [`Clay`](https://github.com/nicbarker/clay) for Linux, that does one thing
and one thing well. Relies on `xclip` to modify system clipboard.

## Build instructions
Ensure `xclip`, `cmake`, `bear` and `gcc` are setup on your system

- Build:
    ```
    $ mkdir build && cd build
    $ cmake ..
    $ make
    ```

- Run the binary:
  ```$ ./build/clipboard-manager```

