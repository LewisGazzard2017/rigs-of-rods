/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2014 Petr Ohlidal

	For more information, see http://www.rigsofrods.com/

	Rigs of Rods is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3, as
	published by the Free Software Foundation.

	Rigs of Rods is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

#include "InputEngine.h"

// some gcc fixes
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif //OGRE_PLATFORM_LINUX

#include "Application.h"
#include "ErrorUtils.h"
#include "OgreSubsystem.h"
#include "RoRWindowEventUtilities.h"
#include "Settings.h"

#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreException.h>


#include "Console.h"
#include "GUIManager.h"
#include "Language.h"
#define _LW(str) LanguageEngine::getSingleton().lookUp(str).asWStr_c_str()

const char *mOISDeviceType[6] = {"Unknown Device", "Keyboard", "Mouse", "JoyStick", "Tablet", "Other Device"};

// LOOOONG list of possible events. see the struct type for the structure ;)
// NOTE: Legacy (slow) event query function has arguments: 
//       getEventValue(int eventID, bool pure = false, int valueSource = ET_ANY) 
// To pre-compute states (new design, fast), we need to know all Event/args combinations in advance.
EventInfo eventInfo[] = {                                                                                                              // ~~~ Usage analysis by only_a_ptr, date: 12/2014

	// ~~~ USAGE (all airplane events): USER: AircraftSimulation::UpdateVehicle()
	{ "AIRPLANE_STEER_RIGHT",     EV_AIRPLANE_STEER_RIGHT,    "Keyboard RIGHT",       _LW("steer right") },                            // QUERY: isEventAnalog(), getEventValue(defaults)
	{ "AIRPLANE_BRAKE",           EV_AIRPLANE_BRAKE,          "Keyboard B",           _LW("normal brake for an aircraft") },           // QUERY: getEventValue(defaults)
	{ "AIRPLANE_ELEVATOR_DOWN",   EV_AIRPLANE_ELEVATOR_DOWN,  "Keyboard DOWN",        _LW("pull the elevator down in an aircraft.") }, // QUERY: getEventValue(defaults)
	{ "AIRPLANE_ELEVATOR_UP",     EV_AIRPLANE_ELEVATOR_UP,    "Keyboard UP",          _LW("pull the elevator up in an aircraft.") },   // QUERY: getEventValue(defaults)
	{ "AIRPLANE_FLAPS_FULL",      EV_AIRPLANE_FLAPS_FULL,     "Keyboard CTRL+2",      _LW("full flaps in an aircraft.") },             // QUERY: getEventBoolValueBounce(defaults)
	{ "AIRPLANE_FLAPS_LESS",      EV_AIRPLANE_FLAPS_LESS,     "Keyboard EXPL+1",      _LW("one step less flaps.") },                   // QUERY: getEventBoolValueBounce(defaults)
	{ "AIRPLANE_FLAPS_MORE",      EV_AIRPLANE_FLAPS_MORE,     "Keyboard EXPL+2",      _LW("one step more flaps.") },                   // QUERY: getEventBoolValueBounce(defaults)        
	{ "AIRPLANE_FLAPS_NONE",      EV_AIRPLANE_FLAPS_NONE,     "Keyboard CTRL+1",      _LW("no flaps.") },                              // QUERY: getEventBoolValueBounce(defaults)
	{ "AIRPLANE_PARKING_BRAKE",   EV_AIRPLANE_PARKING_BRAKE,  "Keyboard P",           _LW("airplane parking brake.") },                // QUERY: getEventBoolValueBounce(defaults) 
	{ "AIRPLANE_REVERSE",         EV_AIRPLANE_REVERSE,        "Keyboard R",           _LW("reverse the turboprops") },                 // QUERY: getEventBoolValueBounce(defaults) 
	{ "AIRPLANE_RUDDER_LEFT",     EV_AIRPLANE_RUDDER_LEFT,    "Keyboard Z",           _LW("rudder left") },                            // QUERY: getEventValue(defaults)
	{ "AIRPLANE_RUDDER_RIGHT",    EV_AIRPLANE_RUDDER_RIGHT,   "Keyboard X",           _LW("rudder right") },                           // QUERY: getEventValue(defaults)
	{ "AIRPLANE_STEER_LEFT",      EV_AIRPLANE_STEER_LEFT,     "Keyboard LEFT",        _LW("steer left") },                             // QUERY: isEventAnalog(), getEventValue(defaults)
	{ "AIRPLANE_STEER_RIGHT",     EV_AIRPLANE_STEER_RIGHT,    "Keyboard RIGHT",       _LW("steer right") },                            // QUERY: isEventAnalog(), getEventValue(defaults)
	{ "AIRPLANE_THROTTLE_AXIS",   EV_AIRPLANE_THROTTLE_AXIS,  "None",                 _LW("throttle axis. Only use this if you have fitting hardware :) (i.e. a Slider)") }, // QUERY: isEventDefined(), getEventValue(defaults)
	{ "AIRPLANE_THROTTLE_DOWN",   EV_AIRPLANE_THROTTLE_DOWN,  "Keyboard EXPL+PGDOWN", _LW("decreases the airplane thrust") },          // QUERY: getEventBoolValueBounce(defaults) 
	{ "AIRPLANE_THROTTLE_FULL",   EV_AIRPLANE_THROTTLE_FULL,  "Keyboard CTRL+PGUP",   _LW("full thrust") },                            // QUERY: getEventBoolValueBounce(defaults)                          
	{ "AIRPLANE_THROTTLE_NO",     EV_AIRPLANE_THROTTLE_NO,    "Keyboard CTRL+PGDOWN", _LW("no thrust") },                              // QUERY: getEventBoolValueBounce(defaults) 
	{ "AIRPLANE_THROTTLE_UP",     EV_AIRPLANE_THROTTLE_UP,    "Keyboard EXPL+PGUP",   _LW("increase the airplane thrust") },           // QUERY: getEventBoolValueBounce(defaults)
	{ "AIRPLANE_TOGGLE_ENGINES",  EV_AIRPLANE_TOGGLE_ENGINES, "Keyboard CTRL+HOME",   _LW("switch all engines on / off") },            // QUERY: getEventBoolValueBounce(defaults)
	
	// ~~~ USAGE (all boat events): USER: RoRFrameListener::updateEvents(){driveable==BOAT}
	{ "BOAT_CENTER_RUDDER",        EV_BOAT_CENTER_RUDDER,     "Keyboard PGDOWN",      _LW("center the rudder") },           // QUERY: getEventBoolValueBounce(defaults)
	{ "BOAT_REVERSE",              EV_BOAT_REVERSE,           "Keyboard PGUP",        _LW("no thrust") },                   // QUERY: getEventBoolValueBounce(defaults)
	{ "BOAT_STEER_LEFT",           EV_BOAT_STEER_LEFT,        "Keyboard LEFT",        _LW("steer left a step") },           // QUERY: getEventValue(defaults), getEventBounceTime()
	{ "BOAT_STEER_LEFT_AXIS",      EV_BOAT_STEER_LEFT_AXIS,   "None",                 _LW("steer left (analog value!)") },  // QUERY: getEventValue(defaults), isEventDefined()
	{ "BOAT_STEER_RIGHT",          EV_BOAT_STEER_RIGHT,       "Keyboard RIGHT",       _LW("steer right a step") },          // QUERY: getEventValue(defaults), getEventBounceTime()
	{ "BOAT_STEER_RIGHT_AXIS",     EV_BOAT_STEER_RIGHT_AXIS,  "None",                 _LW("steer right (analog value!)") },	// QUERY: getEventValue(defaults), isEventDefined()
	{ "BOAT_THROTTLE_AXIS",        EV_BOAT_THROTTLE_AXIS,     "None",                 _LW("throttle axis. Only use this if you have fitting hardware :) (i.e. a Slider)") }, // QUERY: getEventValue(defaults), isEventDefined()
	{ "BOAT_THROTTLE_DOWN",        EV_BOAT_THROTTLE_DOWN,     "Keyboard DOWN",        _LW("decrease throttle") },           // QUERY: getEventBoolValueBounce(defaults)
	{ "BOAT_THROTTLE_UP",          EV_BOAT_THROTTLE_UP,       "Keyboard UP",          _LW("increase throttle") },           // QUERY: getEventBoolValueBounce(defaults)
	
	// ~~~ USAGE (all Caelum events): USER: RoRFrameListener::updateEvents(), QUERY: getEventBoolValue() [1x each]
	{ "CAELUM_DECREASE_TIME",      EV_CAELUM_DECREASE_TIME,      "Keyboard EXPL+SUBTRACT",  _LW("decrease day-time") },
	{ "CAELUM_DECREASE_TIME_FAST", EV_CAELUM_DECREASE_TIME_FAST, "Keyboard SHIFT+SUBTRACT", _LW("decrease day-time a lot faster") },
	{ "CAELUM_INCREASE_TIME",      EV_CAELUM_INCREASE_TIME,      "Keyboard EXPL+ADD",       _LW("increase day-time") },
	{ "CAELUM_INCREASE_TIME_FAST", EV_CAELUM_INCREASE_TIME_FAST, "Keyboard SHIFT+ADD",      _LW("increase day-time a lot faster") },
	
	// ~~~ USER: CameraManager::update
	{ "CAMERA_CHANGE",             EV_CAMERA_CHANGE,          "Keyboard EXPL+C",        _LW("change camera mode") },        // QUERY: getEventBoolValueBounce(defaults)
	
	// ~~~ USAGE: USER: CameraBehaviorOrbit::update()
	{ "CAMERA_LOOKBACK",           EV_CAMERA_LOOKBACK,        "Keyboard NUMPAD1",       _LW("look back (toggles between normal and lookback)") }, // QUERY: getEventBoolValueBounce(default)
	{ "CAMERA_RESET",              EV_CAMERA_RESET,           "Keyboard NUMPAD5",       _LW("reset the camera position") }, // QUERY: getEventBoolValue(default)
	{ "CAMERA_ROTATE_DOWN",        EV_CAMERA_ROTATE_DOWN,     "Keyboard NUMPAD2",       _LW("rotate camera down") },        // QUERY: getEventValue(defaults)
	{ "CAMERA_ROTATE_LEFT",        EV_CAMERA_ROTATE_LEFT,     "Keyboard NUMPAD4",       _LW("rotate camera left") },        // QUERY: getEventValue(defaults)
	{ "CAMERA_ROTATE_RIGHT",       EV_CAMERA_ROTATE_RIGHT,    "Keyboard NUMPAD6",       _LW("rotate camera right") },       // QUERY: getEventValue(defaults)
	{ "CAMERA_ROTATE_UP",          EV_CAMERA_ROTATE_UP,       "Keyboard NUMPAD8",       _LW("rotate camera up") },          // QUERY: getEventValue(defaults)
	{ "CAMERA_SWIVEL_DOWN",        EV_CAMERA_SWIVEL_DOWN,     "Keyboard CTRL+NUMPAD2",  _LW("swivel camera down") },        // QUERY: getEventValue(defaults)
	{ "CAMERA_SWIVEL_LEFT",        EV_CAMERA_SWIVEL_LEFT,     "Keyboard CTRL+NUMPAD4",  _LW("swivel camera left") },        // QUERY: getEventValue(defaults)
	{ "CAMERA_SWIVEL_RIGHT",       EV_CAMERA_SWIVEL_RIGHT,    "Keyboard CTRL+NUMPAD6",  _LW("swivel camera right") },       // QUERY: getEventValue(defaults)
	{ "CAMERA_SWIVEL_UP",          EV_CAMERA_SWIVEL_UP,       "Keyboard CTRL+NUMPAD8",  _LW("swivel camera up") },          // QUERY: getEventValue(defaults)
	{ "CAMERA_ZOOM_IN",            EV_CAMERA_ZOOM_IN,         "Keyboard EXPL+NUMPAD9",  _LW("zoom camera in") },            // QUERY: getEventValue(defaults)
	{ "CAMERA_ZOOM_IN_FAST",       EV_CAMERA_ZOOM_IN_FAST,    "Keyboard SHIFT+NUMPAD9", _LW("zoom camera in faster") },     // QUERY: getEventValue(defaults)
	{ "CAMERA_ZOOM_OUT",           EV_CAMERA_ZOOM_OUT,        "Keyboard EXPL+NUMPAD3",  _LW("zoom camera out") },           // QUERY: getEventValue(defaults)
	{ "CAMERA_ZOOM_OUT_FAST",      EV_CAMERA_ZOOM_OUT_FAST,   "Keyboard SHIFT+NUMPAD3", _LW("zoom camera out faster") },    // QUERY: getEventValue(defaults)
	
	// ~~~ USAGE: users <= 2 (Character + CameraBehaviorFree)
	{ "CHARACTER_BACKWARDS",       EV_CHARACTER_BACKWARDS,      "Keyboard S",           _LW("step backwards with the character") }, // Character::update() { getEventValue(defaults)} // CameraBehaviorFree::update() { getEventBoolValue(defaults) }
	{ "CHARACTER_FORWARD",         EV_CHARACTER_FORWARD,        "Keyboard W",           _LW("step forward with the character") },   // Character::update() { getEventValue(defaults)} // CameraBehaviorFree::update() { getEventBoolValue(defaults) }
	{ "CHARACTER_JUMP",            EV_CHARACTER_JUMP,           "Keyboard SPACE",       _LW("let the character jump") },            // Character::update() { getEventBoolValue(defaults) }
	{ "CHARACTER_LEFT",            EV_CHARACTER_LEFT,           "Keyboard LEFT",        _LW("rotate character left") },             // Character::update() { getEventValue(defaults)} // CameraBehaviorFree::update() { getEventBoolValue(defaults) }
	{ "CHARACTER_RIGHT",           EV_CHARACTER_RIGHT,          "Keyboard RIGHT",       _LW("rotate character right") },            // Character::update() { getEventValue(defaults)} // CameraBehaviorFree::update() { getEventBoolValue(defaults) }
	{ "CHARACTER_RUN",             EV_CHARACTER_RUN,            "Keyboard SHIFT+W",     _LW("let the character run") },             // Character::update() { getEventValue(defaults) }       
	{ "CHARACTER_SIDESTEP_LEFT",   EV_CHARACTER_SIDESTEP_LEFT,  "Keyboard A",           _LW("sidestep to the left") },              // Character::update() { getEventValue(defaults)} // CameraBehaviorFree::update() { getEventBoolValue(defaults) }
	{ "CHARACTER_SIDESTEP_RIGHT",  EV_CHARACTER_SIDESTEP_RIGHT, "Keyboard D",           _LW("sidestep to the right") },             // Character::update() { getEventValue(defaults)} // CameraBehaviorFree::update() { getEventBoolValue(defaults) }
	
	// ~~~ USAGE: RoRFrameListener::updateEvents() { FOR EACH Event { getEventValue(defaults) } }
	{ "COMMANDS_01", EV_COMMANDS_01, "Keyboard EXPL+F1", _LW("Command 1") },
	{ "COMMANDS_02", EV_COMMANDS_02, "Keyboard EXPL+F2", _LW("Command 2") },
	{ "COMMANDS_03", EV_COMMANDS_03, "Keyboard EXPL+F3", _LW("Command 3") },
	{ "COMMANDS_04", EV_COMMANDS_04, "Keyboard EXPL+F4", _LW("Command 4") },
	{ "COMMANDS_05", EV_COMMANDS_05, "Keyboard EXPL+F5", _LW("Command 5") },
	{ "COMMANDS_06", EV_COMMANDS_06, "Keyboard EXPL+F6", _LW("Command 6") },
	{ "COMMANDS_07", EV_COMMANDS_07, "Keyboard EXPL+F7", _LW("Command 7") },
	{ "COMMANDS_08", EV_COMMANDS_08, "Keyboard EXPL+F8", _LW("Command 8") },
	{ "COMMANDS_09", EV_COMMANDS_09, "Keyboard EXPL+F9", _LW("Command 9") },
	{ "COMMANDS_10", EV_COMMANDS_10, "Keyboard EXPL+F10", _LW("Command 10") },
	{ "COMMANDS_11", EV_COMMANDS_11, "Keyboard EXPL+F11", _LW("Command 11") },
	{ "COMMANDS_12", EV_COMMANDS_12, "Keyboard EXPL+F12", _LW("Command 12") },
	{ "COMMANDS_13", EV_COMMANDS_13, "Keyboard EXPL+CTRL+F1", _LW("Command 13") },
	{ "COMMANDS_14", EV_COMMANDS_14, "Keyboard EXPL+CTRL+F2", _LW("Command 14") },
	{ "COMMANDS_15", EV_COMMANDS_15, "Keyboard EXPL+CTRL+F3", _LW("Command 15") },
	{ "COMMANDS_16", EV_COMMANDS_16, "Keyboard EXPL+CTRL+F4", _LW("Command 16") },
	{ "COMMANDS_17", EV_COMMANDS_17, "Keyboard EXPL+CTRL+F5", _LW("Command 17") },
	{ "COMMANDS_18", EV_COMMANDS_18, "Keyboard EXPL+CTRL+F6", _LW("Command 18") },
	{ "COMMANDS_19", EV_COMMANDS_19, "Keyboard EXPL+CTRL+F7", _LW("Command 19") },
	{ "COMMANDS_20", EV_COMMANDS_20, "Keyboard EXPL+CTRL+F8", _LW("Command 20") },
	{ "COMMANDS_21", EV_COMMANDS_21, "Keyboard EXPL+CTRL+F9", _LW("Command 21") },
	{ "COMMANDS_22", EV_COMMANDS_22, "Keyboard EXPL+CTRL+F10", _LW("Command 22") },
	{ "COMMANDS_23", EV_COMMANDS_23, "Keyboard EXPL+CTRL+F11", _LW("Command 23") },
	{ "COMMANDS_24", EV_COMMANDS_24, "Keyboard EXPL+CTRL+F12", _LW("Command 24") },
	{ "COMMANDS_25", EV_COMMANDS_25, "Keyboard EXPL+SHIFT+F1", _LW("Command 25") },
	{ "COMMANDS_26", EV_COMMANDS_26, "Keyboard EXPL+SHIFT+F2", _LW("Command 26") },
	{ "COMMANDS_27", EV_COMMANDS_27, "Keyboard EXPL+SHIFT+F3", _LW("Command 27") },
	{ "COMMANDS_28", EV_COMMANDS_28, "Keyboard EXPL+SHIFT+F4", _LW("Command 28") },
	{ "COMMANDS_29", EV_COMMANDS_29, "Keyboard EXPL+SHIFT+F5", _LW("Command 29") },
	{ "COMMANDS_30", EV_COMMANDS_30, "Keyboard EXPL+SHIFT+F6", _LW("Command 30") },
	{ "COMMANDS_31", EV_COMMANDS_31, "Keyboard EXPL+SHIFT+F7", _LW("Command 31") },
	{ "COMMANDS_32", EV_COMMANDS_32, "Keyboard EXPL+SHIFT+F8", _LW("Command 32") },
	{ "COMMANDS_33", EV_COMMANDS_33, "Keyboard EXPL+SHIFT+F9", _LW("Command 33") },
	{ "COMMANDS_34", EV_COMMANDS_34, "Keyboard EXPL+SHIFT+F10", _LW("Command 34") },
	{ "COMMANDS_35", EV_COMMANDS_35, "Keyboard EXPL+SHIFT+F11", _LW("Command 35") },
	{ "COMMANDS_36", EV_COMMANDS_36, "Keyboard EXPL+SHIFT+F12", _LW("Command 36") },
	{ "COMMANDS_37", EV_COMMANDS_37, "Keyboard EXPL+ALT+F1", _LW("Command 37") },
	{ "COMMANDS_38", EV_COMMANDS_38, "Keyboard EXPL+ALT+F2", _LW("Command 38") },
	{ "COMMANDS_39", EV_COMMANDS_39, "Keyboard EXPL+ALT+F3", _LW("Command 39") },
	{ "COMMANDS_40", EV_COMMANDS_40, "Keyboard EXPL+ALT+F4", _LW("Command 40") },
	{ "COMMANDS_41", EV_COMMANDS_41, "Keyboard EXPL+ALT+F5", _LW("Command 41") },
	{ "COMMANDS_42", EV_COMMANDS_42, "Keyboard EXPL+ALT+F6", _LW("Command 42") },
	{ "COMMANDS_43", EV_COMMANDS_43, "Keyboard EXPL+ALT+F7", _LW("Command 43") },
	{ "COMMANDS_44", EV_COMMANDS_44, "Keyboard EXPL+ALT+F8", _LW("Command 44") },
	{ "COMMANDS_45", EV_COMMANDS_45, "Keyboard EXPL+ALT+F9", _LW("Command 45") },
	{ "COMMANDS_46", EV_COMMANDS_46, "Keyboard EXPL+ALT+F10", _LW("Command 46") },
	{ "COMMANDS_47", EV_COMMANDS_47, "Keyboard EXPL+ALT+F11", _LW("Command 47") },
	{ "COMMANDS_48", EV_COMMANDS_48, "Keyboard EXPL+ALT+F12", _LW("Command 48") },
	{ "COMMANDS_49", EV_COMMANDS_49, "Keyboard EXPL+CTRL+SHIFT+F1", _LW("Command 49") },
	{ "COMMANDS_50", EV_COMMANDS_50, "Keyboard EXPL+CTRL+SHIFT+F2", _LW("Command 50") },
	{ "COMMANDS_51", EV_COMMANDS_51, "Keyboard EXPL+CTRL+SHIFT+F3", _LW("Command 51") },
	{ "COMMANDS_52", EV_COMMANDS_52, "Keyboard EXPL+CTRL+SHIFT+F4", _LW("Command 52") },
	{ "COMMANDS_53", EV_COMMANDS_53, "Keyboard EXPL+CTRL+SHIFT+F5", _LW("Command 53") },
	{ "COMMANDS_54", EV_COMMANDS_54, "Keyboard EXPL+CTRL+SHIFT+F6", _LW("Command 54") },
	{ "COMMANDS_55", EV_COMMANDS_55, "Keyboard EXPL+CTRL+SHIFT+F7", _LW("Command 55") },
	{ "COMMANDS_56", EV_COMMANDS_56, "Keyboard EXPL+CTRL+SHIFT+F8", _LW("Command 56") },
	{ "COMMANDS_57", EV_COMMANDS_57, "Keyboard EXPL+CTRL+SHIFT+F9", _LW("Command 57") },
	{ "COMMANDS_58", EV_COMMANDS_58, "Keyboard EXPL+CTRL+SHIFT+F10", _LW("Command 58") },
	{ "COMMANDS_59", EV_COMMANDS_59, "Keyboard EXPL+CTRL+SHIFT+F11", _LW("Command 59") },
	{ "COMMANDS_60", EV_COMMANDS_60, "Keyboard EXPL+CTRL+SHIFT+F12", _LW("Command 60") },
	{ "COMMANDS_61", EV_COMMANDS_61, "Keyboard EXPL+CTRL+ALT+F1", _LW("Command 61") },
	{ "COMMANDS_62", EV_COMMANDS_62, "Keyboard EXPL+CTRL+ALT+F2", _LW("Command 62") },
	{ "COMMANDS_63", EV_COMMANDS_63, "Keyboard EXPL+CTRL+ALT+F3", _LW("Command 63") },
	{ "COMMANDS_64", EV_COMMANDS_64, "Keyboard EXPL+CTRL+ALT+F4", _LW("Command 64") },
	{ "COMMANDS_65", EV_COMMANDS_65, "Keyboard EXPL+CTRL+ALT+F5", _LW("Command 65") },
	{ "COMMANDS_66", EV_COMMANDS_66, "Keyboard EXPL+CTRL+ALT+F6", _LW("Command 66") },
	{ "COMMANDS_67", EV_COMMANDS_67, "Keyboard EXPL+CTRL+ALT+F7", _LW("Command 67") },
	{ "COMMANDS_68", EV_COMMANDS_68, "Keyboard EXPL+CTRL+ALT+F8", _LW("Command 68") },
	{ "COMMANDS_69", EV_COMMANDS_69, "Keyboard EXPL+CTRL+ALT+F9", _LW("Command 69") },
	{ "COMMANDS_70", EV_COMMANDS_70, "Keyboard EXPL+CTRL+ALT+F10", _LW("Command 70") },
	{ "COMMANDS_71", EV_COMMANDS_71, "Keyboard EXPL+CTRL+ALT+F11", _LW("Command 71") },
	{ "COMMANDS_72", EV_COMMANDS_72, "Keyboard EXPL+CTRL+ALT+F12", _LW("Command 72") },
	{ "COMMANDS_73", EV_COMMANDS_73, "Keyboard EXPL+CTRL+SHIFT+ALT+F1", _LW("Command 73") },
	{ "COMMANDS_74", EV_COMMANDS_74, "Keyboard EXPL+CTRL+SHIFT+ALT+F2", _LW("Command 74") },
	{ "COMMANDS_75", EV_COMMANDS_75, "Keyboard EXPL+CTRL+SHIFT+ALT+F3", _LW("Command 75") },
	{ "COMMANDS_76", EV_COMMANDS_76, "Keyboard EXPL+CTRL+SHIFT+ALT+F4", _LW("Command 76") },
	{ "COMMANDS_77", EV_COMMANDS_77, "Keyboard EXPL+CTRL+SHIFT+ALT+F5", _LW("Command 77") },
	{ "COMMANDS_78", EV_COMMANDS_78, "Keyboard EXPL+CTRL+SHIFT+ALT+F6", _LW("Command 78") },
	{ "COMMANDS_79", EV_COMMANDS_79, "Keyboard EXPL+CTRL+SHIFT+ALT+F7", _LW("Command 79") },
	{ "COMMANDS_80", EV_COMMANDS_80, "Keyboard EXPL+CTRL+SHIFT+ALT+F8", _LW("Command 80") },
	{ "COMMANDS_81", EV_COMMANDS_81, "Keyboard EXPL+CTRL+SHIFT+ALT+F9", _LW("Command 81") },
	{ "COMMANDS_82", EV_COMMANDS_82, "Keyboard EXPL+CTRL+SHIFT+ALT+F10", _LW("Command 82") },
	{ "COMMANDS_83", EV_COMMANDS_83, "Keyboard EXPL+CTRL+SHIFT+ALT+F11", _LW("Command 83") },
	{ "COMMANDS_84", EV_COMMANDS_84, "Keyboard EXPL+CTRL+SHIFT+ALT+F12", _LW("Command 84") },

	// ~~~ USAGE: Misc.
	{ "COMMON_CONSOLEDISPLAY",          EV_COMMON_CONSOLEDISPLAY,          "Keyboard EXPL+GRAVE",           _LW("show / hide the console") },                          // LobbyGUI {getKeboardKeyForCommand()} // NO QUERY!
	{ "COMMON_CONSOLEMODE",             EV_COMMON_CONSOLEMODE,             "Keyboard EXPL+CTRL+GRAVE",      _LW("toggle appearance of console") },                     // NO USAGE!
	{ "COMMON_ENTER_CHATMODE",          EV_COMMON_ENTER_CHATMODE,          "Keyboard Y",                    _LW("enter the chat") },                                   // RoRFrameListener{getEventBoolValueBounce(time=0.5)} // MainThread{getKeyForCommand(), getEventBoolValueBounce(time=0.5)}
	{ "COMMON_SEND_CHAT",               EV_COMMON_SEND_CHAT,               "Keyboard RETURN",               _LW("sends the entered text") },                           // NO USAGE!
	{ "COMMON_ENTER_OR_EXIT_TRUCK",     EV_COMMON_ENTER_OR_EXIT_TRUCK,     "Keyboard RETURN",               _LW("enter or exit a truck") },                            // RoRFrameListener{getEventBoolValue(defaults)}
	{ "COMMON_HIDE_GUI",                EV_COMMON_HIDE_GUI,                "Keyboard EXPL+U",               _LW("hide all GUI elements") },                            // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_LOCK",                    EV_COMMON_LOCK,                    "Keyboard EXPL+L",               _LW("connect hook node to a node in close proximity") },   // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_AUTOLOCK",                EV_COMMON_AUTOLOCK,                "Keyboard EXPL+ALT+L",           _LW("unlock autolock hook node") },                        // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_ROPELOCK",                EV_COMMON_ROPELOCK,                "Keyboard EXPL+CTRL+L",          _LW("connect a rope to a node in close proximity") },      // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_OUTPUT_POSITION",         EV_COMMON_OUTPUT_POSITION,         "Keyboard H",                    _LW("write current position to log (you can open the logfile and reuse the position)") },// RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_PRESSURE_LESS",           EV_COMMON_PRESSURE_LESS,           "Keyboard LBRACKET",             _LW("decrease tire pressure (note: only very few trucks support this)") }, // RoRFrameListener{getEventBoolValue(defaults)}
	{ "COMMON_PRESSURE_MORE",           EV_COMMON_PRESSURE_MORE,           "Keyboard RBRACKET",             _LW("increase tire pressure (note: only very few trucks support this)") }, // RoRFrameListener{getEventBoolValue(defaults)}
	{ "COMMON_QUIT_GAME",               EV_COMMON_QUIT_GAME,               "Keyboard EXPL+ESCAPE",          _LW("exit the game") },                                    // RoRFrameListener+MainThread{getEventBoolValueBounce(defaults)}
	{ "COMMON_REPAIR_TRUCK",            EV_COMMON_REPAIR_TRUCK,            "Keyboard BACK",                 _LW("repair truck") },                                     // RoRFrameListener{getEventBoolValue(defaults)}
	{ "COMMON_RESCUE_TRUCK",            EV_COMMON_RESCUE_TRUCK,            "Keyboard EXPL+R",               _LW("teleport to rescue truck") },                         // RoRFrameListener{getEventBoolValueBounce(time=0.5)}
	{ "COMMON_RESET_TRUCK",             EV_COMMON_RESET_TRUCK,             "Keyboard I",                    _LW("reset truck to original starting position") },        // RoRFrameListener{getEventBoolValue(defaults)}
	{ "COMMON_SCREENSHOT",              EV_COMMON_SCREENSHOT,              "Keyboard EXPL+SYSRQ",           _LW("take a screenshot") },                                // RoRFrameListener{getEventBoolValueBounce(time=0.5)}
	{ "COMMON_SCREENSHOT_BIG",          EV_COMMON_SCREENSHOT_BIG,          "Keyboard EXPL+CTRL+SYSRQ",      _LW("take a big screenshot (3 times the screen size)") },  // NO USAGE!
	{ "COMMON_SAVE_TERRAIN",            EV_COMMON_SAVE_TERRAIN,            "Keyboard EXPL+ALT+SHIF+CTRL+M", _LW("save the currently loaded terrain to a mesh file") }, // NO USAGE!
	{ "COMMON_SECURE_LOAD",             EV_COMMON_SECURE_LOAD,             "Keyboard O",                    _LW("tie a load to the truck") },                          // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_SHOW_SKELETON",           EV_COMMON_SHOW_SKELETON,           "Keyboard K",                    _LW("toggle skeleton display mode") },                     // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_START_TRUCK_EDITOR",      EV_COMMON_START_TRUCK_EDITOR,      "Keyboard EXPL+SHIFT+Y",         _LW("start the old truck editor") },                       // UNUSED!
	{ "COMMON_TOGGLE_CUSTOM_PARTICLES", EV_COMMON_TOGGLE_CUSTOM_PARTICLES, "Keyboard G",                    _LW("toggle particle cannon") },                           // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_TOGGLE_MAT_DEBUG",        EV_COMMON_TOGGLE_MAT_DEBUG,        "",                              _LW("debug purpose - dont use") },                         // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_TOGGLE_RENDER_MODE",      EV_COMMON_TOGGLE_RENDER_MODE,      "Keyboard E",                    _LW("toggle render mode (solid, wireframe and points)") }, // RoRFrameListener{getEventBoolValueBounce(time=0.5)}
	{ "COMMON_TOGGLE_REPLAY_MODE",      EV_COMMON_TOGGLE_REPLAY_MODE,      "Keyboard J",                    _LW("enable or disable replay mode") },                    // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_TOGGLE_STATS",            EV_COMMON_TOGGLE_STATS,            "Keyboard EXPL+F",               _LW("toggle Ogre statistics (FPS etc.)") },                // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_TOGGLE_TRUCK_BEACONS",    EV_COMMON_TOGGLE_TRUCK_BEACONS,    "Keyboard M",                    _LW("toggle truck beacons") },                             // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_TOGGLE_TRUCK_LIGHTS",     EV_COMMON_TOGGLE_TRUCK_LIGHTS,     "Keyboard N",                    _LW("toggle truck front lights") },                        // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_TRUCK_INFO",              EV_COMMON_TRUCK_INFO,              "Keyboard EXPL+T",               _LW("toggle truck HUD") },                                 // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "COMMON_FOV_LESS",                EV_COMMON_FOV_LESS,                "Keyboard EXPL+NUMPAD7",         _LW("decreases the current FOV value") },                  // RoRFrameListener{getEventBoolValueBounce(time=0.1); getEventBoolValue(defaults)}
	{ "COMMON_FOV_MORE",                EV_COMMON_FOV_MORE,                "Keyboard EXPL+CTRL+NUMPAD7",    _LW("increase the current FOV value") },                   // RoRFrameListener{getEventBoolValueBounce(time=0.1) }
	
	// ~~~ Unused
	{ "GRASS_LESS", EV_GRASS_LESS, "", _LW("EXPERIMENTAL: remove some grass") },
	{ "GRASS_MORE", EV_GRASS_MORE, "", _LW("EXPERIMENTAL: add some grass") },
	{ "GRASS_MOST", EV_GRASS_MOST, "", _LW("EXPERIMENTAL: set maximum amount of grass") },
	{ "GRASS_NONE", EV_GRASS_NONE, "", _LW("EXPERIMENTAL: remove grass completely") },
	{ "GRASS_SAVE", EV_GRASS_SAVE, "", _LW("EXPERIMENTAL: save changes to the grass density image") },

	// ~~~ Unused
	{ "MENU_DOWN",   EV_MENU_DOWN,   "Keyboard DOWN",        _LW("select next element in current category") },
	{ "MENU_LEFT",   EV_MENU_LEFT,   "Keyboard LEFT",        _LW("select previous category") },
	{ "MENU_RIGHT",  EV_MENU_RIGHT,  "Keyboard RIGHT",       _LW("select next category") },
	{ "MENU_SELECT", EV_MENU_SELECT, "Keyboard EXPL+RETURN", _LW("select focussed item and close menu") },
	{ "MENU_UP",     EV_MENU_UP,     "Keyboard UP",          _LW("select previous element in current category") },
	
	// ~~~ USAGE: SurveyMapManager
	{ "SURVEY_MAP_TOGGLE_ICONS", EV_SURVEY_MAP_TOGGLE_ICONS, "Keyboard EXPL+CTRL+SHIFT+ALT+TAB", _LW("toggle map icons") }, // SurveyMapManager{getEventBoolValueBounce(defaults)}
	{ "SURVEY_MAP_TOGGLE_VIEW",  EV_SURVEY_MAP_TOGGLE_VIEW,  "Keyboard EXPL+TAB",                _LW("toggle map modes") }, // SurveyMapManager{getEventBoolValueBounce(defaults)}
	{ "SURVEY_MAP_TOGGLE_ALPHA", EV_SURVEY_MAP_ALPHA,        "Keyboard EXPL+CTRL+SHIFT+TAB",     _LW("toggle translucency of overview-map") }, // SurveyMapManager{getEventBoolValueBounce(defaults)}
	{ "SURVEY_MAP_ZOOM_IN",      EV_SURVEY_MAP_ZOOM_IN,      "Keyboard EXPL+CTRL+TAB",           _LW("zoom in") },  // SurveyMapManager{getEventBoolValue(defaults)}
	{ "SURVEY_MAP_ZOOM_OUT",     EV_SURVEY_MAP_ZOOM_OUT,     "Keyboard EXPL+SHIFT+TAB",          _LW("zoom out") }, // SurveyMapManager{getEventBoolValue(defaults)}
	
	// ~~~ Unused
	{ "TERRAINEDITOR_BUILT",          EV_TERRAINEDITOR_BUILT,          "Keyboard SPACE",       _LW("place currently selected object at current position") },
	{ "TERRAINEDITOR_PITCHBACKWARD",  EV_TERRAINEDITOR_PITCHBACKWARD,  "Keyboard EXPL+DELETE", _LW("pitch object backward") },
	{ "TERRAINEDITOR_PITCHFOREWARD",  EV_TERRAINEDITOR_PITCHFOREWARD,  "Keyboard END",         _LW("pitch object foreward") },
	{ "TERRAINEDITOR_ROTATELEFT",     EV_TERRAINEDITOR_ROTATELEFT,     "Keyboard INSERT",      _LW("rotate object left") },
	{ "TERRAINEDITOR_ROTATERIGHT",    EV_TERRAINEDITOR_ROTATERIGHT,    "Keyboard HOME",        _LW("rotate object right") },
	{ "TERRAINEDITOR_SELECTROAD",     EV_TERRAINEDITOR_SELECTROAD,     "Keyboard B",           _LW("switch to road laying mode") },
	{ "TERRAINEDITOR_TOGGLEOBJECT",   EV_TERRAINEDITOR_TOGGLEOBJECT,   "Keyboard V",           _LW("toggle between available objects") },
	{ "TERRAINEDITOR_TOGGLEROADTYPE", EV_TERRAINEDITOR_TOGGLEROADTYPE, "Keyboard EXPL+CTRL+T", _LW("toggle between available road types") },
	
	// ~~~ Usage: misc.
	{ "TRUCK_ACCELERATE",     EV_TRUCK_ACCELERATE,     "Keyboard UP",     _LW("accelerate the truck") },                       // LandVehicleSimulation{ getEventValue(defaults) }
	{ "TRUCK_AUTOSHIFT_DOWN", EV_TRUCK_AUTOSHIFT_DOWN, "Keyboard PGDOWN", _LW("shift automatic transmission one gear down") }, // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_AUTOSHIFT_UP",   EV_TRUCK_AUTOSHIFT_UP,   "Keyboard PGUP",   _LW("shift automatic transmission one gear up") },   // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_BLINK_LEFT",     EV_TRUCK_BLINK_LEFT,     "Keyboard COMMA",  _LW("toggle left direction indicator (blinker)") },  // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "TRUCK_BLINK_RIGHT",    EV_TRUCK_BLINK_RIGHT,    "Keyboard PERIOD", _LW("toggle right direction indicator (blinker)") }, // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "TRUCK_BLINK_WARN",     EV_TRUCK_BLINK_WARN,     "Keyboard MINUS",  _LW("toggle all direction indicators") },            // RoRFrameListener{getEventBoolValueBounce(defaults)}
	{ "TRUCK_BRAKE",          EV_TRUCK_BRAKE,          "Keyboard DOWN",   _LW("brake") },                                      // LandVehicleSimulation{ 2x getEventValue(defaults) }
	{ "TRUCK_HORN",           EV_TRUCK_HORN,           "Keyboard H",      _LW("truck horn") },                                 // LandVehicleSimulation{getEventBoolValueBounce(defaults); getEventBoolValue(defaults)}
	
	// ~~~ Usage: Beam { getEventBoolValue(defaults) }
	{ "TRUCK_LIGHTTOGGLE1",  EV_TRUCK_LIGHTTOGGLE01, "Keyboard EXPL+CTRL+1", _LW("toggle custom light 1") }, 
	{ "TRUCK_LIGHTTOGGLE2",  EV_TRUCK_LIGHTTOGGLE02, "Keyboard EXPL+CTRL+2", _LW("toggle custom light 2") },
	{ "TRUCK_LIGHTTOGGLE3",  EV_TRUCK_LIGHTTOGGLE03, "Keyboard EXPL+CTRL+3", _LW("toggle custom light 3") },
	{ "TRUCK_LIGHTTOGGLE4",  EV_TRUCK_LIGHTTOGGLE04, "Keyboard EXPL+CTRL+4", _LW("toggle custom light 4") },
	{ "TRUCK_LIGHTTOGGLE5",  EV_TRUCK_LIGHTTOGGLE05, "Keyboard EXPL+CTRL+5", _LW("toggle custom light 5") },
	{ "TRUCK_LIGHTTOGGLE6",  EV_TRUCK_LIGHTTOGGLE06, "Keyboard EXPL+CTRL+6", _LW("toggle custom light 6") },
	{ "TRUCK_LIGHTTOGGLE7",  EV_TRUCK_LIGHTTOGGLE07, "Keyboard EXPL+CTRL+7", _LW("toggle custom light 7") },
	{ "TRUCK_LIGHTTOGGLE8",  EV_TRUCK_LIGHTTOGGLE08, "Keyboard EXPL+CTRL+8", _LW("toggle custom light 8") },
	{ "TRUCK_LIGHTTOGGLE9",  EV_TRUCK_LIGHTTOGGLE09, "Keyboard EXPL+CTRL+9", _LW("toggle custom light 9") },
	{ "TRUCK_LIGHTTOGGLE10", EV_TRUCK_LIGHTTOGGLE10, "Keyboard EXPL+CTRL+0", _LW("toggle custom light 10") },

	// ~~~ Usage: LandVehicleSimulation
	{ "TRUCK_MANUAL_CLUTCH",           EV_TRUCK_MANUAL_CLUTCH,           "Keyboard LSHIFT",          _LW("manual clutch (for manual transmission)") },          // LandVehicleSimulation{ 2x getEventValue(defaults) }
	{ "TRUCK_PARKING_BRAKE",           EV_TRUCK_PARKING_BRAKE,           "Keyboard P",               _LW("toggle parking brake") },             // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_ANTILOCK_BRAKE",          EV_TRUCK_ANTILOCK_BRAKE,          "Keyboard EXPL+SHIFT+B",    _LW("toggle antilock brake") },            // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_TRACTION_CONTROL",        EV_TRUCK_TRACTION_CONTROL,        "Keyboard EXPL+SHIFT+T",    _LW("toggle traction control") },          // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_CRUISE_CONTROL",          EV_TRUCK_CRUISE_CONTROL,          "Keyboard EXPL+SPACE",      _LW("toggle cruise control") },            // LandVehicleSimulation{ getEventBoolValue(defaults)}
	{ "TRUCK_CRUISE_CONTROL_READJUST", EV_TRUCK_CRUISE_CONTROL_READJUST, "Keyboard EXPL+CTRL+SPACE", _LW("match target speed / rpm with current truck speed / rpm") }, // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_CRUISE_CONTROL_ACCL",     EV_TRUCK_CRUISE_CONTROL_ACCL,     "Keyboard EXPL+CTRL+R",     _LW("increase target speed / rpm") },      // LandVehicleSimulation{ getEventBoolValue(defaults)}
	{ "TRUCK_CRUISE_CONTROL_DECL",     EV_TRUCK_CRUISE_CONTROL_DECL,     "Keyboard EXPL+CTRL+F",     _LW("decrease target speed / rpm") },      // LandVehicleSimulation{ getEventBoolValue(defaults)}
	{ "TRUCK_SHIFT_DOWN",              EV_TRUCK_SHIFT_DOWN,              "Keyboard Z",               _LW("shift one gear down in manual transmission mode") },   // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_SHIFT_NEUTRAL",           EV_TRUCK_SHIFT_NEUTRAL,           "Keyboard D",               _LW("shift to neutral gear in manual transmission mode") }, // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_SHIFT_UP",                EV_TRUCK_SHIFT_UP,                "Keyboard A",               _LW("shift one gear up in manual transmission mode") },     // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_SHIFT_GEAR_REVERSE",      EV_TRUCK_SHIFT_GEAR_REVERSE,      "",                         _LW("shift directly to reverse gear") },   // LandVehicleSimulation{ 2x getEventBoolValue(defaults)}

	// ~~~ Usage: LandVehicleSimulation { 4x LOOP { getEventBoolValue(defaults) } }
	{ "TRUCK_SHIFT_GEAR1",  EV_TRUCK_SHIFT_GEAR01, "", _LW("shift directly to first gear") },
	{ "TRUCK_SHIFT_GEAR2",  EV_TRUCK_SHIFT_GEAR02, "", _LW("shift directly to second gear") },
	{ "TRUCK_SHIFT_GEAR3",  EV_TRUCK_SHIFT_GEAR03, "", _LW("shift directly to third gear") },
	{ "TRUCK_SHIFT_GEAR4",  EV_TRUCK_SHIFT_GEAR04, "", _LW("shift directly to fourth gear") },
	{ "TRUCK_SHIFT_GEAR5",  EV_TRUCK_SHIFT_GEAR05, "", _LW("shift directly to 5th gear") },
	{ "TRUCK_SHIFT_GEAR6",  EV_TRUCK_SHIFT_GEAR06, "", _LW("shift directly to 6th gear") },
	{ "TRUCK_SHIFT_GEAR7",  EV_TRUCK_SHIFT_GEAR07, "", _LW("shift directly to 7th gear") },
	{ "TRUCK_SHIFT_GEAR8",  EV_TRUCK_SHIFT_GEAR08, "", _LW("shift directly to 8th gear") },
	{ "TRUCK_SHIFT_GEAR9",  EV_TRUCK_SHIFT_GEAR09, "", _LW("shift directly to 9th gear") },
	{ "TRUCK_SHIFT_GEAR10", EV_TRUCK_SHIFT_GEAR10, "", _LW("shift directly to 10th gear") },
	{ "TRUCK_SHIFT_GEAR11", EV_TRUCK_SHIFT_GEAR11, "", _LW("shift directly to 11th gear") },
	{ "TRUCK_SHIFT_GEAR12", EV_TRUCK_SHIFT_GEAR12, "", _LW("shift directly to 12th gear") },
	{ "TRUCK_SHIFT_GEAR13", EV_TRUCK_SHIFT_GEAR13, "", _LW("shift directly to 13th gear") },
	{ "TRUCK_SHIFT_GEAR14", EV_TRUCK_SHIFT_GEAR14, "", _LW("shift directly to 14th gear") },
	{ "TRUCK_SHIFT_GEAR15", EV_TRUCK_SHIFT_GEAR15, "", _LW("shift directly to 15th gear") },
	{ "TRUCK_SHIFT_GEAR16", EV_TRUCK_SHIFT_GEAR16, "", _LW("shift directly to 16th gear") },
	{ "TRUCK_SHIFT_GEAR17", EV_TRUCK_SHIFT_GEAR17, "", _LW("shift directly to 17th gear") },
	{ "TRUCK_SHIFT_GEAR18", EV_TRUCK_SHIFT_GEAR18, "", _LW("shift directly to 18th gear") },

	{ "TRUCK_SHIFT_LOWRANGE",         EV_TRUCK_SHIFT_LOWRANGE,         "",                           _LW("sets low range (1-6) for H-shaft") },    // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_SHIFT_MIDRANGE",         EV_TRUCK_SHIFT_MIDRANGE,         "",                           _LW("sets middle range (7-12) for H-shaft") },// LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_SHIFT_HIGHRANGE",        EV_TRUCK_SHIFT_HIGHRANGE,        "",                           _LW("sets high range (13-18) for H-shaft") }, // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_STARTER",                EV_TRUCK_STARTER,                "Keyboard S",                 _LW("hold to start the engine") },            // LandVehicleSimulation{ getEventBoolValue(defaults)}
	{ "TRUCK_STEER_LEFT",             EV_TRUCK_STEER_LEFT,             "Keyboard LEFT",              _LW("steer left") },  // [ARGS!!] LandVehicleSimulation{ getEventValue(pure=false, source=Analog); getEventValue(pure=false, source=Digital) }
	{ "TRUCK_STEER_RIGHT",            EV_TRUCK_STEER_RIGHT,            "Keyboard RIGHT",             _LW("steer right") }, // [ARGS!!] LandVehicleSimulation{ getEventValue(pure=false, source=Analog); getEventValue(pure=false, source=Digital) }
	{ "TRUCK_SWITCH_SHIFT_MODES",     EV_TRUCK_SWITCH_SHIFT_MODES,     "Keyboard Q",                 _LW("toggle between transmission modes") },          // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_TOGGLE_AXLE_LOCK",       EV_TRUCK_TOGGLE_AXLE_LOCK,       "Keyboard W",                 _LW("Cycle between available differental models") }, // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_TOGGLE_CONTACT",         EV_TRUCK_TOGGLE_CONTACT,         "Keyboard X",                 _LW("toggle ignition") },        // LandVehicleSimulation{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_TOGGLE_FORWARDCOMMANDS", EV_TRUCK_TOGGLE_FORWARDCOMMANDS, "Keyboard EXPL+CTRL+SHIFT+F", _LW("toggle forwardcommands") }, // RoRFrameListener{ getEventBoolValueBounce(defaults)}
	{ "TRUCK_TOGGLE_IMPORTCOMMANDS",  EV_TRUCK_TOGGLE_IMPORTCOMMANDS,  "Keyboard EXPL+CTRL+SHIFT+I", _LW("toggle importcommands") },  // RoRFrameListener{ getEventBoolValueBounce(defaults)}

	// "new" commands

	// ~~~ Usage: misc.
	{ "COMMON_FULLSCREEN_TOGGLE",    EV_COMMON_FULLSCREEN_TOGGLE,    "Keyboard EXPL+ALT+RETURN",     _LW("toggle between windowed and fullscreen mode") }, // RoRFrameListener{ getEventBoolValueBounce(time=2.f) }
	{ "CAMERA_FREE_MODE_FIX",        EV_CAMERA_FREE_MODE_FIX,        "Keyboard EXPL+ALT+C",          _LW("fix the camera to a position") },      // CameraManager{ getEventBoolValueBounce(defaults) }
	{ "CAMERA_FREE_MODE",            EV_CAMERA_FREE_MODE,            "Keyboard EXPL+SHIFT+C",        _LW("enable / disable free camera mode") }, // CameraManager{ getEventBoolValueBounce(defaults) }
	{ "TRUCK_LEFT_MIRROR_LEFT",      EV_TRUCK_LEFT_MIRROR_LEFT,      "Keyboard EXPL+SEMICOLON",      _LW("move left mirror to the left") },      // LandVehicleSimulation{ getEventBoolValueBounce(defaults) }
	{ "TRUCK_LEFT_MIRROR_RIGHT",     EV_TRUCK_LEFT_MIRROR_RIGHT,     "Keyboard EXPL+CTRL+SEMICOLON", _LW("move left mirror to the right") },     // LandVehicleSimulation{ getEventBoolValueBounce(defaults) }
	{ "TRUCK_RIGHT_MIRROR_LEFT",     EV_TRUCK_RIGHT_MIRROR_LEFT,     "Keyboard EXPL+COLON",          _LW("more right mirror to the left") },     // LandVehicleSimulation{ getEventBoolValueBounce(defaults) }
	{ "TRUCK_RIGHT_MIRROR_RIGHT",    EV_TRUCK_RIGHT_MIRROR_RIGHT,    "Keyboard EXPL+CTRL+COLON",     _LW("move right mirror to the right") },    // LandVehicleSimulation{ getEventBoolValueBounce(defaults) }
	{ "COMMON_REPLAY_FORWARD",       EV_COMMON_REPLAY_FORWARD,       "Keyboard EXPL+RIGHT",          _LW("more replay forward") },       // AircraftSimulation+RoRFrameListener{ getEventBoolValueBounce(time=0.1) }
	{ "COMMON_REPLAY_BACKWARD",      EV_COMMON_REPLAY_BACKWARD,      "Keyboard EXPL+LEFT",           _LW("more replay backward") },      // AircraftSimulation+RoRFrameListener{ getEventBoolValueBounce(time=0.1) }
	{ "COMMON_REPLAY_FAST_FORWARD",  EV_COMMON_REPLAY_FAST_FORWARD,  "Keyboard EXPL+SHIFT+RIGHT",    _LW("move replay fast forward") },  // AircraftSimulation+RoRFrameListener{ getEventBoolValueBounce(time=0.1) }
	{ "COMMON_REPLAY_FAST_BACKWARD", EV_COMMON_REPLAY_FAST_BACKWARD, "Keyboard EXPL+SHIFT+LEFT",     _LW("move replay fast backward") }, // AircraftSimulation+RoRFrameListener{ getEventBoolValueBounce(time=0.1) }

	// ~~~ Unused
	{ "AIRPLANE_AIRBRAKES_NONE", EV_AIRPLANE_AIRBRAKES_NONE, "Keyboard CTRL+3", _LW("no airbrakes") },
	{ "AIRPLANE_AIRBRAKES_FULL", EV_AIRPLANE_AIRBRAKES_FULL, "Keyboard CTRL+4", _LW("full airbrakes") },
	{ "AIRPLANE_AIRBRAKES_LESS", EV_AIRPLANE_AIRBRAKES_LESS, "Keyboard EXPL+3", _LW("less airbrakes") },
	{ "AIRPLANE_AIRBRAKES_MORE", EV_AIRPLANE_AIRBRAKES_MORE, "Keyboard EXPL+4", _LW("more airbrakes") },

	// ~~~ Misc
	{ "AIRPLANE_THROTTLE",     EV_AIRPLANE_THROTTLE,     "",                                _LW("airplane throttle") },        // AircraftSimulation{ getEventBoolValue(defaults) }
	{ "COMMON_TRUCK_REMOVE",   EV_COMMON_TRUCK_REMOVE,   "Keyboard EXPL+CTRL+SHIFT+DELETE", _LW("delete current truck") },     // RoRFrameListener{ getEventBoolValueBounce(defaults) }
	{ "COMMON_NETCHATDISPLAY", EV_COMMON_NETCHATDISPLAY, "Keyboard EXPL+SHIFT+U",           _LW("display or hide net chat") }, // Unused!
	{ "COMMON_NETCHATMODE",    EV_COMMON_NETCHATMODE,    "Keyboard EXPL+CTRL+U",            _LW("toggle between net chat display modes") }, // Unused!
	{ "CHARACTER_ROT_UP",      EV_CHARACTER_ROT_UP,      "Keyboard UP",                     _LW("rotate view up") },           // Character::update() { getEventValue(defaults) }
	{ "CHARACTER_ROT_DOWN",    EV_CHARACTER_ROT_DOWN,    "Keyboard DOWN",                   _LW("rotate view down") },         // Character::update() { getEventValue(defaults) }
	{ "CAMERA_UP",             EV_CAMERA_UP,             "Keyboard Q",                      _LW("move camera up") },           // CameraBehaviorFree{ getEventBoolValue(defaults) }
	{ "CAMERA_DOWN",           EV_CAMERA_DOWN,           "Keyboard Z",                      _LW("move camera down") },         // CameraBehaviorFree{ getEventBoolValue(defaults) }

	// now position storage
	// ~~~ Usage: RoRFrameListener{ getEventBoolValueBounce(time=0.5) }
	{ "TRUCK_SAVE_POS1",  EV_TRUCK_SAVE_POS01, "Keyboard EXPL+ALT+CTRL+1", _LW("save position as slot 1") }, 
	{ "TRUCK_SAVE_POS2",  EV_TRUCK_SAVE_POS02, "Keyboard EXPL+ALT+CTRL+2", _LW("save position as slot 2") },
	{ "TRUCK_SAVE_POS3",  EV_TRUCK_SAVE_POS03, "Keyboard EXPL+ALT+CTRL+3", _LW("save position as slot 3") },
	{ "TRUCK_SAVE_POS4",  EV_TRUCK_SAVE_POS04, "Keyboard EXPL+ALT+CTRL+4", _LW("save position as slot 4") },
	{ "TRUCK_SAVE_POS5",  EV_TRUCK_SAVE_POS05, "Keyboard EXPL+ALT+CTRL+5", _LW("save position as slot 5") },
	{ "TRUCK_SAVE_POS6",  EV_TRUCK_SAVE_POS06, "Keyboard EXPL+ALT+CTRL+6", _LW("save position as slot 6") },
	{ "TRUCK_SAVE_POS7",  EV_TRUCK_SAVE_POS07, "Keyboard EXPL+ALT+CTRL+7", _LW("save position as slot 7") },
	{ "TRUCK_SAVE_POS8",  EV_TRUCK_SAVE_POS08, "Keyboard EXPL+ALT+CTRL+8", _LW("save position as slot 8") },
	{ "TRUCK_SAVE_POS9",  EV_TRUCK_SAVE_POS09, "Keyboard EXPL+ALT+CTRL+9", _LW("save position as slot 9") },
	{ "TRUCK_SAVE_POS10", EV_TRUCK_SAVE_POS10, "Keyboard EXPL+ALT+CTRL+0", _LW("save position as slot 10") },

	// ~~~ Usage: RoRFrameListener{ getEventBoolValueBounce(time=0.5) }
	{ "TRUCK_LOAD_POS1",  EV_TRUCK_LOAD_POS01, "Keyboard EXPL+ALT+1", _LW("load position under slot 1") },
	{ "TRUCK_LOAD_POS2",  EV_TRUCK_LOAD_POS02, "Keyboard EXPL+ALT+2", _LW("load position under slot 2") },
	{ "TRUCK_LOAD_POS3",  EV_TRUCK_LOAD_POS03, "Keyboard EXPL+ALT+3", _LW("load position under slot 3") },
	{ "TRUCK_LOAD_POS4",  EV_TRUCK_LOAD_POS04, "Keyboard EXPL+ALT+4", _LW("load position under slot 4") },
	{ "TRUCK_LOAD_POS5",  EV_TRUCK_LOAD_POS05, "Keyboard EXPL+ALT+5", _LW("load position under slot 5") },
	{ "TRUCK_LOAD_POS6",  EV_TRUCK_LOAD_POS06, "Keyboard EXPL+ALT+6", _LW("load position under slot 6") },
	{ "TRUCK_LOAD_POS7",  EV_TRUCK_LOAD_POS07, "Keyboard EXPL+ALT+7", _LW("load position under slot 7") },
	{ "TRUCK_LOAD_POS8",  EV_TRUCK_LOAD_POS08, "Keyboard EXPL+ALT+8", _LW("load position under slot 8") },
	{ "TRUCK_LOAD_POS9",  EV_TRUCK_LOAD_POS09, "Keyboard EXPL+ALT+9", _LW("load position under slot 9") },
	{ "TRUCK_LOAD_POS10", EV_TRUCK_LOAD_POS10, "Keyboard EXPL+ALT+0", _LW("load position under slot 10") },

	// ~~~ Unused
	{ "DOF_TOGGLE",                  EV_DOF_TOGGLE,                  "Keyboard EXPL+CTRL+D", _LW("turn on Depth of Field on or off") },
	{ "DOF_DEBUG",                   EV_DOF_DEBUG,                   "Keyboard EXPL+ALT+D",  _LW("turn on the Depth of field debug view") },
	{ "DOF_DEBUG_TOGGLE_FOCUS_MODE", EV_DOF_DEBUG_TOGGLE_FOCUS_MODE, "Keyboard EXPL+SPACE",  _LW("toggle the DOF focus mode") },
	{ "DOF_DEBUG_ZOOM_IN",           EV_DOF_DEBUG_ZOOM_IN,           "Keyboard EXPL+Q",      _LW("zoom in") },
	{ "DOF_DEBUG_ZOOM_OUT",          EV_DOF_DEBUG_ZOOM_OUT,          "Keyboard EXPL+Z",      _LW("zoom in") },
	{ "DOF_DEBUG_APERTURE_MORE",     EV_DOF_DEBUG_APERTURE_MORE,     "Keyboard EXPL+1",      _LW("more aperture") },
	{ "DOF_DEBUG_APERTURE_LESS",     EV_DOF_DEBUG_APERTURE_LESS,     "Keyboard EXPL+2",      _LW("less aperture") },
	{ "DOF_DEBUG_FOCUS_IN",          EV_DOF_DEBUG_FOCUS_IN,          "Keyboard EXPL+3",      _LW("move focus in") },
	{ "DOF_DEBUG_FOCUS_OUT",         EV_DOF_DEBUG_FOCUS_OUT,         "Keyboard EXPL+4",      _LW("move focus out") },

	// ~~~ Misc
	{ "TRUCKEDIT_RELOAD", EV_TRUCKEDIT_RELOAD, "Keyboard EXPL+SHIFT+CTRL+R", _LW("reload truck") },        // RoRFrameListener{ getEventBoolValueBounce(time=0.5) }
	{ "TOGGLESHADERS",    EV_TOGGLESHADERS,    "Keyboard EXPL+SHIFT+CTRL+S", _LW("toggle shader usage") }, // Unused!
	{ "GETNEWVEHICLE",    EV_GETNEWVEHICLE,    "Keyboard EXPL+CTRL+G",       _LW("get new vehicle") },     // RoRFrameListener{ getEventBoolValueBounce(time=0.5) }

	// the end, DO NOT MODIFY
	{"", -1, "", L""},
};


