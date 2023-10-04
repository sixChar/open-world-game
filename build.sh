mkdir build
pushd build
gcc ../src/sdl_handmade.c -o handmade -g -Wall -lSDL2 -lm
popd
