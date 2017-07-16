// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <iostream>

// -------- SDL library -----------

// Do not #define `main()` to SDL_Main, see http://wiki.libsdl.org/SDL_SetMainReady and http://wiki.libsdl.org/MigrationGuide#Some_general_truths
#define SDL_MAIN_HANDLED
#include <SDL.h>



// TODO: reference additional headers your program requires here