#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#define strnlen(str,len) strlen(str)
#endif

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <X11/Xlib.h>
//#include <linux/LinuxMouse.h>
#endif

#ifndef NOOGRE
#include "Console.h"
#endif

using namespace std;
using namespace Ogre;
using namespace OIS;

// Constructor takes a RenderWindow because it uses that to determine input context
InputEngine::InputEngine() :
	  m_capture_mode(false)
	, m_ois_joysticks_free_slot(0)
	, m_inputs_changed(true)
	, m_ois_force_feedback(0)
	, m_ois_input_manager(0)
	, m_ois_keyboard(0)
	, m_ois_mouse(0)
	, m_mapping_loaded(false)
{
	for (int i=0;i<MAX_JOYSTICKS;i++) m_ois_joysticks[i]=0;
	LOG("*** Loading OIS ***");
	initAllKeys();

	memset(m_event_values, 0, sizeof(m_event_values));
	memset(m_event_times,  0, sizeof(m_event_times));
}

InputEngine::~InputEngine()
{
	LOG("*** Terminating destructor ***");
	destroy();
}

void InputEngine::destroy()
{
	if ( m_ois_input_manager )
	{
		LOG("*** Terminating OIS ***");
		if (m_ois_mouse)
		{
			m_ois_input_manager->destroyInputObject( m_ois_mouse );
			m_ois_mouse=0;
		}
		if (m_ois_keyboard)
		{
			m_ois_input_manager->destroyInputObject( m_ois_keyboard );
			m_ois_keyboard=0;
		}
		if (m_ois_joysticks)
		{
			for (int i=0;i<MAX_JOYSTICKS;i++)
			{
				if (!m_ois_joysticks[i]) continue;
				m_ois_input_manager->destroyInputObject(m_ois_joysticks[i]);
				m_ois_joysticks[i] = 0;
			}
		}

		OIS::InputManager::destroyInputSystem(m_ois_input_manager);
		m_ois_input_manager = 0;
	}
}


