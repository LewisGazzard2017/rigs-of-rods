# Project NextSim

NextSim project implements a new simulation infrastructure within RoR,
sharing only bare essentials (OGRE 3d engine integration, OIS input integration,
OpenAL audio integration) with the old simulation code.

Technically, NextSim implements a new game-loop from scratch.
For details, see commentary in file 'source/main/Simulation.h'

For discussion and status info, visit https://forum.rigsofrods.org

## Building

See instructions for regular build (BUILDING.md)

NextSim requires latest stable version of AngelScript (2.31.1).
Download the sources from http://angelcode.com/angelscript/downloads.html
For VisualStudio 2015, use the contained project with 1 modification: change
build type to `/MD` (default is `/MT`)

