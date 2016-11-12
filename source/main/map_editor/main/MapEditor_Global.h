#pragma once

/// @file
/// @brief Included everywhere

#include <OgreCommon.h>

/// StuntRally uses same sources for both editor and simulation.
/// This macro tells compiler where it's at.
#define SR_EDITOR

// ===== from vdrift/dbl.h =====

typedef double Dbl;

#define PI_d  3.14159265358979323846

// ====== dummy input event structures =========

struct MapEditor_KeyboardEvent    {}; //SDL_KeyboardEvent
struct MapEditor_MouseMotionEvent {}; //SFO::MouseMotionEvent
struct MapEditor_MouseButtonEvent {}; //SDL_MouseButtonEvent
struct MapEditor_TextInputEvent   {}; //SDL_TextInputEvent

// ============= replacements for boost ================

std::string MapEditor_GetDirectoryPath(std::string const & dir_in);

bool MapEditor_StrStartsWith(std::string haystack, std::string const & needle);