bool InputEngine::setup(String hwnd, bool capture, bool capturemouse, int _grabMode, bool captureKbd)
{
	m_grab_mode = _grabMode;
	
	LOG("*** Initializing OIS ***");

	//try to delete old ones first (linux can only handle one at a time)
	destroy();
	m_capture_mode = capture;
	if (m_capture_mode)
	{
		//size_t hWnd = 0;
		//win->getCustomAttribute("WINDOW", &hWnd);
		ParamList pl;
		
#if 0 //OGRE_PLATFORM == OGRE_PLATFORM_LINUX
		// we get the ogre way of defining the handle, extract the window HWND only
		int screen=0,app=0,windowhandle=0;
		sscanf(hwnd.c_str(), "%d:%d:%d", &screen, &app, &windowhandle);
		hwnd = TOSTRING(windowhandle);
		printf("OIS windowhandle = %s\n", hwnd.c_str());
#endif // LINUX
		
		pl.insert(OIS::ParamList::value_type("WINDOW", hwnd));
		if (m_grab_mode == GRAB_ALL)
		{
		} else if (m_grab_mode == GRAB_DYNAMICALLY || m_grab_mode == GRAB_NONE)
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
			pl.insert(OIS::ParamList::value_type("x11_mouse_hide", "false"));
			pl.insert(OIS::ParamList::value_type("XAutoRepeatOn", "false"));
			pl.insert(OIS::ParamList::value_type("x11_mouse_grab", "false"));
			pl.insert(OIS::ParamList::value_type("x11_keyboard_grab", "false"));
#else
			pl.insert(OIS::ParamList::value_type("w32_mouse", "DISCL_FOREGROUND"));
			pl.insert(OIS::ParamList::value_type("w32_mouse", "DISCL_NONEXCLUSIVE"));
			pl.insert(OIS::ParamList::value_type("w32_keyboard", "DISCL_FOREGROUND"));
			pl.insert(OIS::ParamList::value_type("w32_keyboard", "DISCL_NONEXCLUSIVE"));
#endif // LINUX
		}

		LOG("*** OIS WINDOW: "+hwnd);

		m_ois_input_manager = OIS::InputManager::createInputSystem(pl);

		//Print debugging information
		unsigned int v = m_ois_input_manager->getVersionNumber();
		LOG("OIS Version: " + TOSTRING(v>>16) + String(".") + TOSTRING((v>>8) & 0x000000FF) + String(".") + TOSTRING(v & 0x000000FF));
		LOG("+ Release Name: " + m_ois_input_manager->getVersionName());
		LOG("+ Manager: " + m_ois_input_manager->inputSystemName());
		LOG("+ Total Keyboards: " + TOSTRING(m_ois_input_manager->getNumberOfDevices(OISKeyboard)));
		LOG("+ Total Mice: " + TOSTRING(m_ois_input_manager->getNumberOfDevices(OISMouse)));
		LOG("+ Total JoySticks: " + TOSTRING(m_ois_input_manager->getNumberOfDevices(OISJoyStick)));

		//List all devices
		OIS::DeviceList deviceList = m_ois_input_manager->listFreeDevices();
		for (OIS::DeviceList::iterator i = deviceList.begin(); i != deviceList.end(); ++i )
			LOG("* Device: " + String(mOISDeviceType[i->first]) + String(" Vendor: ") + i->second);

		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		m_ois_keyboard=0;
		if (captureKbd)
		{
			try
			{
				m_ois_keyboard = static_cast<Keyboard*>(m_ois_input_manager->createInputObject( OISKeyboard, true ));
				m_ois_keyboard->setTextTranslation(OIS::Keyboard::Unicode);
			} catch(OIS::Exception &ex)
			{
				LOG(String("Exception raised on keyboard creation: ") + String(ex.eText));
			}
		}


		try
		{
			//This demo uses at most 10 joysticks - use old way to create (i.e. disregard vendor)
			int numSticks = std::min(m_ois_input_manager->getNumberOfDevices(OISJoyStick), 10);
			m_ois_joysticks_free_slot = 0;
			for (int i = 0; i < numSticks; ++i)
			{
				m_ois_joysticks[i] = (JoyStick*)m_ois_input_manager->createInputObject(OISJoyStick, true);
				m_ois_joysticks[i]->setEventCallback(this);
				m_ois_joysticks_free_slot++;
				//create force feedback too
				//here, we take the first device we can get, but we could take a device index
				if (!m_ois_force_feedback) m_ois_force_feedback = (OIS::ForceFeedback*)m_ois_joysticks[i]->queryInterface(OIS::Interface::ForceFeedback );

#ifndef NOOGRE
				LOG("Creating Joystick " + TOSTRING(i + 1) + " (" + m_ois_joysticks[i]->vendor() + ")");
				LOG("* Axes: " + TOSTRING(m_ois_joysticks[i]->getNumberOfComponents(OIS_Axis)));
				LOG("* Sliders: " + TOSTRING(m_ois_joysticks[i]->getNumberOfComponents(OIS_Slider)));
				LOG("* POV/HATs: " + TOSTRING(m_ois_joysticks[i]->getNumberOfComponents(OIS_POV)));
				LOG("* Buttons: " + TOSTRING(m_ois_joysticks[i]->getNumberOfComponents(OIS_Button)));
				LOG("* Vector3: " + TOSTRING(m_ois_joysticks[i]->getNumberOfComponents(OIS_Vector3)));
#endif //NOOGRE
			}
		}
#ifndef NOOGRE
		catch(OIS::Exception &ex)
		{
			LOG(String("Exception raised on joystick creation: ") + String(ex.eText));
		}
#else  //NOOGRE
		catch(...)
		{
		}
#endif //NOOGRE

		if (capturemouse)
		{
			try
			{
				m_ois_mouse = static_cast<Mouse*>(m_ois_input_manager->createInputObject( OISMouse, true ));
			} catch(OIS::Exception &ex)
			{
				LOG(String("Exception raised on mouse creation: ") + String(ex.eText));
			}
		}

		//Set initial mouse clipping size
		//windowResized(win);

		// set Callbacks
		if (m_ois_keyboard)
			m_ois_keyboard->setEventCallback(this);
		if (capturemouse && m_ois_mouse)
		{
			m_ois_mouse->setEventCallback(this);

			// init states (not required for keyboard)
			m_mouse_state = m_ois_mouse->getMouseState();
		}
		if (m_ois_joysticks_free_slot)
		{
			for (int i=0;i<m_ois_joysticks_free_slot;i++)
				m_joystick_states[i] = m_ois_joysticks[i]->getJoyStickState();
		}

		// set the mouse to the middle of the screen, hackish!
#if _WIN32
		// under linux, this will not work and the cursor will never reach (0,0)
		if (m_ois_mouse && RoR::Application::GetOgreSubsystem()->GetRenderWindow())
		{
			OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(m_ois_mouse->getMouseState());
			mutableMouseState.X.abs = RoR::Application::GetOgreSubsystem()->GetRenderWindow()->getWidth()  * 0.5f;
			mutableMouseState.Y.abs = RoR::Application::GetOgreSubsystem()->GetRenderWindow()->getHeight() * 0.5f;
		}
#endif // _WIN32
	}
	//this becomes more and more convoluted!
