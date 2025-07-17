# Simple Clipboard manager
A very simple and minimal clipboard manager for Linux written in `C` using the excellent
layout library [`Clay`](https://github.com/nicbarker/clay). 
Relies on `xclip` to modify system clipboard.

## Build instructions
Ensure `xclip`, `cmake` and `gcc` are setup on your system

#### Build:
```bash
$ mkdir build && cd build
$ cmake ..
$ make
```

#### Run the binary:
```bash
$ ./build/clipboard-manager
```
