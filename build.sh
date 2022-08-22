emcc -o build/index.html main.c sm_simple.c -Os -Wall -I ../raylib/src \
    -L ../raylib/src -s USE_GLFW=3 -s ASYNCIFY --shell-file minshell.html \
    -DPLATFORM_WEB -lraylib && cd build/ && python -m http.server