#ifdef NOOGRE
	// we will load the mapping manually
#else //NOOGRE
	if (!m_mapping_loaded)
	{
		// load default one
		loadMapping(CONFIGFILENAME);

#ifndef NOOGRE
		// then load device specific ones
		for (int i = 0; i < m_ois_joysticks_free_slot; ++i)
		{
			String deviceStr = m_ois_joysticks[i]->vendor();
			
			// care about unsuitable chars
			String repl = "\\/ #@?!$%^&*()+=-><.:'|\";";
			for(unsigned int c = 0; c < repl.size(); c++)
			{
				deviceStr = StringUtil::replaceAll(deviceStr, repl.substr(c,1), "_");
			}
			deviceStr += ".map";
			
			loadMapping(deviceStr, true, i);
		}
#endif //NOOGRE
		m_mapping_loaded = true;
		completeMissingEvents();

		return false;
	}
#endif //NOOGRE
	return true;
}

void InputEngine::grabMouse(bool enable)
{
	static int lastmode = -1;
	if (!m_ois_mouse)
		return;
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	if ((enable && lastmode == 0) || (!enable && lastmode == 1) || (lastmode == -1))
	{
		LOG("*** mouse grab: " + TOSTRING(enable));
		//((LinuxMouse *)m_ois_mouse)->grab(enable);
		lastmode = enable?1:0;
	}
#endif
}

