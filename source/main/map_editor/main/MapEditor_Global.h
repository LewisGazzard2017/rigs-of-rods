#pragma once

/// @file
/// @brief Included everywhere

/// StuntRally uses same sources for both editor and simulation.
/// This macro tells compiler where it's at.
#define SR_EDITOR

// ===== from vdrift/dbl.h =====

typedef double Dbl;

#define PI_d  3.14159265358979323846

// ====== dummy input events (to be implemented) =========

struct MapEditor_KeyboardEvent {};