#ifndef __SDL_H__
#define __SDL_H__
#include "chip8.h"
#include <SDL2/SDL.h>

byte init_display(const char *title, int scale, int width, int height);

int close_display();

#endif