void InputEngine::hideMouse(bool visible)
{
	static int mode = -1;
	if (!m_ois_mouse)
		return;
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	if ((visible && mode == 0) || (!visible && mode == 1) || mode == -1)
	{
		//((LinuxMouse *)m_ois_mouse)->hide(visible);
		mode = visible?1:0;
	}
#endif
}

void InputEngine::setMousePosition(int x, int y, bool padding)
{
	if (!m_ois_mouse)
		return;
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	// padding ensures that the mouse has a safety area at the window's borders
//	((LinuxMouse *)m_ois_mouse)->setMousePosition(x, y, padding);
#endif
}

OIS::MouseState InputEngine::getMouseState()
{
	static float mX=999999, mY=999999;
	static int mode = -1;
	if (mode == -1)
	{
#ifndef NOOGRE
		// try to find the correct location!
		mX = FSETTING("MouseSensX", 1);
		mY = FSETTING("MouseSensY", 1);
		LOG("Mouse X sens: " + TOSTRING((Real)mX));
		LOG("Mouse Y sens: " + TOSTRING((Real)mY));
		mode = 1;
		if (mX == 0 || mY ==0)
			mode = 2;
#else
		// no scaling without ogre
		mode = 2;
#endif
	}

	OIS::MouseState m;
	if (m_ois_mouse)
	{
		m = m_ois_mouse->getMouseState();
		if (mode == 1)
		{
			m.X.rel = (int)((float)m.X.rel * mX);
			m.Y.rel = (int)((float)m.X.rel * mY);
		}
	}
	return m;
}

String InputEngine::getKeyNameForKeyCode(OIS::KeyCode keycode)
{
	if (keycode == KC_LSHIFT || keycode == KC_RSHIFT)
		return "SHIFT";
	if (keycode == KC_LCONTROL || keycode == KC_RCONTROL)
		return "CTRL";
	if (keycode == KC_LMENU || keycode == KC_RMENU)
		return "ALT";
	for (auto allit = m_all_keys.begin();allit != m_all_keys.end();allit++)
	{
		if (allit->second == keycode)
			return allit->first;
	}
	return "unknown";
}

void InputEngine::Capture()
{
	if (m_ois_keyboard)
	{
		m_ois_keyboard->capture();
	}

	if (m_ois_mouse)
	{
		m_ois_mouse->capture();
	}

	for (int i=0; i < m_ois_joysticks_free_slot; i++)
	{
		if (m_ois_joysticks[i])
		{
			m_ois_joysticks[i]->capture();
		}
	}
}

void InputEngine::windowResized(Ogre::RenderWindow* rw)
{
	if (!m_ois_mouse)
		return;
	//update mouse area
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);
	const OIS::MouseState &ms = m_ois_mouse->getMouseState();
	ms.width = width;
	ms.height = height;
#ifdef USE_MYGUI
	RoR::Application::GetGuiManager()->windowResized(rw);
#endif //MYGUI
}

void InputEngine::SetKeyboardListener(OIS::KeyListener* keyboard_listener)
{
	assert (m_ois_keyboard != nullptr);
	m_ois_keyboard->setEventCallback(keyboard_listener);
}

OIS::MouseState InputEngine::SetMouseListener(OIS::MouseListener* mouse_listener)
{
	assert (m_ois_mouse != nullptr);
	m_ois_mouse->setEventCallback(mouse_listener);
	return m_ois_mouse->getMouseState();
}

void InputEngine::RestoreMouseListener()
{
	if (m_ois_mouse)
	{
		m_ois_mouse->setEventCallback(this);

		// init states (not required for keyboard)
		m_mouse_state = m_ois_mouse->getMouseState();
	}
}

void InputEngine::RestoreKeyboardListener()
{
	SetKeyboardListener(this);
}

/* --- Joystik Events ------------------------------------------ */
bool InputEngine::buttonPressed( const OIS::JoyStickEvent &arg, int button )
{
	m_inputs_changed=true;
	//LOG("*** buttonPressed " + TOSTRING(button));
	int i = arg.device->getID();
	if (i < 0 || i >= MAX_JOYSTICKS) i = 0;
	m_joystick_states[i] = arg.state;
	return true;
}

bool InputEngine::buttonReleased( const OIS::JoyStickEvent &arg, int button )
{
	m_inputs_changed=true;
	//LOG("*** buttonReleased " + TOSTRING(button));
	int i = arg.device->getID();
	if (i < 0 || i >= MAX_JOYSTICKS) i = 0;
	m_joystick_states[i] = arg.state;
	return true;
}

bool InputEngine::axisMoved( const OIS::JoyStickEvent &arg, int axis )
{
	m_inputs_changed=true;
	//LOG("*** axisMoved " + TOSTRING(axis) + " / " + TOSTRING((int)(arg.state.mAxes[axis].abs / (float)(m_ois_joysticks->MAX_AXIS/100))));
	int i = arg.device->getID();
	if (i < 0 || i >= MAX_JOYSTICKS) i = 0;
	m_joystick_states[i] = arg.state;
	return true;
}

bool InputEngine::sliderMoved( const OIS::JoyStickEvent &arg, int )
{
	m_inputs_changed=true;
	//LOG("*** sliderMoved");
	int i = arg.device->getID();
	if (i < 0 || i >= MAX_JOYSTICKS) i = 0;
	m_joystick_states[i] = arg.state;
	return true;
}

bool InputEngine::povMoved( const OIS::JoyStickEvent &arg, int )
{
	m_inputs_changed=true;
	//LOG("*** povMoved");
	int i = arg.device->getID();
	if (i < 0 || i >= MAX_JOYSTICKS) i = 0;
	m_joystick_states[i] = arg.state;
	return true;
}

/* --- Key Events ------------------------------------------ */
bool InputEngine::keyPressed( const OIS::KeyEvent &arg )
{
	if (RoR::Application::GetGuiManager()->keyPressed(arg))
	{
		return true;
	}

	if (m_keys_pressed[arg.key] != KEY_PRESSED)
	{
		m_inputs_changed=true;
	}
	m_keys_pressed[arg.key] = KEY_PRESSED;

	return true;
}

bool InputEngine::keyReleased( const OIS::KeyEvent &arg )
{
	if (RoR::Application::GetGuiManager()->keyReleased(arg))
	{
		return true;
	}

	if (m_keys_pressed[arg.key] != KEY_NOT_PRESSED)
	{
		m_inputs_changed=true;
	}
	m_keys_pressed[arg.key] = KEY_NOT_PRESSED;

	return true;
}

/* --- Mouse Events ------------------------------------------ */
bool InputEngine::mouseMoved( const OIS::MouseEvent &arg )
{
#ifdef USE_MYGUI
	if (RoR::Application::GetGuiManager()->mouseMoved(arg))
		return true;
#endif //MYGUI
	//LOG("*** mouseMoved");
	m_inputs_changed=true;
	m_mouse_state = arg.state;
	return true;
}

bool InputEngine::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
#ifdef USE_MYGUI
	if (RoR::Application::GetGuiManager()->mousePressed(arg, id))
		return true;
#endif //MYGUI
	//LOG("*** mousePressed");
	m_inputs_changed=true;
	m_mouse_state = arg.state;
	return true;
}

bool InputEngine::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
#ifdef USE_MYGUI
	if (RoR::Application::GetGuiManager()->mouseReleased(arg, id))
		return true;
#endif //MYGUI
	//LOG("*** mouseReleased");
	m_inputs_changed=true;
	m_mouse_state = arg.state;
	return true;
}

/* --- Custom Methods ------------------------------------------ */
void InputEngine::prepareShutdown()
{
	LOG("*** Inputsystem prepare for shutdown ***");
	destroy();
}


void InputEngine::resetKeys()
{
	memset(&m_keys_pressed, KEY_NOT_PRESSED, sizeof(char) * KEY_MAPPING_ARRAY_SIZE);
}

bool InputEngine::getEventBoolValue(int eventID)
{
	return (getEventValue(eventID) > 0.5f);
}

