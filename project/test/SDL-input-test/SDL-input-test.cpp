// SDL-input-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// ### http://wiki.libsdl.org/MigrationGuide
// ### http://lazyfoo.net/tutorials/SDL/19_gamepads_and_joysticks/index.php

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;

int main()
{
    std::cout << "Hello SDL!"<<std::endl;
    SDL_SetMainReady();
    // Initialize the SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        std::cerr << "SDL_Init() Failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (SDL_VideoInit(nullptr) != 0)
    {
        std::cerr << "SDL_VideoInit() Failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window, SDL2 way
    SDL_Window *sdl_window = SDL_CreateWindow("RoR SDL test", // Title
                                              SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED,
                                              640, 480, // resolution
                                              0); // Flags

    if (sdl_window == nullptr)
    {
        std::cerr<<"SDL_CreateWindow() returned NULL, message:"<< SDL_GetError()<<std::endl;
        SDL_VideoQuit();
        SDL_Quit();
        return 1;
    }

    // SDL2 rendering context
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdl_window, -1, 0);

    if (sdlRenderer == nullptr)
    {
        std::cerr<<"SDL_CreateRenderer() returned NULL, message:"<< SDL_GetError()<<std::endl;
        SDL_VideoQuit();
        SDL_Quit();
        return 1;
    }

    // Fill the window dark blue
    SDL_SetRenderDrawColor(sdlRenderer, 0, 20, 60, 255);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderPresent(sdlRenderer);

    //Game Controller 1 handler
    SDL_Joystick* gGameController = NULL;
    int num_joy = SDL_NumJoysticks();
    std::cout <<"Num joysticks: " << num_joy <<std::endl;

    // Main loop
    SDL_Event event;
    bool quit_app = false;
    while (!quit_app)
    {
        // Check for messages
        while (SDL_PollEvent(&event) != 0)
        {
            // Check for the quit message
            if (event.type == SDL_QUIT)
            {
                quit_app = true;
            }
        }
        // Game loop will go here...
    }

    // Tell the SDL to clean up and shut down
    SDL_VideoQuit();
    SDL_Quit();
    
    return 0;
}

