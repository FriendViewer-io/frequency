#include <SDL.h>
#include <conio.h>

#undef main

int main() {
   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindow("SampleWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500,
                    SDL_WINDOW_RESIZABLE);
   _getch();
}