bool InputEngine::getEventBoolValueBounce(int eventID, float time)
{
	if (m_event_times[eventID] > 0)
		return false;
	else
	{
		bool res = getEventBoolValue(eventID);
		if (res) m_event_times[eventID] = time;
		return res;
	}
}

float InputEngine::getEventBounceTime(int eventID)
{
	return m_event_times[eventID];
}

void InputEngine::updateKeyBounces(float dt)
{
	for (int i = 0; i < EVENT_MAPPING_ARRAY_SIZE; ++i)
	{
		m_event_times[i] = (m_event_times[i] > 0.f) ? (m_event_times[i] - dt) : (m_event_times[i]);
	}
}

float InputEngine::deadZone(float axisValue, float dz)
{
	// no deadzone?
	if (dz < 0.0001f)
		return axisValue;

	// check for deadzone
	if (fabs(axisValue) < dz)
		// dead zone case
		return 0.0f;
	else
		// non-deadzone, remap the remaining part
		return (axisValue - dz) * (1.0f / (1.0f-dz));
}

float InputEngine::axisLinearity(float axisValue, float linearity)
{
	return (axisValue * linearity);
}

float InputEngine::logval(float val)
{
	if (val>0) return log10(1.0/(1.1-val))/1.0;
	if (val==0) return 0;
	return -log10(1.0/(1.1+val))/1.0;
}

void InputEngine::smoothValue(float &ref, float value, float rate)
{
	if (value < -1) value = -1;
	if (value > 1) value = 1;
	// smooth!
	if (ref > value)
	{
		ref -= rate;
		if (ref < value)
			ref = value;
	}
	else if (ref < value)
		ref += rate;
}

String InputEngine::getEventCommand(int event_id)
{
	EventTrigger* t = GetFirstTriggerForEvent(event_id);
	if (t != nullptr)
	{
		return String(t->configline);
	}
	return String();
}

bool InputEngine::isEventDefined(int event_id)
{
	EventTrigger* t = GetFirstTriggerForEvent(event_id);
	return (t != nullptr && t->eventtype != ET_NONE);
}

int InputEngine::getKeboardKeyForCommand(int event_id)
{
	EventTrigger* t = GetFirstTriggerForEvent(event_id);
	if (t != nullptr && t->eventtype == ET_Keyboard)
	{
		return t->keyCode;
	}
	return -1;
}

float InputEngine::GetEventValuePureAnalog(int event_id)
{
	auto itor_end = m_event_triggers.end();
	for (auto itor = m_event_triggers.begin(); itor != itor_end; ++itor)
	{
		float value = 0.f;
		EventTrigger & t = *itor;
		if (t.GetEventId() != event_id)
		{
			continue;
		}

		switch (t.eventtype)
		{
			case ET_MouseAxisX:
				value = m_mouse_state.X.abs / 32767;
				break;
			case ET_MouseAxisY:
				value = m_mouse_state.Y.abs / 32767;
				break;
			case ET_MouseAxisZ:
				value = m_mouse_state.Z.abs / 32767;
				break;
				
			case ET_JoystickAxisRel:
			case ET_JoystickAxisAbs:
				{
					if (t.joystickNumber > m_ois_joysticks_free_slot || !m_ois_joysticks[t.joystickNumber])
					{
						value=0;
						continue;
					}
					if (t.joystickAxisNumber >= (int)m_joystick_states[t.joystickNumber].mAxes.size())
					{
#ifndef NOOGRE
						LOG("*** Joystick has not enough axis for mapping: need axe "+TOSTRING(t.joystickAxisNumber) + ", availabe axis: "+TOSTRING(m_joystick_states[t.joystickNumber].mAxes.size()));
#endif
						value=0;
						continue;
					}
					Axis axe = m_joystick_states[t.joystickNumber].mAxes[t.joystickAxisNumber];

					if (t.eventtype == ET_JoystickAxisRel)
					{
						value = (float)axe.rel / (float)m_ois_joysticks[t.joystickNumber]->MAX_AXIS;
					}
					else
					{
						value = (float)axe.abs / (float)m_ois_joysticks[t.joystickNumber]->MAX_AXIS;
						switch(t.joystickAxisRegion)
						{
						case 0:
							// normal case, full axis used
							value = (value + 1)/2;
							break;
						case -1:
							// lower range used
							if (value > 0)
								value = 0;
							else
								value = -value;
							break;
						case 1:
							// upper range used
							if (value < 0)
								value = 0;
							break;
						}

						if (t.joystickAxisHalf)
						{
							// XXX: TODO: write this
							//float a = (double)((value+1.0)/2.0);
							//float b = (double)(1.0-(value+1.0)/2.0);
							//LOG("half: "+TOSTRING(value)+" / "+TOSTRING(a)+" / "+TOSTRING(b));
							//no dead zone in half axis
							value = (1.0 + value) / 2.0;
							if (t.joystickAxisReverse)
							{
								value = 1.0 - value;
							}
						}
						else
						{
							//LOG("not half: "+TOSTRING(value)+" / "+TOSTRING(deadZone(value, t.joystickAxisDeadzone)) +" / "+TOSTRING(t.joystickAxisDeadzone) );
							if (t.joystickAxisReverse)
							{
								value = 1-value;
							}
						}
						// digital mapping of analog axis
						if (t.joystickAxisUseDigital)
						{
							if (value >= 0.5)
							{
								value = 1;
							}
							else
							{
								value = 0;
							}
						}
					}
				}
				break;
			case ET_JoystickSliderX:
			case ET_JoystickSliderY:
				{
					if (t.joystickNumber > m_ois_joysticks_free_slot || !m_ois_joysticks[t.joystickNumber])
					{
						value=0;
						continue;
					}
					if (t.eventtype == ET_JoystickSliderX)
						value = (float)m_joystick_states[t.joystickNumber].mSliders[t.joystickSliderNumber].abX / (float)m_ois_joysticks[t.joystickNumber]->MAX_AXIS;
					else if (t.eventtype == ET_JoystickSliderY)
						value = (float)m_joystick_states[t.joystickNumber].mSliders[t.joystickSliderNumber].abY / (float)m_ois_joysticks[t.joystickNumber]->MAX_AXIS;
					value = (value + 1)/2; // full axis
					if (t.joystickSliderReverse)
						value = 1.0 - value; // reversed
				}
				break;
		}

		if (value > 0.f)
		{
			return value;
		}

	} // FOREACH(m_event_trigger)
	return 0.f;
}

bool InputEngine::isEventAnalog(int event_id)
{
	bool analog = false;
	auto itor_end = m_event_triggers.end();
	for (auto itor = m_event_triggers.begin(); itor != itor_end; ++itor)
	{
		if (itor->GetEventId() == event_id)
		{
			eventtypes ev_type = itor->eventtype;
			//loop through all eventtypes, because we want to find a analog device wether it is the first device or not
			//this means a analog device is always preferred over a digital one
			if ((ev_type == ET_MouseAxisX \
				|| ev_type == ET_MouseAxisY \
				|| ev_type == ET_MouseAxisZ \
				|| ev_type == ET_JoystickAxisAbs \
				|| ev_type == ET_JoystickAxisRel \
				|| ev_type == ET_JoystickSliderX \
				|| ev_type == ET_JoystickSliderY) \
				//check if value comes from analog device
				//this way, only valid events (e.g. joystick mapped, but unplugged) are recognized as analog events
				&& GetEventValuePureAnalog(event_id) != 0.f)
				{
					return true;
				}
		}
	}
	return nullptr;
}

void InputEngine::UpdateEventValues()
{
	// Reset
	memset(m_event_values, 0, sizeof(m_event_values));
	m_event_value_steer_left_digital  = 0.f;
	m_event_value_steer_left_analog   = 0.f;
	m_event_value_steer_right_digital = 0.f;
	m_event_value_steer_right_analog  = 0.f;

	// Loop triggers and resolve events
	auto itor_end = m_event_triggers.end();
	for (auto itor = m_event_triggers.begin(); itor != itor_end; ++itor)
	{
		EventTrigger & t = *itor;
		if(m_event_values[t.GetEventId()] != 0.f)
		{
			continue; // If already resolved, skip.
		}
		bool is_steering_event = ((t.GetEventId() == EV_TRUCK_STEER_LEFT) | (t.GetEventId() == EV_TRUCK_STEER_RIGHT));
		
		float value = 0.f;

		// [IF source { ANY | DIGITAL } ]
		switch(t.eventtype)
		{
			case ET_NONE: 
				break;
			case ET_Keyboard:
				if (!m_keys_pressed[t.keyCode])
				{
					break;
				}

				// only use explicite mapping, if two keys with different modifiers exist, i.e. F1 and SHIFT+F1.
				// check for modificators
				if (t.explicite)
				{
					if (t.ctrl != (m_keys_pressed[KC_LCONTROL] || m_keys_pressed[KC_RCONTROL]))
						break;
					if (t.shift != (m_keys_pressed[KC_LSHIFT] || m_keys_pressed[KC_RSHIFT]))
						break;
					if (t.alt != (m_keys_pressed[KC_LMENU] || m_keys_pressed[KC_RMENU]))
						break;
				} 
				else 
				{
					if (t.ctrl && !(m_keys_pressed[KC_LCONTROL] || m_keys_pressed[KC_RCONTROL]))
						break;
					if (t.shift && !(m_keys_pressed[KC_LSHIFT] || m_keys_pressed[KC_RSHIFT]))
						break;
					if (t.alt && !(m_keys_pressed[KC_LMENU] || m_keys_pressed[KC_RMENU]))
						break;
				}
				value = 1;
				break;
			case ET_MouseButton:
				//if (t.mouseButtonNumber == 0)
				// TODO: FIXME
				value = m_mouse_state.buttonDown(MB_Left);
				break;
			case ET_JoystickButton:
				{
					if (t.joystickNumber > m_ois_joysticks_free_slot || !m_ois_joysticks[t.joystickNumber])
					{
						value=0;
						continue;
					}
					if (t.joystickButtonNumber >= (int)m_ois_joysticks[t.joystickNumber]->getNumberOfComponents(OIS_Button))
					{
#ifndef NOOGRE
							LOG("*** Joystick has not enough buttons for mapping: need button "+TOSTRING(t.joystickButtonNumber) + ", availabe buttons: "+TOSTRING(m_ois_joysticks[t.joystickNumber]->getNumberOfComponents(OIS_Button)));
#endif
						value=0;
						continue;
					}
					value = m_joystick_states[t.joystickNumber].mButtons[t.joystickButtonNumber];
				}
				break;
			case ET_JoystickPov:
				{
					if (t.joystickNumber > m_ois_joysticks_free_slot || !m_ois_joysticks[t.joystickNumber])
					{
						value=0;
						continue;
					}
					if (t.joystickPovNumber >= (int)m_ois_joysticks[t.joystickNumber]->getNumberOfComponents(OIS_POV))
					{
#ifndef NOOGRE
						LOG("*** Joystick has not enough POVs for mapping: need POV "+TOSTRING(t.joystickPovNumber) + ", availabe POVs: "+TOSTRING(m_ois_joysticks[t.joystickNumber]->getNumberOfComponents(OIS_POV)));
#endif
						value=0;
						continue;
					}
					if (m_joystick_states[t.joystickNumber].mPOV[t.joystickPovNumber].direction & t.joystickPovDirection)
						value = 1;
					else
						value = 0;
				}
				break;
		} // switch (t.eventtype)

		if (is_steering_event)
		{
			if (t.GetEventId() == EV_TRUCK_STEER_LEFT)
			{
				m_event_value_steer_left_digital = value;
			}
			else // Steer right
			{
				m_event_value_steer_right_digital = value;
			}
		}

		// [IF source { ANY | ANALOG } ]
		switch (t.eventtype)
		{
			case ET_MouseAxisX:
				value = m_mouse_state.X.abs / 32767;
				break;
			case ET_MouseAxisY:
				value = m_mouse_state.Y.abs / 32767;
				break;
			case ET_MouseAxisZ:
				value = m_mouse_state.Z.abs / 32767;
				break;
				
			case ET_JoystickAxisRel:
			case ET_JoystickAxisAbs:
				{
					if (t.joystickNumber > m_ois_joysticks_free_slot || !m_ois_joysticks[t.joystickNumber])
					{
						value=0;
						continue;
					}
					if (t.joystickAxisNumber >= (int)m_joystick_states[t.joystickNumber].mAxes.size())
					{
#ifndef NOOGRE
						LOG("*** Joystick has not enough axis for mapping: need axe "+TOSTRING(t.joystickAxisNumber) + ", availabe axis: "+TOSTRING(m_joystick_states[t.joystickNumber].mAxes.size()));
#endif
						value=0;
						continue;
					}
					Axis axe = m_joystick_states[t.joystickNumber].mAxes[t.joystickAxisNumber];

					if (t.eventtype == ET_JoystickAxisRel)
					{
						value = (float)axe.rel / (float)m_ois_joysticks[t.joystickNumber]->MAX_AXIS;
					}
					else
					{
						value = (float)axe.abs / (float)m_ois_joysticks[t.joystickNumber]->MAX_AXIS;
						switch(t.joystickAxisRegion)
						{
						case 0:
							// normal case, full axis used
							value = (value + 1)/2;
							break;
						case -1:
							// lower range used
							if (value > 0)
								value = 0;
							else
								value = -value;
							break;
						case 1:
							// upper range used
							if (value < 0)
								value = 0;
							break;
						}

						if (t.joystickAxisHalf)
						{
							// XXX: TODO: write this
							//float a = (double)((value+1.0)/2.0);
							//float b = (double)(1.0-(value+1.0)/2.0);
							//LOG("half: "+TOSTRING(value)+" / "+TOSTRING(a)+" / "+TOSTRING(b));
							//no dead zone in half axis
							value = (1.0 + value) / 2.0;
							if (t.joystickAxisReverse)
							{
								value = 1.0 - value;
							}
							value = axisLinearity(value, t.joystickAxisLinearity);
						}
						else
						{
							//LOG("not half: "+TOSTRING(value)+" / "+TOSTRING(deadZone(value, t.joystickAxisDeadzone)) +" / "+TOSTRING(t.joystickAxisDeadzone) );
							if (t.joystickAxisReverse)
							{
								value = 1-value;
							}
								
							// no deadzone when using oure value
							value = deadZone(value, t.joystickAxisDeadzone);
							value = axisLinearity(value, t.joystickAxisLinearity);
						}
						// digital mapping of analog axis
						if (t.joystickAxisUseDigital)
						{
							if (value >= 0.5)
							{
								value = 1;
							}
							else
							{
								value = 0;
							}
						}
					}
				}
				break;
			case ET_JoystickSliderX:
			case ET_JoystickSliderY:
				{
					if (t.joystickNumber > m_ois_joysticks_free_slot || !m_ois_joysticks[t.joystickNumber])
					{
						value=0;
						continue;
					}
					if (t.eventtype == ET_JoystickSliderX)
					{
						value = (float)m_joystick_states[t.joystickNumber].mSliders[t.joystickSliderNumber].abX / (float)m_ois_joysticks[t.joystickNumber]->MAX_AXIS;
					}
					else if (t.eventtype == ET_JoystickSliderY)
					{
						value = (float)m_joystick_states[t.joystickNumber].mSliders[t.joystickSliderNumber].abY / (float)m_ois_joysticks[t.joystickNumber]->MAX_AXIS;
					}
					value = (value + 1)/2; // full axis
					if (t.joystickSliderReverse)
					{
						value = 1.0 - value; // reversed
					}
				}
				break;
		}

		if (is_steering_event)
		{
			if (t.GetEventId() == EV_TRUCK_STEER_LEFT)
			{
				m_event_value_steer_left_analog = value;
			}
			else // Steer right
			{
				m_event_value_steer_right_analog = value;
			}
		}
		else
		{
			m_event_values[t.GetEventId()] = value;
		}

	} // [FOREACH trigger IN m_event_triggers]
}

bool InputEngine::isKeyDown(OIS::KeyCode key)
{
	if (!m_ois_keyboard) return false;
	return this->m_ois_keyboard->isKeyDown(key);
}

bool InputEngine::isKeyDownValueBounce(OIS::KeyCode mod, float time)
{
	if(m_event_times[-mod] > 0)
		return false;
	else
	{
		bool res = isKeyDown(mod);
		if(res) m_event_times[-mod] = time;
		return res;
	}
}

/* // Used by saveMapping() which is commented out ATM.
String InputEngine::getEventTypeName(int type)
{
	switch(type)
	{
	case ET_NONE: return "None";
	case ET_Keyboard: return "Keyboard";
	case ET_MouseButton: return "MouseButton";
	case ET_MouseAxisX: return "MouseAxisX";
	case ET_MouseAxisY: return "MouseAxisY";
	case ET_MouseAxisZ: return "MouseAxisZ";
	case ET_JoystickButton: return "JoystickButton";
	case ET_JoystickAxisAbs: return "JoystickAxis";
	case ET_JoystickAxisRel: return "JoystickAxis";
	case ET_JoystickPov: return "JoystickPov";
	case ET_JoystickSliderX: return "JoystickSliderX";
	case ET_JoystickSliderY: return "JoystickSliderY";
	}
	return "unknown";
}
*/

bool InputEngine::processLine(char *line, int deviceID)
{
	// Check comment
	static String cur_comment = "";
	size_t linelen = strnlen(line, 1024);
	if (line[0]==';' || linelen < 5)
	{
		cur_comment += line;
		return false;
	}

	// Parse line
	char eventName[256]="";
	char evtype[256]="";
	sscanf(line, "%s %s", eventName, evtype);
	if (strnlen(eventName, 255) == 0 || strnlen(evtype, 255) == 0)
	{
		return false;
	}
	
	// Detect event type
	eventtypes eventtype = ET_NONE;
	if      (!strncmp(evtype, "Keyboard", 8))         { eventtype = ET_Keyboard;        }
	else if (!strncmp(evtype, "MouseButton", 10))     { eventtype = ET_MouseButton;     }
	else if (!strncmp(evtype, "MouseAxisX", 9))       { eventtype = ET_MouseAxisX;      }
	else if (!strncmp(evtype, "MouseAxisY", 9))       { eventtype = ET_MouseAxisY;      }
	else if (!strncmp(evtype, "MouseAxisZ", 9))       { eventtype = ET_MouseAxisZ;      }
	else if (!strncmp(evtype, "JoystickButton", 14))  { eventtype = ET_JoystickButton;  }
	else if (!strncmp(evtype, "JoystickAxis", 12))    { eventtype = ET_JoystickAxisAbs; }
	//else if (!strncmp(evtype, "JoystickAxis", 250)) { eventtype = ET_JoystickAxisRel; }
	else if (!strncmp(evtype, "JoystickPov", 11))     { eventtype = ET_JoystickPov;     }
	else if (!strncmp(evtype, "JoystickSlider", 14))  { eventtype = ET_JoystickSliderX; }
	else if (!strncmp(evtype, "JoystickSliderX", 15)) { eventtype = ET_JoystickSliderX; }
	else if (!strncmp(evtype, "JoystickSliderY", 15)) { eventtype = ET_JoystickSliderY; }
	else if (!strncmp(evtype, "None", 4))             { eventtype = ET_NONE;            }

	const char delimiters[] = "+";
	int joyNo = 0;
	float defaultDeadzone = 0.1f;
	float defaultLinearity = 1.0f;

	switch(eventtype)
	{
	case ET_Keyboard:
		{
			bool alt   = false;
			bool shift = false;
			bool ctrl  = false;
			bool expl  = false;

			char *keycode            = 0;
			char  keycodes[256]      = {};
			char  keycodes_work[256] = {};

			OIS::KeyCode key = KC_UNASSIGNED;

			sscanf(line, "%s %s %s", eventName, evtype, keycodes);
			// separate all keys and construct the key combination
			//LOG("try to add key: " + String(eventName)+","+ String(evtype)+","+String(keycodes));
			strncpy(keycodes_work, keycodes, 255);
			keycodes_work[255] = '\0';
			char *token = strtok(keycodes_work, delimiters);

			while (token != NULL)
			{
				if      (strncmp(token, "SHIFT", 5) == 0) { shift = true; }
				else if (strncmp(token, "CTRL",  4) == 0) { ctrl  = true; }
				else if (strncmp(token, "ALT",   3) == 0) { alt   = true; }
				else if (strncmp(token, "EXPL",  4) == 0) { expl  = true; }
				keycode = token;
				token = strtok(NULL, delimiters);
			}

			auto allit = m_all_keys.find(keycode);
			if (allit == m_all_keys.end())
			{
#ifndef NOOGRE
				LOG("unknown key: " + string(keycodes));
#endif
				key = KC_UNASSIGNED;
			} else
			{
				//LOG("found key: " + string(keycode) + " = " + TOSTRING((int)key));
				key = allit->second;
			}
			int eventID = resolveEventName(String(eventName));
			if (eventID == -1)
			{
				LOG("Error while processing input config: Unknown Event: "+String(eventName));
				return false;
			}
			EventTrigger t_key;
			t_key.eventtype = ET_Keyboard;
			t_key.shift     = shift;
			t_key.ctrl      = ctrl;
			t_key.alt       = alt;
			t_key.keyCode   = key;
			t_key.explicite = expl;
			
			strncpy(t_key.configline, keycodes, 128);
			strncpy(t_key.group, getEventGroup(eventName).c_str(), 128);
			strncpy(t_key.tmp_eventname, eventName, 128);

			strncpy(t_key.comments, cur_comment.c_str(), 1024);
			t_key.m_event_id = eventID;
			m_event_triggers.push_back(t_key);

			return true;
		}
	case ET_JoystickButton:
		{
			int buttonNo = 0;
			char tmp2[256] = {};
			sscanf(line, "%s %s %d %d %s", eventName, evtype, &joyNo, &buttonNo, tmp2);
			EventTrigger t_joy;
			//memset(&t_joy, 0, sizeof(event_trigger_t));
			int eventID = resolveEventName(String(eventName));
			if (eventID == -1) return false;
			t_joy.eventtype = ET_JoystickButton;
			t_joy.joystickNumber = (deviceID==-1?joyNo:deviceID);
			t_joy.joystickButtonNumber = buttonNo;
			if (!strcmp(tmp2, "!NEW!"))
			{
				strncpy(t_joy.configline, tmp2, 128);
			} else
			{
				char tmp[256] = {};
				sprintf(tmp, "%d", buttonNo);
				strncpy(t_joy.configline, tmp, 128);
			}
			strncpy(t_joy.group, getEventGroup(eventName).c_str(), 128);
			strncpy(t_joy.tmp_eventname, eventName, 128);
			strncpy(t_joy.comments, cur_comment.c_str(), 1024);
			cur_comment = "";
			t_joy.m_event_id = eventID;
			m_event_triggers.push_back(t_joy);
			return true;
		}
	case ET_JoystickAxisRel:
	case ET_JoystickAxisAbs:
		{
			int axisNo = 0;
			char options[256] = {};
			sscanf(line, "%s %s %d %d %s", eventName, evtype, &joyNo, &axisNo, options);
			int eventID = resolveEventName(String(eventName));
			if (eventID == -1) return false;

			bool half       = false;
			bool reverse    = false;
			bool linear     = false;
			bool relative   = false;
			bool usedigital = false;
			float deadzone  = defaultDeadzone;
			float linearity = defaultLinearity;
			int jAxisRegion = 0;
			//  0 = all
			// -1 = lower
			//  1 = upper
			char tmp[256] = {};
			strcpy(tmp, options);
			tmp[255] = '\0';
			char *token = strtok(tmp, delimiters);
			while (token != NULL)
			{
				if (strncmp(token, "HALF", 4) == 0)
					half=true;
				else if (strncmp(token, "REVERSE", 7) == 0)
					reverse=true;
				else if (strncmp(token, "LINEAR", 6) == 0)
					linear=true;
				else if (strncmp(token, "UPPER", 5) == 0)
					jAxisRegion = 1;
				else if (strncmp(token, "LOWER", 5) == 0)
					jAxisRegion = -1;
				else if (strncmp(token, "RELATIVE", 8) == 0)
					relative=true;
				else if (strncmp(token, "DIGITAL", 7) == 0)
					usedigital=true;
				else if (strncmp(token, "DEADZONE", 8) == 0 && strnlen(token, 250) > 9)
				{
					char tmp2[256] = {};
					strcpy(tmp2,token+9);
					deadzone = atof(tmp2);
					//LOG("got deadzone: " + TOSTRING(deadzone)+", "+String(tmp2));
				}
				else if (strncmp(token, "LINEARITY", 9) == 0 && strnlen(token, 250) > 10)
				{
					char tmp2[256] = {};
					strcpy(tmp2,token+10);
					linearity = atof(tmp2);
				}
				token = strtok(NULL, delimiters);
			}

			if (relative)
				eventtype = ET_JoystickAxisRel;

			EventTrigger t_joy;
			//memset(&t_joy, 0, sizeof(event_trigger_t));
			t_joy.eventtype = eventtype;
			t_joy.joystickAxisRegion = jAxisRegion;
			t_joy.joystickAxisUseDigital = usedigital;
			t_joy.joystickAxisDeadzone = deadzone;
			t_joy.joystickAxisHalf = half;
			t_joy.joystickAxisLinearity = linearity;
			t_joy.joystickAxisReverse = reverse;
			t_joy.joystickAxisNumber = axisNo;
			t_joy.joystickNumber = (deviceID==-1?joyNo:deviceID);
			strncpy(t_joy.configline, options, 128);
			strncpy(t_joy.group, getEventGroup(eventName).c_str(), 128);
			strncpy(t_joy.tmp_eventname, eventName, 128);
			strncpy(t_joy.comments, cur_comment.c_str(), 1024);
			cur_comment = "";
			t_joy.m_event_id = eventID;
			m_event_triggers.push_back(t_joy);
			return true;
		}
	case ET_NONE:
		{
			int eventID = resolveEventName(String(eventName));
			if (eventID == -1) return false;
			EventTrigger t_none;
			t_none.eventtype = eventtype;
			//t_none.configline = "";
			strncpy(t_none.group, getEventGroup(eventName).c_str(), 128);
			strncpy(t_none.tmp_eventname, eventName, 128);
			strncpy(t_none.comments, cur_comment.c_str(), 1024);
			cur_comment = "";
			t_none.m_event_id = eventID;
			m_event_triggers.push_back(t_none);
			return true;
		}
	case ET_MouseButton:
	case ET_MouseAxisX:
	case ET_MouseAxisY:
	case ET_MouseAxisZ:
		// no mouse support D:
		return false;
	case ET_JoystickPov:
		{
			int povNumber = 0;
			char dir[256] = {};
			sscanf(line, "%s %s %d %d %s", eventName, evtype, &joyNo, &povNumber, dir);
			int eventID = resolveEventName(String(eventName));
			if (eventID == -1) return false;

			int direction = OIS::Pov::Centered;
			if (!strcmp(dir, "North"))     direction = OIS::Pov::North;
			if (!strcmp(dir, "South"))     direction = OIS::Pov::South;
			if (!strcmp(dir, "East"))      direction = OIS::Pov::East;
			if (!strcmp(dir, "West"))      direction = OIS::Pov::West;
			if (!strcmp(dir, "NorthEast")) direction = OIS::Pov::NorthEast;
			if (!strcmp(dir, "SouthEast")) direction = OIS::Pov::SouthEast;
			if (!strcmp(dir, "NorthWest")) direction = OIS::Pov::NorthWest;
			if (!strcmp(dir, "SouthWest")) direction = OIS::Pov::SouthWest;

			EventTrigger t_pov;
			t_pov.eventtype = eventtype;
			t_pov.joystickNumber = (deviceID==-1?joyNo:deviceID);
			t_pov.joystickPovNumber = povNumber;
			t_pov.joystickPovDirection = direction;

			strncpy(t_pov.group, getEventGroup(eventName).c_str(), 128);
			strncpy(t_pov.tmp_eventname, eventName, 128);
			strncpy(t_pov.comments, cur_comment.c_str(), 1024);
			cur_comment = "";
			t_pov.m_event_id = eventID;
			m_event_triggers.push_back(t_pov);
			//LOG("added axis: " + TOSTRING(axisNo));
			return true;
		}
	case ET_JoystickSliderX:
	case ET_JoystickSliderY:
		{
			int sliderNumber = 0;
			char options[256] = {};
			char type;
			sscanf(line, "%s %s %d %c %d %s", eventName, evtype, &joyNo, &type, &sliderNumber, options);
			int eventID = resolveEventName(String(eventName));
			if (eventID == -1) return false;

			bool reverse=false;
			char tmp[256] = {};
			strncpy(tmp, options, 255);
			tmp[255] = '\0';
			char *token = strtok(tmp, delimiters);
			while (token != NULL)
			{
				if (strncmp(token, "REVERSE", 7) == 0)
					reverse=true;

				token = strtok(NULL, delimiters);
			}

			EventTrigger t_slider;

			if (type == 'Y' || type == 'y')
				eventtype = ET_JoystickSliderY;
			else if (type == 'X' || type == 'x')
				eventtype = ET_JoystickSliderX;

			t_slider.eventtype = eventtype;
			t_slider.joystickNumber = (deviceID==-1?joyNo:deviceID);
			t_slider.joystickSliderNumber = sliderNumber;
			t_slider.joystickSliderReverse = reverse;
			// TODO: add region support to sliders!
			t_slider.joystickSliderRegion = 0;
			strncpy(t_slider.configline, options, 128);
			strncpy(t_slider.group, getEventGroup(eventName).c_str(), 128);
			strncpy(t_slider.tmp_eventname, eventName, 128);
			strncpy(t_slider.comments, cur_comment.c_str(), 1024);
			cur_comment = "";
			t_slider.m_event_id = eventID;
			m_event_triggers.push_back(t_slider);

			return true;
		}
	default:
		return false;
	}
}

int InputEngine::getJoyComponentCount(OIS::ComponentType type, int joystickNumber)
{
	if (joystickNumber > m_ois_joysticks_free_slot || !m_ois_joysticks[joystickNumber]) return 0;
	return m_ois_joysticks[joystickNumber]->getNumberOfComponents(type);
}

std::string InputEngine::getJoyVendor(int joystickNumber)
{
	if (joystickNumber > m_ois_joysticks_free_slot || !m_ois_joysticks[joystickNumber]) return "unknown";
	return m_ois_joysticks[joystickNumber]->vendor();
}

JoyStickState *InputEngine::getCurrentJoyState(int joystickNumber)
{
	if (joystickNumber > m_ois_joysticks_free_slot) return 0;
	return &m_joystick_states[joystickNumber];
}

String InputEngine::getEventGroup(String eventName)
{
	const char delimiters[] = "_";
	char tmp[256] = {};
	strncpy(tmp, eventName.c_str(), 255);
	tmp[255] = '\0';
	char *token = strtok(tmp, delimiters);
	while (token != NULL)
	{
		return String(token);
	}
	return "";
}

bool InputEngine::reloadConfig(std::string outfile)
{
	m_event_triggers.clear();
	loadMapping(outfile);
	return true;
}

#if 0 // Unused, exclude from compiling
bool InputEngine::saveMapping(String outfile, String hwnd, int joyNum)
{
	// -10 = all
	// -2  = keyboard
	// -3  = mouse
	// >0 joystick
	FILE *f = fopen(const_cast<char *>(outfile.c_str()),"w");
	if (!f)
		return false;

	// Removed long commented out code

	int counter = 0;
	char curGroup[128] = "";
	std::map<int, std::vector<event_trigger_t> > controls = getEvents();
	std::map<int, std::vector<event_trigger_t> >::iterator mapIt;
	std::vector<event_trigger_t>::iterator vecIt;
	for (mapIt = controls.begin(); mapIt != controls.end(); mapIt++)
	{
		std::vector<event_trigger_t> vec = mapIt->second;

		for (vecIt = vec.begin(); vecIt != vec.end(); vecIt++, counter++)
		{
			// filters
			if (vecIt->eventtype == ET_Keyboard && joyNum != -10 && joyNum != -2) continue;
			if ((vecIt->eventtype == ET_MouseAxisX || vecIt->eventtype == ET_MouseAxisY || vecIt->eventtype == ET_MouseAxisZ) && joyNum != -10 && joyNum != -3) continue;
			if ((vecIt->eventtype == ET_JoystickAxisAbs || vecIt->eventtype == ET_JoystickAxisRel || vecIt->eventtype == ET_JoystickButton || vecIt->eventtype == ET_JoystickPov || vecIt->eventtype == ET_JoystickSliderX || vecIt->eventtype == ET_JoystickSliderY) && joyNum>=0 && vecIt->joystickNumber != joyNum) continue;

			if (strcmp(vecIt->group, curGroup))
			{
				strncpy(curGroup, vecIt->group, 128);
				// group title:
				fprintf(f, "\n; %s\n", curGroup);
			}

			// no user comments for now!
			//if (vecIt->comments!="")
			//	fprintf(f, "%s", vecIt->comments.c_str());

			// print event name
			fprintf(f, "%-30s ", eventIDToName(mapIt->first).c_str());
			// print event type
			fprintf(f, "%-20s ", getEventTypeName(vecIt->eventtype).c_str());

			if (vecIt->eventtype == ET_Keyboard)
			{
				fprintf(f, "%s ", vecIt->configline);
			} else if (vecIt->eventtype == ET_JoystickAxisAbs || vecIt->eventtype == ET_JoystickAxisRel)
			{
				fprintf(f, "%d ", vecIt->joystickNumber);
				fprintf(f, "%d ", vecIt->joystickAxisNumber);
				fprintf(f, "%s ", vecIt->configline);
			} else if (vecIt->eventtype == ET_JoystickSliderX || vecIt->eventtype == ET_JoystickSliderY)
			{
				fprintf(f, "%d ", vecIt->joystickNumber);
				char type = 'X';
				if (vecIt->eventtype == ET_JoystickSliderY)
					type = 'Y';
				fprintf(f, "%c ", type);
				fprintf(f, "%d ", vecIt->joystickSliderNumber);
				fprintf(f, "%s ", vecIt->configline);
			} else if (vecIt->eventtype == ET_JoystickButton)
			{
				fprintf(f, "%d ", vecIt->joystickNumber);
				fprintf(f, "%d ", vecIt->joystickButtonNumber);
			} else if (vecIt->eventtype == ET_JoystickPov)
			{
				const char *dirStr = "North";
				if (vecIt->joystickPovDirection == OIS::Pov::North)     dirStr = "North";
				if (vecIt->joystickPovDirection == OIS::Pov::South)     dirStr = "South";
				if (vecIt->joystickPovDirection == OIS::Pov::East)      dirStr = "East";
				if (vecIt->joystickPovDirection == OIS::Pov::West)      dirStr = "West";
				if (vecIt->joystickPovDirection == OIS::Pov::NorthEast) dirStr = "NorthEast";
				if (vecIt->joystickPovDirection == OIS::Pov::SouthEast) dirStr = "SouthEast";
				if (vecIt->joystickPovDirection == OIS::Pov::NorthWest) dirStr = "NorthWest";
				if (vecIt->joystickPovDirection == OIS::Pov::SouthWest) dirStr = "SouthWest";

				fprintf(f, "%d %d %s", vecIt->joystickNumber, vecIt->joystickPovNumber, dirStr);
			}
			// end this line
			fprintf(f, "\n");
		}
	}
	fclose(f);
	return true;
}
#endif // #if 0 to disable compiling

void InputEngine::completeMissingEvents()
{
	if (!m_mapping_loaded) return;
	
	for (int i = 0; i < EV_MODE_LAST; i++)
	{
		if ((strlen(eventInfo[i].defaultKey) == 0) || (strcmp(eventInfo[i].defaultKey, "None") == 0))
		{
			continue; // No default key mapping, skip
		}
		int event_id = eventInfo[i].eventID;
		EventTrigger* trigger = GetFirstTriggerForEvent(event_id);
		if (trigger != nullptr)
		{
			continue; // Already loaded, skip
		}

		// Not loaded and default exists, insert default
		char tmp[256] = "";
		sprintf(tmp, "%s %s", eventInfo[i].name, eventInfo[i].defaultKey);
		processLine(tmp);
	}
}


bool InputEngine::loadMapping(String outfile, bool append, int deviceID)
{
	char line[1025] = "";

	if (!append)
	{
		// clear everything
		resetKeys();
		m_event_triggers.clear();
	}

	LOG(" * Loading input mapping " + outfile);
	{
		DataStreamPtr ds;
		try
		{
			ds = ResourceGroupManager::getSingleton().openResource(outfile, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		} catch(...)
		{
			return false;
		}
		while (!ds->eof())
		{
			size_t size = 1024;
			if (ds->tell() + size >= ds->size())
				size = ds->size()-ds->tell();
			if (ds->tell() >= ds->size())
				break;
			size_t readnum = ds->readLine(line, size);
			if (readnum > 5)
				processLine(line, deviceID);
		}
	}
	LOG(" * Input map successfully loaded");
	return true;
}

int InputEngine::resolveEventName(String eventName)
{
	int i=0;
	while(i!=EV_MODE_LAST)
	{
		if (eventInfo[i].name == eventName)
			return eventInfo[i].eventID;
		i++;
	}
#ifndef NOOGRE
	LOG("unknown event (ignored): " + eventName);
#endif
	return -1;
}

String InputEngine::eventIDToName(int eventID)
{
	int i=0;
	while(i!=EV_MODE_LAST)
	{
		if (eventInfo[i].eventID == eventID)
			return eventInfo[i].name;
		i++;
	}
	return "unknown";
}

void InputEngine::initAllKeys()
{
	m_all_keys["0"] = KC_0;
	m_all_keys["1"] = KC_1;
	m_all_keys["2"] = KC_2;
	m_all_keys["3"] = KC_3;
	m_all_keys["4"] = KC_4;
	m_all_keys["5"] = KC_5;
	m_all_keys["6"] = KC_6;
	m_all_keys["7"] = KC_7;
	m_all_keys["8"] = KC_8;
	m_all_keys["9"] = KC_9;
	m_all_keys["A"] = KC_A ;
	m_all_keys["ABNT_C1"] = KC_ABNT_C1;
	m_all_keys["ABNT_C2"] = KC_ABNT_C2;
	m_all_keys["ADD"] = KC_ADD;
	m_all_keys["APOSTROPHE"] = KC_APOSTROPHE;
	m_all_keys["APPS"] = KC_APPS;
	m_all_keys["AT"] = KC_AT;
	m_all_keys["AX"] = KC_AX;
	m_all_keys["B"] = KC_B;
	m_all_keys["BACK"] = KC_BACK;
	m_all_keys["BACKSLASH"] = KC_BACKSLASH;
	m_all_keys["C"] = KC_C;
	m_all_keys["CALCULATOR"] = KC_CALCULATOR;
	m_all_keys["CAPITAL"] = KC_CAPITAL;
	m_all_keys["COLON"] = KC_COLON;
	m_all_keys["COMMA"] = KC_COMMA;
	m_all_keys["CONVERT"] = KC_CONVERT;
	m_all_keys["D"] = KC_D;
	m_all_keys["DECIMAL"] = KC_DECIMAL;
	m_all_keys["DELETE"] = KC_DELETE;
	m_all_keys["DIVIDE"] = KC_DIVIDE;
	m_all_keys["DOWN"] = KC_DOWN;
	m_all_keys["E"] = KC_E;
	m_all_keys["END"] = KC_END;
	m_all_keys["EQUALS"] = KC_EQUALS;
	m_all_keys["ESCAPE"] = KC_ESCAPE;
	m_all_keys["F"] = KC_F;
	m_all_keys["F1"] = KC_F1;
	m_all_keys["F10"] = KC_F10;
	m_all_keys["F11"] = KC_F11;
	m_all_keys["F12"] = KC_F12;
	m_all_keys["F13"] = KC_F13;
	m_all_keys["F14"] = KC_F14;
	m_all_keys["F15"] = KC_F15;
	m_all_keys["F2"] = KC_F2;
	m_all_keys["F3"] = KC_F3;
	m_all_keys["F4"] = KC_F4;
	m_all_keys["F5"] = KC_F5;
	m_all_keys["F6"] = KC_F6;
	m_all_keys["F7"] = KC_F7;
	m_all_keys["F8"] = KC_F8;
	m_all_keys["F9"] = KC_F9;
	m_all_keys["G"] = KC_G;
	m_all_keys["GRAVE"] = KC_GRAVE;
	m_all_keys["H"] = KC_H;
	m_all_keys["HOME"] = KC_HOME;
	m_all_keys["I"] = KC_I;
	m_all_keys["INSERT"] = KC_INSERT;
	m_all_keys["J"] = KC_J;
	m_all_keys["K"] = KC_K;
	m_all_keys["KANA"] = KC_KANA;
	m_all_keys["KANJI"] = KC_KANJI;
	m_all_keys["L"] = KC_L;
	m_all_keys["LBRACKET"] = KC_LBRACKET;
	m_all_keys["LCONTROL"] = KC_LCONTROL;
	m_all_keys["LEFT"] = KC_LEFT;
	m_all_keys["LMENU"] = KC_LMENU;
	m_all_keys["LSHIFT"] = KC_LSHIFT;
	m_all_keys["LWIN"] = KC_LWIN;
	m_all_keys["M"] = KC_M;
	m_all_keys["MAIL"] = KC_MAIL;
	m_all_keys["MEDIASELECT"] = KC_MEDIASELECT;
	m_all_keys["MEDIASTOP"] = KC_MEDIASTOP;
	m_all_keys["MINUS"] = KC_MINUS;
	m_all_keys["MULTIPLY"] = KC_MULTIPLY;
	m_all_keys["MUTE"] = KC_MUTE;
	m_all_keys["MYCOMPUTER"] = KC_MYCOMPUTER;
	m_all_keys["N"] = KC_N;
	m_all_keys["NEXTTRACK"] = KC_NEXTTRACK;
	m_all_keys["NOCONVERT"] = KC_NOCONVERT;
	m_all_keys["NUMLOCK"] = KC_NUMLOCK;
	m_all_keys["NUMPAD0"] = KC_NUMPAD0;
	m_all_keys["NUMPAD1"] = KC_NUMPAD1;
	m_all_keys["NUMPAD2"] = KC_NUMPAD2;
	m_all_keys["NUMPAD3"] = KC_NUMPAD3;
	m_all_keys["NUMPAD4"] = KC_NUMPAD4;
	m_all_keys["NUMPAD5"] = KC_NUMPAD5;
	m_all_keys["NUMPAD6"] = KC_NUMPAD6;
	m_all_keys["NUMPAD7"] = KC_NUMPAD7;
	m_all_keys["NUMPAD8"] = KC_NUMPAD8;
	m_all_keys["NUMPAD9"] = KC_NUMPAD9;
	m_all_keys["NUMPADCOMMA"] = KC_NUMPADCOMMA;
	m_all_keys["NUMPADENTER"] = KC_NUMPADENTER;
	m_all_keys["NUMPADEQUALS"] = KC_NUMPADEQUALS;
	m_all_keys["O"] = KC_O;
	m_all_keys["OEM_102"] = KC_OEM_102;
	m_all_keys["P"] = KC_P;
	m_all_keys["PAUSE"] = KC_PAUSE;
	m_all_keys["PERIOD"] = KC_PERIOD;
	m_all_keys["PGDOWN"] = KC_PGDOWN;
	m_all_keys["PGUP"] = KC_PGUP;
	m_all_keys["PLAYPAUSE"] = KC_PLAYPAUSE;
	m_all_keys["POWER"] = KC_POWER;
	m_all_keys["PREVTRACK"] = KC_PREVTRACK;
	m_all_keys["Q"] = KC_Q;
	m_all_keys["R"] = KC_R;
	m_all_keys["RBRACKET"] = KC_RBRACKET;
	m_all_keys["RCONTROL"] = KC_RCONTROL;
	m_all_keys["RETURN"] = KC_RETURN;
	m_all_keys["RIGHT"] = KC_RIGHT;
	m_all_keys["RMENU"] = KC_RMENU;
	m_all_keys["RSHIFT"] = KC_RSHIFT;
	m_all_keys["RWIN"] = KC_RWIN;
	m_all_keys["S"] = KC_S;
	m_all_keys["SCROLL"] = KC_SCROLL;
	m_all_keys["SEMICOLON"] = KC_SEMICOLON;
	m_all_keys["SLASH"] = KC_SLASH;
	m_all_keys["SLEEP"] = KC_SLEEP;
	m_all_keys["SPACE"] = KC_SPACE;
	m_all_keys["STOP"] = KC_STOP;
	m_all_keys["SUBTRACT"] = KC_SUBTRACT;
	m_all_keys["SYSRQ"] = KC_SYSRQ;
	m_all_keys["T"] = KC_T;
	m_all_keys["TAB"] = KC_TAB;
	m_all_keys["U"] = KC_U;
	//m_all_keys["UNASSIGNED"] = KC_UNASSIGNED;
	m_all_keys["UNDERLINE"] = KC_UNDERLINE;
	m_all_keys["UNLABELED"] = KC_UNLABELED;
	m_all_keys["UP"] = KC_UP;
	m_all_keys["V"] = KC_V;
	m_all_keys["VOLUMEDOWN"] = KC_VOLUMEDOWN;
	m_all_keys["VOLUMEUP"] = KC_VOLUMEUP;
	m_all_keys["W"] = KC_W;
	m_all_keys["WAKE"] = KC_WAKE;
	m_all_keys["WEBBACK"] = KC_WEBBACK;
	m_all_keys["WEBFAVORITES"] = KC_WEBFAVORITES;
	m_all_keys["WEBFORWARD"] = KC_WEBFORWARD;
	m_all_keys["WEBHOME"] = KC_WEBHOME;
	m_all_keys["WEBREFRESH"] = KC_WEBREFRESH;
	m_all_keys["WEBSEARCH"] = KC_WEBSEARCH;
	m_all_keys["WEBSTOP"] = KC_WEBSTOP;
	m_all_keys["X"] = KC_X;
	m_all_keys["Y"] = KC_Y;
	m_all_keys["YEN"] = KC_YEN;
	m_all_keys["Z"] = KC_Z;
}

void InputEngine::setupDefault(Ogre::String inputhwnd /* = "" */)
{
	// setup input
	int inputGrabMode=GRAB_ALL;
	String inputGrabSetting = SSETTING("Input Grab", "All");

	if     (inputGrabSetting == "All")
		inputGrabMode = GRAB_ALL;
	else if (inputGrabSetting == "Dynamically")
		inputGrabMode = GRAB_DYNAMICALLY;
	else if (inputGrabSetting == "None")
		inputGrabMode = GRAB_NONE;

	// start input engine
	size_t hWnd = 0;
	RoR::Application::GetOgreSubsystem()->GetRenderWindow()->getCustomAttribute("WINDOW", &hWnd);

	this->setup(TOSTRING(hWnd), true, true, inputGrabMode);

}

EventTrigger* InputEngine::GetFirstTriggerForEvent(int event_id)
{
	auto itor_end = m_event_triggers.end();
	for (auto itor = m_event_triggers.begin(); itor != itor_end; ++itor)
	{
		if (itor->GetEventId() == event_id)
		{
			return &(*itor);
		}
	}
	return nullptr;
}

String InputEngine::getKeyForCommand( int event_id )
{
	EventTrigger* trigger = GetFirstTriggerForEvent(event_id);
	if (trigger != nullptr)
	{
		return getKeyNameForKeyCode(static_cast<OIS::KeyCode>(trigger->keyCode));
	}
	return String();
}
