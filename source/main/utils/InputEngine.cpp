/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013-2017 Petr Ohlidal & contributors

    For more information, see http://www.rigsofrods.org/

    Rigs of Rods is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3, as
    published by the Free Software Foundation.

    Rigs of Rods is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

#include "InputEngine.h"

//namespace RoR { // TODO: This would require change in many files, let's postpone it ~ only_a_ptr, 06/2017

GameInputEvent    EV_AIRPLANE_AIRBRAKES_FULL       ("AIRPLANE_AIRBRAKES_FULL",      "Keyboard CTRL+4");
GameInputEvent    EV_AIRPLANE_AIRBRAKES_LESS       ("AIRPLANE_AIRBRAKES_LESS",      "Keyboard EXPL+3");
GameInputEvent    EV_AIRPLANE_AIRBRAKES_MORE       ("AIRPLANE_AIRBRAKES_MORE",      "Keyboard EXPL+4");
GameInputEvent    EV_AIRPLANE_AIRBRAKES_NONE       ("AIRPLANE_AIRBRAKES_NONE",      "Keyboard CTRL+3");
GameInputEvent    EV_AIRPLANE_BRAKE                ("AIRPLANE_BRAKE",               "Keyboard B");             ///< normal brake for an aircraft.
GameInputEvent    EV_AIRPLANE_ELEVATOR_DOWN        ("AIRPLANE_ELEVATOR_DOWN",       "Keyboard DOWN");          ///< pull the elevator down in an aircraft.
GameInputEvent    EV_AIRPLANE_ELEVATOR_UP          ("AIRPLANE_ELEVATOR_UP",         "Keyboard UP");            ///< pull the elevator up in an aircraft.
GameInputEvent    EV_AIRPLANE_FLAPS_FULL           ("AIRPLANE_FLAPS_FULL",          "Keyboard CTRL+2");        ///< full flaps in an aircraft.
GameInputEvent    EV_AIRPLANE_FLAPS_LESS           ("AIRPLANE_FLAPS_LESS",          "Keyboard EXPL+1");        ///< one step less flaps.
GameInputEvent    EV_AIRPLANE_FLAPS_MORE           ("AIRPLANE_FLAPS_MORE",          "Keyboard EXPL+2");        ///< one step more flaps.
GameInputEvent    EV_AIRPLANE_FLAPS_NONE           ("AIRPLANE_FLAPS_NONE",          "Keyboard CTRL+1");        ///< no flaps.
GameInputEvent    EV_AIRPLANE_PARKING_BRAKE        ("AIRPLANE_PARKING_BRAKE",       "Keyboard P");             ///< airplane parking brake.
GameInputEvent    EV_AIRPLANE_REVERSE              ("AIRPLANE_REVERSE",             "Keyboard R");             ///< reverse the turboprops
GameInputEvent    EV_AIRPLANE_RUDDER_LEFT          ("AIRPLANE_RUDDER_LEFT",         "Keyboard Z");             ///< rudder left
GameInputEvent    EV_AIRPLANE_RUDDER_RIGHT         ("AIRPLANE_RUDDER_RIGHT",        "Keyboard X");             ///< rudder right
GameInputEvent    EV_AIRPLANE_STEER_LEFT           ("AIRPLANE_STEER_LEFT",          "Keyboard LEFT");          ///< steer left
GameInputEvent    EV_AIRPLANE_STEER_RIGHT          ("AIRPLANE_STEER_RIGHT",         "Keyboard RIGHT");         ///< steer right
GameInputEvent    EV_AIRPLANE_THROTTLE             ("AIRPLANE_THROTTLE",            "");
GameInputEvent    EV_AIRPLANE_THROTTLE_AXIS        ("AIRPLANE_THROTTLE_AXIS",       "None");                   ///< throttle axis. Only use this if you have fitting hardware :) (i.e. a Slider)
GameInputEvent    EV_AIRPLANE_THROTTLE_DOWN        ("AIRPLANE_THROTTLE_DOWN",       "Keyboard EXPL+PGDOWN");   ///< decreases the airplane thrust
GameInputEvent    EV_AIRPLANE_THROTTLE_FULL        ("AIRPLANE_THROTTLE_FULL",       "Keyboard CTRL+PGUP");     ///< full thrust
GameInputEvent    EV_AIRPLANE_THROTTLE_NO          ("AIRPLANE_THROTTLE_NO",         "Keyboard CTRL+PGDOWN");   ///< no thrust
GameInputEvent    EV_AIRPLANE_THROTTLE_UP          ("AIRPLANE_THROTTLE_UP",         "Keyboard EXPL+PGUP");     ///< increase the airplane thrust
GameInputEvent    EV_AIRPLANE_TOGGLE_ENGINES       ("AIRPLANE_TOGGLE_ENGINES",      "Keyboard CTRL+HOME");     ///< switch all engines on / off

GameInputEvent    EV_BOAT_CENTER_RUDDER            ("BOAT_CENTER_RUDDER",           "KeyboardPGDOWN");         //!< center the rudder
GameInputEvent    EV_BOAT_REVERSE                  ("BOAT_REVERSE",                 "KeyboardPGUP");           //!< no thrust
GameInputEvent    EV_BOAT_STEER_LEFT               ("BOAT_STEER_LEFT",              "KeyboardLEFT");           //!< steer left a step
GameInputEvent    EV_BOAT_STEER_LEFT_AXIS          ("BOAT_STEER_LEFT_AXIS",         "None");                   //!< steer left (analog value!)
GameInputEvent    EV_BOAT_STEER_RIGHT              ("BOAT_STEER_RIGHT",             "KeyboardRIGHT");          //!< steer right a step
GameInputEvent    EV_BOAT_STEER_RIGHT_AXIS         ("BOAT_STEER_RIGHT_AXIS",        "None");                   //!< steer right (analog value!)
GameInputEvent    EV_BOAT_THROTTLE_AXIS            ("BOAT_THROTTLE_AXIS",           "None");                   //!< throttle axis. Only use this if you have fitting hardware :) (i.e. a Slider)
GameInputEvent    EV_BOAT_THROTTLE_DOWN            ("BOAT_THROTTLE_DOWN",           "KeyboardDOWN");           //!< decrease throttle
GameInputEvent    EV_BOAT_THROTTLE_UP              ("BOAT_THROTTLE_UP",             "KeyboardUP");             //!< increase throttle

GameInputEvent    EV_SKY_DECREASE_TIME             ("SKY_DECREASE_TIME",            "Keyboard EXPL+SUBTRACT"); //!< decrease day-time
GameInputEvent    EV_SKY_DECREASE_TIME_FAST        ("SKY_DECREASE_TIME_FAST",       "Keyboard SHIFT+SUBTRACT");//!< decrease day-time a lot faster
GameInputEvent    EV_SKY_INCREASE_TIME             ("SKY_INCREASE_TIME",            "Keyboard EXPL+ADD");      //!< increase day-time
GameInputEvent    EV_SKY_INCREASE_TIME_FAST        ("SKY_INCREASE_TIME_FAST",       "Keyboard SHIFT+ADD");     //!< increase day-time a lot faster

GameInputEvent    EV_CAMERA_CHANGE                 ("CAMERA_CHANGE",                "Keyboard EXPL+C");        //!< change camera mode
GameInputEvent    EV_CAMERA_DOWN                   ("CAMERA_DOWN",                  "Keyboard Z");
GameInputEvent    EV_CAMERA_FREE_MODE              ("CAMERA_FREE_MODE",             "Keyboard EXPL+SHIFT+C");
GameInputEvent    EV_CAMERA_FREE_MODE_FIX          ("CAMERA_FREE_MODE_FIX",         "Keyboard EXPL+ALT+C");
GameInputEvent    EV_CAMERA_LOOKBACK               ("CAMERA_LOOKBACK",              "Keyboard NUMPAD1");       //!< look back (toggles between normal and lookback)
GameInputEvent    EV_CAMERA_RESET                  ("CAMERA_RESET",                 "Keyboard NUMPAD5");       //!< reset the camera position
GameInputEvent    EV_CAMERA_ROTATE_DOWN            ("CAMERA_ROTATE_DOWN",           "Keyboard NUMPAD2");       //!< rotate camera down
GameInputEvent    EV_CAMERA_ROTATE_LEFT            ("CAMERA_ROTATE_LEFT",           "Keyboard NUMPAD4");       //!< rotate camera left
GameInputEvent    EV_CAMERA_ROTATE_RIGHT           ("CAMERA_ROTATE_RIGHT",          "Keyboard NUMPAD6");       //!< rotate camera right
GameInputEvent    EV_CAMERA_ROTATE_UP              ("CAMERA_ROTATE_UP",             "Keyboard NUMPAD8");       //!< rotate camera up
GameInputEvent    EV_CAMERA_SWIVEL_DOWN            ("CAMERA_SWIVEL_DOWN",           "Keyboard CTRL+NUMPAD2");  //!< swivel camera down
GameInputEvent    EV_CAMERA_SWIVEL_LEFT            ("CAMERA_SWIVEL_LEFT",           "Keyboard CTRL+NUMPAD4");  //!< swivel camera left
GameInputEvent    EV_CAMERA_SWIVEL_RIGHT           ("CAMERA_SWIVEL_RIGHT",          "Keyboard CTRL+NUMPAD6");  //!< swivel camera right
GameInputEvent    EV_CAMERA_SWIVEL_UP              ("CAMERA_SWIVEL_UP",             "Keyboard CTRL+NUMPAD8");  //!< swivel camera up
GameInputEvent    EV_CAMERA_UP                     ("CAMERA_UP",                    "Keyboard Q");
GameInputEvent    EV_CAMERA_ZOOM_IN                ("CAMERA_ZOOM_IN",               "Keyboard EXPL+NUMPAD9");  //!< zoom camera in
GameInputEvent    EV_CAMERA_ZOOM_IN_FAST           ("CAMERA_ZOOM_IN_FAST",          "Keyboard SHIFT+NUMPAD9"); //!< zoom camera in faster
GameInputEvent    EV_CAMERA_ZOOM_OUT               ("CAMERA_ZOOM_OUT",              "Keyboard EXPL+NUMPAD3");  //!< zoom camera out
GameInputEvent    EV_CAMERA_ZOOM_OUT_FAST          ("CAMERA_ZOOM_OUT_FAST",         "Keyboard SHIFT+NUMPAD3"); //!< zoom camera out faster

GameInputEvent    EV_CHARACTER_BACKWARDS           ("CHARACTER_BACKWARDS",          "Keyboard S");             //!< step backwards with the character
GameInputEvent    EV_CHARACTER_FORWARD             ("CHARACTER_FORWARD",            "Keyboard W");             //!< step forward with the character
GameInputEvent    EV_CHARACTER_JUMP                ("CHARACTER_JUMP",               "Keyboard SPACE");         //!< let the character jump
GameInputEvent    EV_CHARACTER_LEFT                ("CHARACTER_LEFT",               "Keyboard LEFT");          //!< rotate character left
GameInputEvent    EV_CHARACTER_RIGHT               ("CHARACTER_RIGHT",              "Keyboard RIGHT");         //!< rotate character right
GameInputEvent    EV_CHARACTER_ROT_DOWN            ("CHARACTER_ROT_DOWN",           "Keyboard DOWN");
GameInputEvent    EV_CHARACTER_ROT_UP              ("CHARACTER_ROT_UP",             "Keyboard UP");
GameInputEvent    EV_CHARACTER_RUN                 ("CHARACTER_RUN",                "Keyboard SHIFT+W");       //!< let the character run
GameInputEvent    EV_CHARACTER_SIDESTEP_LEFT       ("CHARACTER_SIDESTEP_LEFT",      "Keyboard A");             //!< sidestep to the left
GameInputEvent    EV_CHARACTER_SIDESTEP_RIGHT      ("CHARACTER_SIDESTEP_RIGHT",     "Keyboard D");             //!< sidestep to the right

GameInputEvent    EV_COMMANDS_01                   ("COMMANDS_01",                  "Keyboard EXPL+F1");
GameInputEvent    EV_COMMANDS_02                   ("COMMANDS_02",                  "Keyboard EXPL+F2");
GameInputEvent    EV_COMMANDS_03                   ("COMMANDS_03",                  "Keyboard EXPL+F3");
GameInputEvent    EV_COMMANDS_04                   ("COMMANDS_04",                  "Keyboard EXPL+F4");
GameInputEvent    EV_COMMANDS_05                   ("COMMANDS_05",                  "Keyboard EXPL+F5");
GameInputEvent    EV_COMMANDS_06                   ("COMMANDS_06",                  "Keyboard EXPL+F6");
GameInputEvent    EV_COMMANDS_07                   ("COMMANDS_07",                  "Keyboard EXPL+F7");
GameInputEvent    EV_COMMANDS_08                   ("COMMANDS_08",                  "Keyboard EXPL+F8");
GameInputEvent    EV_COMMANDS_09                   ("COMMANDS_09",                  "Keyboard EXPL+F9");
GameInputEvent    EV_COMMANDS_10                   ("COMMANDS_10",                  "Keyboard EXPL+F10");
GameInputEvent    EV_COMMANDS_11                   ("COMMANDS_11",                  "Keyboard EXPL+F11");
GameInputEvent    EV_COMMANDS_12                   ("COMMANDS_12",                  "Keyboard EXPL+F12");
GameInputEvent    EV_COMMANDS_13                   ("COMMANDS_13",                  "Keyboard EXPL+CTRL+F1");
GameInputEvent    EV_COMMANDS_14                   ("COMMANDS_14",                  "Keyboard EXPL+CTRL+F2");
GameInputEvent    EV_COMMANDS_15                   ("COMMANDS_15",                  "Keyboard EXPL+CTRL+F3");
GameInputEvent    EV_COMMANDS_16                   ("COMMANDS_16",                  "Keyboard EXPL+CTRL+F4");
GameInputEvent    EV_COMMANDS_17                   ("COMMANDS_17",                  "Keyboard EXPL+CTRL+F5");
GameInputEvent    EV_COMMANDS_18                   ("COMMANDS_18",                  "Keyboard EXPL+CTRL+F6");
GameInputEvent    EV_COMMANDS_19                   ("COMMANDS_19",                  "Keyboard EXPL+CTRL+F7");
GameInputEvent    EV_COMMANDS_20                   ("COMMANDS_20",                  "Keyboard EXPL+CTRL+F8");
GameInputEvent    EV_COMMANDS_21                   ("COMMANDS_21",                  "Keyboard EXPL+CTRL+F9");
GameInputEvent    EV_COMMANDS_22                   ("COMMANDS_22",                  "Keyboard EXPL+CTRL+F10");
GameInputEvent    EV_COMMANDS_23                   ("COMMANDS_23",                  "Keyboard EXPL+CTRL+F11");
GameInputEvent    EV_COMMANDS_24                   ("COMMANDS_24",                  "Keyboard EXPL+CTRL+F12");
GameInputEvent    EV_COMMANDS_25                   ("COMMANDS_25",                  "Keyboard EXPL+SHIFT+F1");
GameInputEvent    EV_COMMANDS_26                   ("COMMANDS_26",                  "Keyboard EXPL+SHIFT+F2");
GameInputEvent    EV_COMMANDS_27                   ("COMMANDS_27",                  "Keyboard EXPL+SHIFT+F3");
GameInputEvent    EV_COMMANDS_28                   ("COMMANDS_28",                  "Keyboard EXPL+SHIFT+F4");
GameInputEvent    EV_COMMANDS_29                   ("COMMANDS_29",                  "Keyboard EXPL+SHIFT+F5");
GameInputEvent    EV_COMMANDS_30                   ("COMMANDS_30",                  "Keyboard EXPL+SHIFT+F6");
GameInputEvent    EV_COMMANDS_31                   ("COMMANDS_31",                  "Keyboard EXPL+SHIFT+F7");
GameInputEvent    EV_COMMANDS_32                   ("COMMANDS_32",                  "Keyboard EXPL+SHIFT+F8");
GameInputEvent    EV_COMMANDS_33                   ("COMMANDS_33",                  "Keyboard EXPL+SHIFT+F9");
GameInputEvent    EV_COMMANDS_34                   ("COMMANDS_34",                  "Keyboard EXPL+SHIFT+F10");
GameInputEvent    EV_COMMANDS_35                   ("COMMANDS_35",                  "Keyboard EXPL+SHIFT+F11");
GameInputEvent    EV_COMMANDS_36                   ("COMMANDS_36",                  "Keyboard EXPL+SHIFT+F12");
GameInputEvent    EV_COMMANDS_37                   ("COMMANDS_37",                  "Keyboard EXPL+ALT+F1");
GameInputEvent    EV_COMMANDS_38                   ("COMMANDS_38",                  "Keyboard EXPL+ALT+F2");
GameInputEvent    EV_COMMANDS_39                   ("COMMANDS_39",                  "Keyboard EXPL+ALT+F3");
GameInputEvent    EV_COMMANDS_40                   ("COMMANDS_40",                  "Keyboard EXPL+ALT+F4");
GameInputEvent    EV_COMMANDS_41                   ("COMMANDS_41",                  "Keyboard EXPL+ALT+F5");
GameInputEvent    EV_COMMANDS_42                   ("COMMANDS_42",                  "Keyboard EXPL+ALT+F6");
GameInputEvent    EV_COMMANDS_43                   ("COMMANDS_43",                  "Keyboard EXPL+ALT+F7");
GameInputEvent    EV_COMMANDS_44                   ("COMMANDS_44",                  "Keyboard EXPL+ALT+F8");
GameInputEvent    EV_COMMANDS_45                   ("COMMANDS_45",                  "Keyboard EXPL+ALT+F9");
GameInputEvent    EV_COMMANDS_46                   ("COMMANDS_46",                  "Keyboard EXPL+ALT+F10");
GameInputEvent    EV_COMMANDS_47                   ("COMMANDS_47",                  "Keyboard EXPL+ALT+F11");
GameInputEvent    EV_COMMANDS_48                   ("COMMANDS_48",                  "Keyboard EXPL+ALT+F12");
GameInputEvent    EV_COMMANDS_49                   ("COMMANDS_49",                  "Keyboard EXPL+CTRL+SHIFT+F1");
GameInputEvent    EV_COMMANDS_50                   ("COMMANDS_50",                  "Keyboard EXPL+CTRL+SHIFT+F2");
GameInputEvent    EV_COMMANDS_51                   ("COMMANDS_51",                  "Keyboard EXPL+CTRL+SHIFT+F3");
GameInputEvent    EV_COMMANDS_52                   ("COMMANDS_52",                  "Keyboard EXPL+CTRL+SHIFT+F4");
GameInputEvent    EV_COMMANDS_53                   ("COMMANDS_53",                  "Keyboard EXPL+CTRL+SHIFT+F5");
GameInputEvent    EV_COMMANDS_54                   ("COMMANDS_54",                  "Keyboard EXPL+CTRL+SHIFT+F6");
GameInputEvent    EV_COMMANDS_55                   ("COMMANDS_55",                  "Keyboard EXPL+CTRL+SHIFT+F7");
GameInputEvent    EV_COMMANDS_56                   ("COMMANDS_56",                  "Keyboard EXPL+CTRL+SHIFT+F8");
GameInputEvent    EV_COMMANDS_57                   ("COMMANDS_57",                  "Keyboard EXPL+CTRL+SHIFT+F9");
GameInputEvent    EV_COMMANDS_58                   ("COMMANDS_58",                  "Keyboard EXPL+CTRL+SHIFT+F10");
GameInputEvent    EV_COMMANDS_59                   ("COMMANDS_59",                  "Keyboard EXPL+CTRL+SHIFT+F11");
GameInputEvent    EV_COMMANDS_60                   ("COMMANDS_60",                  "Keyboard EXPL+CTRL+SHIFT+F12");
GameInputEvent    EV_COMMANDS_61                   ("COMMANDS_61",                  "Keyboard EXPL+CTRL+ALT+F1");
GameInputEvent    EV_COMMANDS_62                   ("COMMANDS_62",                  "Keyboard EXPL+CTRL+ALT+F2");
GameInputEvent    EV_COMMANDS_63                   ("COMMANDS_63",                  "Keyboard EXPL+CTRL+ALT+F3");
GameInputEvent    EV_COMMANDS_64                   ("COMMANDS_64",                  "Keyboard EXPL+CTRL+ALT+F4");
GameInputEvent    EV_COMMANDS_65                   ("COMMANDS_65",                  "Keyboard EXPL+CTRL+ALT+F5");
GameInputEvent    EV_COMMANDS_66                   ("COMMANDS_66",                  "Keyboard EXPL+CTRL+ALT+F6");
GameInputEvent    EV_COMMANDS_67                   ("COMMANDS_67",                  "Keyboard EXPL+CTRL+ALT+F7");
GameInputEvent    EV_COMMANDS_68                   ("COMMANDS_68",                  "Keyboard EXPL+CTRL+ALT+F8");
GameInputEvent    EV_COMMANDS_69                   ("COMMANDS_69",                  "Keyboard EXPL+CTRL+ALT+F9");
GameInputEvent    EV_COMMANDS_70                   ("COMMANDS_70",                  "Keyboard EXPL+CTRL+ALT+F10");
GameInputEvent    EV_COMMANDS_71                   ("COMMANDS_71",                  "Keyboard EXPL+CTRL+ALT+F11");
GameInputEvent    EV_COMMANDS_72                   ("COMMANDS_72",                  "Keyboard EXPL+CTRL+ALT+F12");
GameInputEvent    EV_COMMANDS_73                   ("COMMANDS_73",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F1");
GameInputEvent    EV_COMMANDS_74                   ("COMMANDS_74",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F2");
GameInputEvent    EV_COMMANDS_75                   ("COMMANDS_75",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F3");
GameInputEvent    EV_COMMANDS_76                   ("COMMANDS_76",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F4");
GameInputEvent    EV_COMMANDS_77                   ("COMMANDS_77",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F5");
GameInputEvent    EV_COMMANDS_78                   ("COMMANDS_78",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F6");
GameInputEvent    EV_COMMANDS_79                   ("COMMANDS_79",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F7");
GameInputEvent    EV_COMMANDS_80                   ("COMMANDS_80",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F8");
GameInputEvent    EV_COMMANDS_81                   ("COMMANDS_81",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F9");
GameInputEvent    EV_COMMANDS_82                   ("COMMANDS_82",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F10");
GameInputEvent    EV_COMMANDS_83                   ("COMMANDS_83",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F11");
GameInputEvent    EV_COMMANDS_84                   ("COMMANDS_84",                  "Keyboard EXPL+CTRL+SHIFT+ALT+F12");

GameInputEvent    EV_COMMON_ACCELERATE_SIMULATION  ("COMMON_ACCELERATE_SIMULATION", "Keyboard CTRL+EQUALS");   //!< accelerate the simulation speed
GameInputEvent    EV_COMMON_DECELERATE_SIMULATION  ("COMMON_DECELERATE_SIMULATION", "Keyboard SHIFT+EQUALS");  //!< decelerate the simulation speed
GameInputEvent    EV_COMMON_RESET_SIMULATION_PACE  ("COMMON_RESET_SIMULATION_PACE", "Keyboard BACKSLASH");     //!< reset the simulation speed
GameInputEvent    EV_COMMON_AUTOLOCK               ("COMMON_AUTOLOCK",              "Keyboard EXPL+ALT+L");    //!< unlock autolock hook node
GameInputEvent    EV_COMMON_CONSOLE_TOGGLE         ("COMMON_CONSOLE_TOGGLE",        "Keyboard EXPL+GRAVE");    //!< show / hide the console
GameInputEvent    EV_COMMON_ENTER_CHATMODE         ("COMMON_ENTER_CHATMODE",        "Keyboard Y");             //!< enter the chat mode
GameInputEvent    EV_COMMON_ENTER_OR_EXIT_TRUCK    ("COMMON_ENTER_OR_EXIT_TRUCK",   "Keyboard RETURN");               //!< enter or exit a truck
GameInputEvent    EV_COMMON_ENTER_NEXT_TRUCK       ("COMMON_ENTER_NEXT_TRUCK",      "Keyboard EXPL+CTRL+RBRACKET");   //!< enter next truck
GameInputEvent    EV_COMMON_ENTER_PREVIOUS_TRUCK   ("COMMON_ENTER_PREVIOUS_TRUCK",  "Keyboard EXPL+CTRL+LBRACKET");   //!< enter previous truck
GameInputEvent    EV_COMMON_REMOVE_CURRENT_TRUCK   ("COMMON_REMOVE_CURRENT_TRUCK",  "Keyboard EXPL+CTRL+DELETE");     //!< remove current truck
GameInputEvent    EV_COMMON_RESPAWN_LAST_TRUCK     ("COMMON_RESPAWN_LAST_TRUCK",    "Keyboard EXPL+CTRL+PERIOD");     //!< respawn last truck
GameInputEvent    EV_COMMON_FOV_LESS               ("COMMON_FOV_LESS",              "Keyboard EXPL+NUMPAD7");         //!<decreases the current FOV value
GameInputEvent    EV_COMMON_FOV_MORE               ("COMMON_FOV_MORE",              "Keyboard EXPL+CTRL+NUMPAD7");    //!<increases the current FOV value
GameInputEvent    EV_COMMON_FULLSCREEN_TOGGLE      ("COMMON_FULLSCREEN_TOGGLE",     "Keyboard EXPL+ALT+RETURN");
GameInputEvent    EV_COMMON_HIDE_GUI               ("COMMON_HIDE_GUI",              "Keyboard EXPL+U");               //!< hide all GUI elements
GameInputEvent    EV_COMMON_LOCK                   ("COMMON_LOCK",                  "Keyboard EXPL+L");               //!< connect hook node to a node in close proximity
GameInputEvent    EV_COMMON_NETCHATDISPLAY         ("COMMON_NETCHATDISPLAY",        "Keyboard EXPL+SHIFT+U");
GameInputEvent    EV_COMMON_NETCHATMODE            ("COMMON_NETCHATMODE",           "Keyboard EXPL+CTRL+U");
GameInputEvent    EV_COMMON_OUTPUT_POSITION        ("COMMON_OUTPUT_POSITION",       "Keyboard H");                    //!< write current position to log (you can open the logfile and reuse the position)
GameInputEvent    EV_COMMON_GET_NEW_VEHICLE        ("COMMON_GET_NEW_VEHICLE",       "Keyboard EXPL+CTRL+G");          //!< get new vehicle
GameInputEvent    EV_COMMON_PRESSURE_LESS          ("COMMON_PRESSURE_LESS",         "Keyboard LBRACKET");             //!< decrease tire pressure (note: only very few trucks support this)
GameInputEvent    EV_COMMON_PRESSURE_MORE          ("COMMON_PRESSURE_MORE",         "Keyboard RBRACKET");             //!< increase tire pressure (note: only very few trucks support this)
GameInputEvent    EV_COMMON_QUIT_GAME              ("COMMON_QUIT_GAME",             "Keyboard EXPL+ESCAPE");          //!< exit the game
GameInputEvent    EV_COMMON_REPAIR_TRUCK           ("COMMON_REPAIR_TRUCK",          "Keyboard BACK");                 //!< repair truck to original condition
GameInputEvent    EV_COMMON_REPLAY_BACKWARD        ("COMMON_REPLAY_BACKWARD",       "Keyboard EXPL+LEFT");
GameInputEvent    EV_COMMON_REPLAY_FAST_BACKWARD   ("COMMON_REPLAY_FAST_BACKWARD",  "Keyboard EXPL+SHIFT+LEFT");
GameInputEvent    EV_COMMON_REPLAY_FAST_FORWARD    ("COMMON_REPLAY_FAST_FORWARD",   "Keyboard EXPL+SHIFT+RIGHT");
GameInputEvent    EV_COMMON_REPLAY_FORWARD         ("COMMON_REPLAY_FORWARD",        "Keyboard EXPL+RIGHT");
GameInputEvent    EV_COMMON_RESCUE_TRUCK           ("COMMON_RESCUE_TRUCK",          "Keyboard EXPL+R");               //!< teleport to rescue truck
GameInputEvent    EV_COMMON_RESET_TRUCK            ("COMMON_RESET_TRUCK",           "Keyboard I");                    //!< reset truck to original starting position
GameInputEvent    EV_COMMON_ROPELOCK               ("COMMON_ROPELOCK",              "Keyboard EXPL+CTRL+L");          //!< connect hook node to a node in close proximity
GameInputEvent    EV_COMMON_SAVE_TERRAIN           ("COMMON_SAVE_TERRAIN",          "Keyboard EXPL+ALT+SHIF+CTRL+M"); //!< save terrain mesh to file (currently unused)
GameInputEvent    EV_COMMON_SCREENSHOT             ("COMMON_SCREENSHOT",            "Keyboard EXPL+SYSRQ");           //!< take a screenshot
GameInputEvent    EV_COMMON_SCREENSHOT_BIG         ("COMMON_SCREENSHOT_BIG",        "Keyboard EXPL+CTRL+SYSRQ");      //!< take a BIG screenshot
GameInputEvent    EV_COMMON_SECURE_LOAD            ("COMMON_SECURE_LOAD",           "Keyboard O");                    //!< tie a load to the truck
GameInputEvent    EV_COMMON_SEND_CHAT              ("COMMON_SEND_CHAT",             "Keyboard RETURN");               //!< send the chat text
GameInputEvent    EV_COMMON_SHOW_SKELETON          ("COMMON_SHOW_SKELETON",         "Keyboard K");                    //!< toggle skeleton display mode
GameInputEvent    EV_COMMON_TOGGLE_TERRAIN_EDITOR  ("COMMON_TOGGLE_TERRAIN_EDITOR",   "Keyboard EXPL+SHIFT+Y");       //!< toggle terrain editor
GameInputEvent    EV_COMMON_TOGGLE_CUSTOM_PARTICLES("COMMON_TOGGLE_CUSTOM_PARTICLES", "Keyboard G");                  //!< toggle particle cannon
GameInputEvent    EV_COMMON_TOGGLE_MAT_DEBUG       ("COMMON_TOGGLE_MAT_DEBUG",        "");                            //!< debug purpose - dont use
GameInputEvent    EV_COMMON_TOGGLE_RENDER_MODE     ("COMMON_TOGGLE_RENDER_MODE",    "Keyboard E");                    //!< toggle render mode (solid wireframe and points)
GameInputEvent    EV_COMMON_TOGGLE_REPLAY_MODE     ("COMMON_TOGGLE_REPLAY_MODE",    "Keyboard J");                    //!< deprecated key - without function
GameInputEvent    EV_COMMON_TOGGLE_STATS           ("COMMON_TOGGLE_STATS",          "Keyboard EXPL+F");               //!< toggle Ogre statistics (FPS etc.)
GameInputEvent    EV_COMMON_TOGGLE_TRUCK_BEACONS   ("COMMON_TOGGLE_TRUCK_BEACONS",  "Keyboard M");                    //!< toggle truck beacons
GameInputEvent    EV_COMMON_TOGGLE_TRUCK_LIGHTS    ("COMMON_TOGGLE_TRUCK_LIGHTS",   "Keyboard N");                    //!< toggle truck front lights
GameInputEvent    EV_COMMON_TRUCK_INFO             ("COMMON_TRUCK_INFO",            "Keyboard EXPL+T");               //!< toggle truck HUD
GameInputEvent    EV_COMMON_TRUCK_DESCRIPTION      ("COMMON_TRUCK_DESCRIPTION",     "Keyboard EXPL+CTRL+T");          //!< toggle truck description
GameInputEvent    EV_COMMON_TRUCK_REMOVE           ("COMMON_TRUCK_REMOVE",          "Keyboard EXPL+CTRL+SHIFT+DELETE");
GameInputEvent    EV_COMMON_TELEPORT_TOGGLE        ("COMMON_TELEPORT_TOGGLE",       "Keyboard EXPL+F2");              //!< Enter/exit teleport mode (Only in character mode)

GameInputEvent    EV_GRASS_LESS                    ("GRASS_LESS",                   ""); //!< EXPERIMENTAL: remove some grass
GameInputEvent    EV_GRASS_MORE                    ("GRASS_MORE",                   ""); //!< EXPERIMENTAL: add some grass
GameInputEvent    EV_GRASS_MOST                    ("GRASS_MOST",                   ""); //!< EXPERIMENTAL: set maximum amount of grass
GameInputEvent    EV_GRASS_NONE                    ("GRASS_NONE",                   ""); //!< EXPERIMENTAL: remove grass completely
GameInputEvent    EV_GRASS_SAVE                    ("GRASS_SAVE",                   ""); //!< EXPERIMENTAL: save changes to the grass density image

GameInputEvent    EV_MENU_DOWN                     ("MENU_DOWN",                    "Keyboard DOWN");                 //!< select next element in current category
GameInputEvent    EV_MENU_LEFT                     ("MENU_LEFT",                    "Keyboard LEFT");                 //!< select previous category
GameInputEvent    EV_MENU_RIGHT                    ("MENU_RIGHT",                   "Keyboard RIGHT");                //!< select next category
GameInputEvent    EV_MENU_SELECT                   ("MENU_SELECT",                  "Keyboard EXPL+RETURN");          //!< select focussed item and close menu
GameInputEvent    EV_MENU_UP                       ("MENU_UP",                      "Keyboard UP");                   //select previous element in current category

GameInputEvent    EV_SURVEY_MAP_ALPHA              ("SURVEY_MAP_ALPHA",             "Keyboard EXPL+CTRL+SHIFT+TAB");  //!< toggle translucency of overview-map
GameInputEvent    EV_SURVEY_MAP_TOGGLE_ICONS       ("SURVEY_MAP_TOGGLE_ICONS",      "Keyboard EXPL+CTRL+SHIFT+ALT+TAB"); //!< toggle map icons
GameInputEvent    EV_SURVEY_MAP_TOGGLE_VIEW        ("SURVEY_MAP_TOGGLE_VIEW",       "Keyboard EXPL+TAB");             //!< toggle map modes
GameInputEvent    EV_SURVEY_MAP_ZOOM_IN            ("SURVEY_MAP_ZOOM_IN",           "Keyboard EXPL+CTRL+TAB");        //!< increase survey map scale
GameInputEvent    EV_SURVEY_MAP_ZOOM_OUT           ("SURVEY_MAP_ZOOM_OUT",          "Keyboard EXPL+SHIFT+TAB");       //!< decrease survey map scale

GameInputEvent    EV_TRUCK_ACCELERATE              ("TRUCK_ACCELERATE",             "Keyboard UP");                   //!< accelerate the truck
GameInputEvent    EV_TRUCK_ACCELERATE_MODIFIER_25  ("TRUCK_ACCELERATE_MODIFIER_25", "Keyboard ALT+UP");               //!< accelerate with 25 percent pedal input
GameInputEvent    EV_TRUCK_ACCELERATE_MODIFIER_50  ("TRUCK_ACCELERATE_MODIFIER_50", "Keyboard CTRL+UP");              //!< accelerate with 50 percent pedal input
GameInputEvent    EV_TRUCK_ANTILOCK_BRAKE          ("TRUCK_ANTILOCK_BRAKE",         "Keyboard EXPL+SHIFT+B");         //!< toggle antilockbrake system
GameInputEvent    EV_TRUCK_AUTOSHIFT_DOWN          ("TRUCK_AUTOSHIFT_DOWN",         "Keyboard PGDOWN");               //!< shift automatic transmission one gear down
GameInputEvent    EV_TRUCK_AUTOSHIFT_UP            ("TRUCK_AUTOSHIFT_UP",           "Keyboard PGUP");                 //!< shift automatic transmission one gear up
GameInputEvent    EV_TRUCK_BLINK_LEFT              ("TRUCK_BLINK_LEFT",             "Keyboard COMMA");                //!< toggle left direction indicator (blinker)
GameInputEvent    EV_TRUCK_BLINK_RIGHT             ("TRUCK_BLINK_RIGHT",            "Keyboard PERIOD");               //!< toggle right direction indicator (blinker)
GameInputEvent    EV_TRUCK_BLINK_WARN              ("TRUCK_BLINK_WARN",             "Keyboard MINUS");                //!< toggle all direction indicators
GameInputEvent    EV_TRUCK_BRAKE                   ("TRUCK_BRAKE",                  "Keyboard DOWN");                 //!< brake
GameInputEvent    EV_TRUCK_BRAKE_MODIFIER_25       ("TRUCK_BRAKE_MODIFIER_25",      "Keyboard ALT+DOWN");             //!< brake with 25 percent pedal input
GameInputEvent    EV_TRUCK_BRAKE_MODIFIER_50       ("TRUCK_BRAKE_MODIFIER_50",      "Keyboard CTRL+DOWN");            //!< brake with 50 percent pedal input
GameInputEvent    EV_TRUCK_CRUISE_CONTROL          ("TRUCK_CRUISE_CONTROL",         "Keyboard EXPL+SPACE");           //!< toggle cruise control
GameInputEvent    EV_TRUCK_CRUISE_CONTROL_ACCL     ("TRUCK_CRUISE_CONTROL_ACCL",    "Keyboard EXPL+CTRL+R");          //!< increase target speed / rpm
GameInputEvent    EV_TRUCK_CRUISE_CONTROL_DECL     ("TRUCK_CRUISE_CONTROL_DECL",    "Keyboard EXPL+CTRL+F");          //!< decrease target speed / rpm
GameInputEvent    EV_TRUCK_CRUISE_CONTROL_READJUST ("TRUCK_CRUISE_CONTROL_READJUST","Keyboard EXPL+CTRL+SPACE");      //!< match target speed / rpm with current truck speed / rpm
GameInputEvent    EV_TRUCK_HORN                    ("TRUCK_HORN",                   "Keyboard H");                    //!< truck horn
GameInputEvent    EV_TRUCK_LEFT_MIRROR_LEFT        ("TRUCK_LEFT_MIRROR_LEFT",       "Keyboard EXPL+SEMICOLON");
GameInputEvent    EV_TRUCK_LEFT_MIRROR_RIGHT       ("TRUCK_LEFT_MIRROR_RIGHT",      "Keyboard EXPL+CTRL+SEMICOLON");
GameInputEvent    EV_TRUCK_LIGHTTOGGLE01           ("TRUCK_LIGHTTOGGLE01",          "Keyboard EXPL+CTRL+1");          //!< toggle custom light 1
GameInputEvent    EV_TRUCK_LIGHTTOGGLE02           ("TRUCK_LIGHTTOGGLE02",          "Keyboard EXPL+CTRL+2");          //!< toggle custom light 2
GameInputEvent    EV_TRUCK_LIGHTTOGGLE03           ("TRUCK_LIGHTTOGGLE03",          "Keyboard EXPL+CTRL+3");          //!< toggle custom light 3
GameInputEvent    EV_TRUCK_LIGHTTOGGLE04           ("TRUCK_LIGHTTOGGLE04",          "Keyboard EXPL+CTRL+4");          //!< toggle custom light 4
GameInputEvent    EV_TRUCK_LIGHTTOGGLE05           ("TRUCK_LIGHTTOGGLE05",          "Keyboard EXPL+CTRL+5");          //!< toggle custom light 5
GameInputEvent    EV_TRUCK_LIGHTTOGGLE06           ("TRUCK_LIGHTTOGGLE06",          "Keyboard EXPL+CTRL+6");          //!< toggle custom light 6
GameInputEvent    EV_TRUCK_LIGHTTOGGLE07           ("TRUCK_LIGHTTOGGLE07",          "Keyboard EXPL+CTRL+7");          //!< toggle custom light 7
GameInputEvent    EV_TRUCK_LIGHTTOGGLE08           ("TRUCK_LIGHTTOGGLE08",          "Keyboard EXPL+CTRL+8");          //!< toggle custom light 8
GameInputEvent    EV_TRUCK_LIGHTTOGGLE09           ("TRUCK_LIGHTTOGGLE09",          "Keyboard EXPL+CTRL+9");          //!< toggle custom light 9
GameInputEvent    EV_TRUCK_LIGHTTOGGLE10           ("TRUCK_LIGHTTOGGLE10",          "Keyboard EXPL+CTRL+0");          //!< toggle custom light 10
GameInputEvent    EV_TRUCK_MANUAL_CLUTCH           ("TRUCK_MANUAL_CLUTCH",          "Keyboard LSHIFT");               //!< manual clutch (for manual transmission)
GameInputEvent    EV_TRUCK_PARKING_BRAKE           ("TRUCK_PARKING_BRAKE",          "Keyboard P");                    //!< toggle parking brake
GameInputEvent    EV_TRUCK_RIGHT_MIRROR_LEFT       ("TRUCK_RIGHT_MIRROR_LEFT",      "Keyboard EXPL+COLON");                          
GameInputEvent    EV_TRUCK_RIGHT_MIRROR_RIGHT      ("TRUCK_RIGHT_MIRROR_RIGHT",     "Keyboard EXPL+CTRL+COLON");                            
GameInputEvent    EV_TRUCK_SHIFT_DOWN              ("TRUCK_SHIFT_DOWN",             "Keyboard Z");                    //!< shift one gear down in manual transmission mode
GameInputEvent    EV_TRUCK_SHIFT_GEAR01            ("TRUCK_SHIFT_GEAR01",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR02            ("TRUCK_SHIFT_GEAR02",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR03            ("TRUCK_SHIFT_GEAR03",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR04            ("TRUCK_SHIFT_GEAR04",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR05            ("TRUCK_SHIFT_GEAR05",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR06            ("TRUCK_SHIFT_GEAR06",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR07            ("TRUCK_SHIFT_GEAR07",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR08            ("TRUCK_SHIFT_GEAR08",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR09            ("TRUCK_SHIFT_GEAR09",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR10            ("TRUCK_SHIFT_GEAR10",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR11            ("TRUCK_SHIFT_GEAR11",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR12            ("TRUCK_SHIFT_GEAR12",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR13            ("TRUCK_SHIFT_GEAR13",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR14            ("TRUCK_SHIFT_GEAR14",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR15            ("TRUCK_SHIFT_GEAR15",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR16            ("TRUCK_SHIFT_GEAR16",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR17            ("TRUCK_SHIFT_GEAR17",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR18            ("TRUCK_SHIFT_GEAR18",           "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_GEAR_REVERSE      ("TRUCK_SHIFT_GEAR_REVERSE",     "");                              //!< shift directly into this gear
GameInputEvent    EV_TRUCK_SHIFT_HIGHRANGE         ("TRUCK_SHIFT_HIGHRANGE",        "");                              //!< select high range (13-18) for H-shaft
GameInputEvent    EV_TRUCK_SHIFT_LOWRANGE          ("TRUCK_SHIFT_LOWRANGE",         "");                              //!< select low range (1-6) for H-shaft
GameInputEvent    EV_TRUCK_SHIFT_MIDRANGE          ("TRUCK_SHIFT_MIDRANGE",         "");                              //!< select middle range (7-12) for H-shaft
GameInputEvent    EV_TRUCK_SHIFT_NEUTRAL           ("TRUCK_SHIFT_NEUTRAL",          "Keyboard D");                    //!< shift to neutral gear in manual transmission mode
GameInputEvent    EV_TRUCK_SHIFT_UP                ("TRUCK_SHIFT_UP",               "Keyboard A");                    //!< shift one gear up in manual transmission mode
GameInputEvent    EV_TRUCK_STARTER                 ("TRUCK_STARTER",                "Keyboard S");                    //!< hold to start the engine
GameInputEvent    EV_TRUCK_STEER_LEFT              ("TRUCK_STEER_LEFT",             "Keyboard LEFT");                 //!< steer left
GameInputEvent    EV_TRUCK_STEER_RIGHT             ("TRUCK_STEER_RIGHT",            "Keyboard RIGHT");                //!< steer right
GameInputEvent    EV_TRUCK_SWITCH_SHIFT_MODES      ("TRUCK_SWITCH_SHIFT_MODES",     "Keyboard Q");                    //!< toggle between transmission modes
GameInputEvent    EV_TRUCK_TOGGLE_AXLE_LOCK        ("TRUCK_TOGGLE_AXLE_LOCK",       "Keyboard W");                        
GameInputEvent    EV_TRUCK_TOGGLE_CONTACT          ("TRUCK_TOGGLE_CONTACT",         "Keyboard X");                    //!< toggle engine ignition
GameInputEvent    EV_TRUCK_TOGGLE_FORWARDCOMMANDS  ("TRUCK_TOGGLE_FORWARDCOMMANDS", "Keyboard EXPL+CTRL+SHIFT+F");    //!< toggle forwardcommands
GameInputEvent    EV_TRUCK_TOGGLE_IMPORTCOMMANDS   ("TRUCK_TOGGLE_IMPORTCOMMANDS",  "Keyboard EXPL+CTRL+SHIFT+I");    //!< toggle importcommands
GameInputEvent    EV_TRUCK_TOGGLE_VIDEOCAMERA      ("TRUCK_TOGGLE_VIDEOCAMERA",     "Keyboard EXPL+CTRL+V");          //!< toggle videocamera update
GameInputEvent    EV_TRUCK_TRACTION_CONTROL        ("TRUCK_TRACTION_CONTROL",       "Keyboard EXPL+SHIFT+T");         //!< toggle antilockbrake system

GameInputEvent    EV_TRUCK_SAVE_POS01              ("TRUCK_SAVE_POS01",             "Keyboard EXPL+ALT+CTRL+1");
GameInputEvent    EV_TRUCK_SAVE_POS02              ("TRUCK_SAVE_POS02",             "Keyboard EXPL+ALT+CTRL+2");
GameInputEvent    EV_TRUCK_SAVE_POS03              ("TRUCK_SAVE_POS03",             "Keyboard EXPL+ALT+CTRL+3");
GameInputEvent    EV_TRUCK_SAVE_POS04              ("TRUCK_SAVE_POS04",             "Keyboard EXPL+ALT+CTRL+4");
GameInputEvent    EV_TRUCK_SAVE_POS05              ("TRUCK_SAVE_POS05",             "Keyboard EXPL+ALT+CTRL+5");
GameInputEvent    EV_TRUCK_SAVE_POS06              ("TRUCK_SAVE_POS06",             "Keyboard EXPL+ALT+CTRL+6");
GameInputEvent    EV_TRUCK_SAVE_POS07              ("TRUCK_SAVE_POS07",             "Keyboard EXPL+ALT+CTRL+7");
GameInputEvent    EV_TRUCK_SAVE_POS08              ("TRUCK_SAVE_POS08",             "Keyboard EXPL+ALT+CTRL+8");
GameInputEvent    EV_TRUCK_SAVE_POS09              ("TRUCK_SAVE_POS09",             "Keyboard EXPL+ALT+CTRL+9");
GameInputEvent    EV_TRUCK_SAVE_POS10              ("TRUCK_SAVE_POS10",             "Keyboard EXPL+ALT+CTRL+0");

GameInputEvent    EV_TRUCK_LOAD_POS01              ("TRUCK_LOAD_POS01",             "Keyboard EXPL+ALT+1");
GameInputEvent    EV_TRUCK_LOAD_POS02              ("TRUCK_LOAD_POS02",             "Keyboard EXPL+ALT+2");
GameInputEvent    EV_TRUCK_LOAD_POS03              ("TRUCK_LOAD_POS03",             "Keyboard EXPL+ALT+3");
GameInputEvent    EV_TRUCK_LOAD_POS04              ("TRUCK_LOAD_POS04",             "Keyboard EXPL+ALT+4");
GameInputEvent    EV_TRUCK_LOAD_POS05              ("TRUCK_LOAD_POS05",             "Keyboard EXPL+ALT+5");
GameInputEvent    EV_TRUCK_LOAD_POS06              ("TRUCK_LOAD_POS06",             "Keyboard EXPL+ALT+6");
GameInputEvent    EV_TRUCK_LOAD_POS07              ("TRUCK_LOAD_POS07",             "Keyboard EXPL+ALT+7");
GameInputEvent    EV_TRUCK_LOAD_POS08              ("TRUCK_LOAD_POS08",             "Keyboard EXPL+ALT+8");
GameInputEvent    EV_TRUCK_LOAD_POS09              ("TRUCK_LOAD_POS09",             "Keyboard EXPL+ALT+9");
GameInputEvent    EV_TRUCK_LOAD_POS10              ("TRUCK_LOAD_POS10",             "Keyboard EXPL+ALT+0");

GameInputEvent    EV_DOF_TOGGLE                    ("DOF_TOGGLE",                   "Keyboard EXPL+CTRL+D");          ///< turn on Depth of Field on or off
GameInputEvent    EV_DOF_DEBUG                     ("DOF_DEBUG",                    "Keyboard EXPL+ALT+D");           ///< turn on the Depth of field debug view
GameInputEvent    EV_DOF_DEBUG_TOGGLE_FOCUS_MODE   ("DOF_DEBUG_TOGGLE_FOCUS_MODE",  "Keyboard EXPL+SPACE");           ///< toggle the DOF focus mode
GameInputEvent    EV_DOF_DEBUG_ZOOM_IN             ("DOF_DEBUG_ZOOM_IN",            "Keyboard EXPL+Q");
GameInputEvent    EV_DOF_DEBUG_ZOOM_OUT            ("DOF_DEBUG_ZOOM_OUT",           "Keyboard EXPL+Z");
GameInputEvent    EV_DOF_DEBUG_APERTURE_MORE       ("DOF_DEBUG_APERTURE_MORE",      "Keyboard EXPL+1");
GameInputEvent    EV_DOF_DEBUG_APERTURE_LESS       ("DOF_DEBUG_APERTURE_LESS",      "Keyboard EXPL+2");
GameInputEvent    EV_DOF_DEBUG_FOCUS_IN            ("DOF_DEBUG_FOCUS_IN",           "Keyboard EXPL+3");               ///< move focus in
GameInputEvent    EV_DOF_DEBUG_FOCUS_OUT           ("DOF_DEBUG_FOCUS_OUT",          "Keyboard EXPL+4");               ///< move focus out

GameInputEvent    EV_TRUCKEDIT_RELOAD              ("TRUCKEDIT_RELOAD",             "Keyboard EXPL+SHIFT+CTRL+R");    ///< reload truck
GameInputEvent    EV_TOGGLESHADERS                 ("TOGGLESHADERS",                "Keyboard EXPL+SHIFT+CTRL+S");    ///< toggle shader usage

} // namespace RoR

//                                                             @@@@@@@@@
// ############################################################## OLD #############################################################
//                                                             @@@@@@@@@



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

#ifndef NOOGRE

#include "GUIManager.h"
#include "Language.h"
#else
#define _L(x) x
#endif

const char* mOISDeviceType[6] = {"Unknown Device", "Keyboard", "Mouse", "JoyStick", "Tablet", "Other Device"};




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

#endif

using namespace std;
using namespace Ogre;
using namespace OIS;

// Constructor takes a RenderWindow because it uses that to determine input context
InputEngine::InputEngine() :
    captureMode(false)
    , free_joysticks(0)
    , inputsChanged(true)
    , mForceFeedback(0)
    , mInputManager(0)
    , mKeyboard(0)
    , mMouse(0)
    , mappingLoaded(false)
    , uniqueCounter(0)
{
    for (int i = 0; i < MAX_JOYSTICKS; i++)
        mJoy[i] = 0;
#ifndef NOOGRE
    LOG("*** Loading OIS ***");
#endif
    initAllKeys();
}

InputEngine::~InputEngine()
{
#ifndef NOOGRE
    LOG("*** Terminating destructor ***");
#endif
    destroy();
}

void InputEngine::destroy()
{
    if (mInputManager)
    {
#ifndef NOOGRE
        LOG("*** Terminating OIS ***");
#endif
        if (mMouse)
        {
            mInputManager->destroyInputObject(mMouse);
            mMouse = 0;
        }
        if (mKeyboard)
        {
            mInputManager->destroyInputObject(mKeyboard);
            mKeyboard = 0;
        }
        if (mJoy)
        {
            for (int i = 0; i < MAX_JOYSTICKS; i++)
            {
                if (!mJoy[i])
                    continue;
                mInputManager->destroyInputObject(mJoy[i]);
                mJoy[i] = 0;
            }
        }

        OIS::InputManager::destroyInputSystem(mInputManager);
        mInputManager = 0;
    }
}

bool InputEngine::setup(String hwnd, bool capture, bool capturemouse, bool captureKbd)
{
#ifndef NOOGRE
    LOG("*** Initializing OIS ***");
#endif
    //try to delete old ones first (linux can only handle one at a time)
    destroy();
    captureMode = capture;
    if (captureMode)
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
        if (RoR::App::GetIoInputGrabMode() != RoR::App::INPUT_GRAB_ALL)
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

#ifndef NOOGRE
        LOG("*** OIS WINDOW: "+hwnd);
#endif //NOOGRE

        mInputManager = OIS::InputManager::createInputSystem(pl);

#ifndef NOOGRE
        //Print debugging information
        unsigned int v = mInputManager->getVersionNumber();
        LOG("OIS Version: " + TOSTRING(v>>16) + String(".") + TOSTRING((v>>8) & 0x000000FF) + String(".") + TOSTRING(v & 0x000000FF));
        LOG("+ Release Name: " + mInputManager->getVersionName());
        LOG("+ Manager: " + mInputManager->inputSystemName());
        LOG("+ Total Keyboards: " + TOSTRING(mInputManager->getNumberOfDevices(OISKeyboard)));
        LOG("+ Total Mice: " + TOSTRING(mInputManager->getNumberOfDevices(OISMouse)));
        LOG("+ Total JoySticks: " + TOSTRING(mInputManager->getNumberOfDevices(OISJoyStick)));

        //List all devices
        OIS::DeviceList deviceList = mInputManager->listFreeDevices();
        for (OIS::DeviceList::iterator i = deviceList.begin(); i != deviceList.end(); ++i)
        LOG("* Device: " + String(mOISDeviceType[i->first]) + String(" Vendor: ") + i->second);
#endif //NOOGRE

        //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
        mKeyboard = 0;
        if (captureKbd)
        {
            try
            {
                mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject(OISKeyboard, true));
                mKeyboard->setTextTranslation(OIS::Keyboard::Unicode);
            }
            catch (OIS::Exception& ex)
            {
                LOG(String("Exception raised on keyboard creation: ") + String(ex.eText));
            }
        }

        try
        {
            //This demo uses at most 10 joysticks - use old way to create (i.e. disregard vendor)
            int numSticks = std::min(mInputManager->getNumberOfDevices(OISJoyStick), 10);
            free_joysticks = 0;
            for (int i = 0; i < numSticks; ++i)
            {
                mJoy[i] = (JoyStick*)mInputManager->createInputObject(OISJoyStick, true);
                mJoy[i]->setEventCallback(this);
                free_joysticks++;
                //create force feedback too
                //here, we take the first device we can get, but we could take a device index
                if (!mForceFeedback)
                    mForceFeedback = (OIS::ForceFeedback*)mJoy[i]->queryInterface(OIS::Interface::ForceFeedback);

#ifndef NOOGRE
                LOG("Creating Joystick " + TOSTRING(i + 1) + " (" + mJoy[i]->vendor() + ")");
                LOG("* Axes: " + TOSTRING(mJoy[i]->getNumberOfComponents(OIS_Axis)));
                LOG("* Sliders: " + TOSTRING(mJoy[i]->getNumberOfComponents(OIS_Slider)));
                LOG("* POV/HATs: " + TOSTRING(mJoy[i]->getNumberOfComponents(OIS_POV)));
                LOG("* Buttons: " + TOSTRING(mJoy[i]->getNumberOfComponents(OIS_Button)));
                LOG("* Vector3: " + TOSTRING(mJoy[i]->getNumberOfComponents(OIS_Vector3)));
#endif //NOOGRE
            }
        }
#ifndef NOOGRE
        catch (OIS::Exception& ex)
        {
            LOG(String("Exception raised on joystick creation: ") + String(ex.eText));
        }
#else  //NOOGRE
        catch (...)
        {
        }
#endif //NOOGRE

        if (capturemouse)
        {
            try
            {
                mMouse = static_cast<Mouse*>(mInputManager->createInputObject(OISMouse, true));
            }
            catch (OIS::Exception& ex)
            {
                LOG(String("Exception raised on mouse creation: ") + String(ex.eText));
            }
        }

        //Set initial mouse clipping size
        //windowResized(win);

        // set Callbacks
        if (mKeyboard)
            mKeyboard->setEventCallback(this);
        if (capturemouse && mMouse)
        {
            mMouse->setEventCallback(this);

            // init states (not required for keyboard)
            mouseState = mMouse->getMouseState();
        }
        if (free_joysticks)
        {
            for (int i = 0; i < free_joysticks; i++)
                joyState[i] = mJoy[i]->getJoyStickState();
        }

        // set the mouse to the middle of the screen, hackish!
#if _WIN32
        // under linux, this will not work and the cursor will never reach (0,0)
        if (mMouse && RoR::App::GetOgreSubsystem()->GetRenderWindow())
        {
            OIS::MouseState& mutableMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
            mutableMouseState.X.abs = RoR::App::GetOgreSubsystem()->GetRenderWindow()->getWidth() * 0.5f;
            mutableMouseState.Y.abs = RoR::App::GetOgreSubsystem()->GetRenderWindow()->getHeight() * 0.5f;
        }
#endif // _WIN32
    }
    //this becomes more and more convoluted!
#ifdef NOOGRE
    // we will load the mapping manually
#else //NOOGRE
    if (!mappingLoaded)
    {
        // load default one
        loadMapping(CONFIGFILENAME);

#ifndef NOOGRE
        // then load device specific ones
        for (int i = 0; i < free_joysticks; ++i)
        {
            String deviceStr = mJoy[i]->vendor();

            // care about unsuitable chars
            String repl = "\\/ #@?!$%^&*()+=-><.:'|\";";
            for (unsigned int c = 0; c < repl.size(); c++)
            {
                deviceStr = StringUtil::replaceAll(deviceStr, repl.substr(c, 1), "_");
            }
            deviceStr += ".map";

            loadMapping(deviceStr, true, i);
        }
#endif //NOOGRE
        mappingLoaded = true;
        completeMissingEvents();

        return false;
    }
#endif //NOOGRE
    return true;
}

void InputEngine::grabMouse(bool enable)
{
    static int lastmode = -1;
    if (!mMouse)
        return;
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    if ((enable && lastmode == 0) || (!enable && lastmode == 1) || (lastmode == -1))
    {
        LOG("*** mouse grab: " + TOSTRING(enable));
    //((LinuxMouse *)mMouse)->grab(enable);
        lastmode = enable?1:0;
    }
#endif
}

void InputEngine::hideMouse(bool visible)
{
    static int mode = -1;
    if (!mMouse)
        return;
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    if ((visible && mode == 0) || (!visible && mode == 1) || mode == -1)
    {
    //((LinuxMouse *)mMouse)->hide(visible);
        mode = visible?1:0;
    }
#endif
}

void InputEngine::setMousePosition(int x, int y, bool padding)
{
    if (!mMouse)
        return;
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    // padding ensures that the mouse has a safety area at the window's borders
    //	((LinuxMouse *)mMouse)->setMousePosition(x, y, padding);
#endif
}

OIS::MouseState InputEngine::getMouseState()
{
    static float mX = 999999, mY = 999999;
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
        if (mX == 0 || mY == 0)
            mode = 2;
#else
        // no scaling without ogre
        mode = 2;
#endif
    }

    OIS::MouseState m;
    if (mMouse)
    {
        m = mMouse->getMouseState();
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
    for (allit = allkeys.begin(); allit != allkeys.end(); allit++)
    {
        if (allit->second == keycode)
            return allit->first;
    }
    return "unknown";
}

void InputEngine::Capture()
{
    if (mKeyboard)
    {
        mKeyboard->capture();
    }

    if (mMouse)
    {
        mMouse->capture();
    }

    for (int i = 0; i < free_joysticks; i++)
    {
        if (mJoy[i])
        {
            mJoy[i]->capture();
        }
    }
}

void InputEngine::windowResized(Ogre::RenderWindow* rw)
{
    if (!mMouse)
        return;
    //update mouse area
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
    const OIS::MouseState& ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
    RoR::App::GetGuiManager()->windowResized(rw);
}

void InputEngine::SetKeyboardListener(OIS::KeyListener* keyboard_listener)
{
    assert (mKeyboard != nullptr);
    mKeyboard->setEventCallback(keyboard_listener);
}

OIS::MouseState InputEngine::SetMouseListener(OIS::MouseListener* mouse_listener)
{
    assert (mMouse != nullptr);
    mMouse->setEventCallback(mouse_listener);
    return mMouse->getMouseState();
}

void InputEngine::RestoreMouseListener()
{
    if (mMouse)
    {
        mMouse->setEventCallback(this);

        // init states (not required for keyboard)
        mouseState = mMouse->getMouseState();
    }
}

void InputEngine::RestoreKeyboardListener()
{
    SetKeyboardListener(this);
}

/* --- Joystik Events ------------------------------------------ */
bool InputEngine::buttonPressed(const OIS::JoyStickEvent& arg, int button)
{
    inputsChanged = true;
    //LOG("*** buttonPressed " + TOSTRING(button));
    int i = arg.device->getID();
    if (i < 0 || i >= MAX_JOYSTICKS)
        i = 0;
    joyState[i] = arg.state;
    return true;
}

bool InputEngine::buttonReleased(const OIS::JoyStickEvent& arg, int button)
{
    inputsChanged = true;
    //LOG("*** buttonReleased " + TOSTRING(button));
    int i = arg.device->getID();
    if (i < 0 || i >= MAX_JOYSTICKS)
        i = 0;
    joyState[i] = arg.state;
    return true;
}

bool InputEngine::axisMoved(const OIS::JoyStickEvent& arg, int axis)
{
    inputsChanged = true;
    //LOG("*** axisMoved " + TOSTRING(axis) + " / " + TOSTRING((int)(arg.state.mAxes[axis].abs / (float)(mJoy->MAX_AXIS/100))));
    int i = arg.device->getID();
    if (i < 0 || i >= MAX_JOYSTICKS)
        i = 0;
    joyState[i] = arg.state;
    return true;
}

bool InputEngine::sliderMoved(const OIS::JoyStickEvent& arg, int)
{
    inputsChanged = true;
    //LOG("*** sliderMoved");
    int i = arg.device->getID();
    if (i < 0 || i >= MAX_JOYSTICKS)
        i = 0;
    joyState[i] = arg.state;
    return true;
}

bool InputEngine::povMoved(const OIS::JoyStickEvent& arg, int)
{
    inputsChanged = true;
    //LOG("*** povMoved");
    int i = arg.device->getID();
    if (i < 0 || i >= MAX_JOYSTICKS)
        i = 0;
    joyState[i] = arg.state;
    return true;
}

/* --- Key Events ------------------------------------------ */
bool InputEngine::keyPressed(const OIS::KeyEvent& arg)
{
    if (RoR::App::GetGuiManager()->keyPressed(arg))
        return true;

    //LOG("*** keyPressed");
    if (keyState[arg.key] != 1)
        inputsChanged = true;
    keyState[arg.key] = 1;

    return true;
}

bool InputEngine::keyReleased(const OIS::KeyEvent& arg)
{
    if (RoR::App::GetGuiManager()->keyReleased(arg))
        return true;
    //LOG("*** keyReleased");
    if (keyState[arg.key] != 0)
        inputsChanged = true;
    keyState[arg.key] = 0;
    return true;
}

/* --- Mouse Events ------------------------------------------ */
bool InputEngine::mouseMoved(const OIS::MouseEvent& arg)
{
    if (RoR::App::GetGuiManager()->mouseMoved(arg))
        return true;
    //LOG("*** mouseMoved");
    inputsChanged = true;
    mouseState = arg.state;
    return true;
}

bool InputEngine::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (RoR::App::GetGuiManager()->mousePressed(arg, id))
        return true;
    //LOG("*** mousePressed");
    inputsChanged = true;
    mouseState = arg.state;
    return true;
}

bool InputEngine::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
    if (RoR::App::GetGuiManager()->mouseReleased(arg, id))
        return true;
    //LOG("*** mouseReleased");
    inputsChanged = true;
    mouseState = arg.state;
    return true;
}

/* --- Custom Methods ------------------------------------------ */
void InputEngine::prepareShutdown()
{
#ifndef NOOGRE
    LOG("*** Inputsystem prepare for shutdown ***");
#endif
    destroy();
}

void InputEngine::resetKeys()
{
    for (std::map<int, bool>::iterator iter = keyState.begin(); iter != keyState.end(); ++iter)
    {
        iter->second = false;
    }
}

bool InputEngine::getEventBoolValue(int eventID)
{
    return (getEventValue(eventID) > 0.5f);
}

bool InputEngine::getEventBoolValueBounce(int eventID, float time)
{
    if (event_times[eventID] > 0)
        return false;
    else
    {
        bool res = getEventBoolValue(eventID);
        if (res)
            event_times[eventID] = time;
        return res;
    }
}

float InputEngine::getEventBounceTime(int eventID)
{
    return event_times[eventID];
}

void InputEngine::updateKeyBounces(float dt)
{
    for (std::map<int, float>::iterator it = event_times.begin(); it != event_times.end(); it++)
    {
        if (it->second > 0)
            it->second -= dt;
        //else
        // erase element?
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
        return (axisValue - dz) * (1.0f / (1.0f - dz));
}

float InputEngine::axisLinearity(float axisValue, float linearity)
{
    return (axisValue * linearity);
}

float InputEngine::logval(float val)
{
    if (val > 0)
        return log10(1.0 / (1.1 - val)) / 1.0;
    if (val == 0)
        return 0;
    return -log10(1.0 / (1.1 + val)) / 1.0;
}

void InputEngine::smoothValue(float& ref, float value, float rate)
{
    if (value < -1)
        value = -1;
    if (value > 1)
        value = 1;
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

String InputEngine::getEventCommand(int eventID)
{
    std::vector<event_trigger_t> t_vec = events[eventID];
    if (t_vec.size() > 0)
        return String(t_vec[0].configline);
    return "";
}

event_trigger_t* InputEngine::getEventBySUID(int suid)
{
    std::map<int, std::vector<event_trigger_t>>::iterator a;
    std::vector<event_trigger_t>::iterator b;
    for (a = events.begin(); a != events.end(); a++)
    {
        for (b = a->second.begin(); b != a->second.end(); b++)
        {
            if (b->suid == suid)
                return &(*b);
        }
    }
    return 0;
}

bool InputEngine::deleteEventBySUID(int suid)
{
    std::map<int, std::vector<event_trigger_t>>::iterator a;
    std::vector<event_trigger_t>::iterator b;
    for (a = events.begin(); a != events.end(); a++)
    {
        for (b = a->second.begin(); b != a->second.end(); b++)
        {
            if (b->suid == suid)
            {
                a->second.erase(b);
                return true;
            }
        }
    }
    return false;
}

bool InputEngine::isEventDefined(int eventID)
{
    std::vector<event_trigger_t> t_vec = events[eventID];
    if (t_vec.size() > 0)
    {
        if (t_vec[0].eventtype != ET_NONE)
            return true;
    }
    return false;
}

int InputEngine::getKeboardKeyForCommand(int eventID)
{
    std::vector<event_trigger_t> t_vec = events[eventID];
    for (std::vector<event_trigger_t>::iterator i = t_vec.begin(); i != t_vec.end(); i++)
    {
        event_trigger_t t = *i;
        if (t.eventtype == ET_Keyboard)
            return t.keyCode;
    }
    return -1;
}

bool InputEngine::isEventAnalog(int eventID)
{
    std::vector<event_trigger_t> t_vec = events[eventID];
    if (t_vec.size() > 0)
    {
        //loop through all eventtypes, because we want to find a analog device wether it is the first device or not
        //this means a analog device is always preferred over a digital one
        for (unsigned int i = 0; i < t_vec.size(); i++)
        {
            if ((t_vec[i].eventtype == ET_MouseAxisX
                    || t_vec[i].eventtype == ET_MouseAxisY
                    || t_vec[i].eventtype == ET_MouseAxisZ
                    || t_vec[i].eventtype == ET_JoystickAxisAbs
                    || t_vec[i].eventtype == ET_JoystickAxisRel
                    || t_vec[i].eventtype == ET_JoystickSliderX
                    || t_vec[i].eventtype == ET_JoystickSliderY)
                //check if value comes from analog device
                //this way, only valid events (e.g. joystick mapped, but unplugged) are recognized as analog events
                && getEventValue(eventID, true, 2) != 0.0)
            {
                return true;
            }
        }
    }
    return false;
#if 0
    // XXX : TODO fix this problem properly
    std::vector<event_trigger_t> t_vec = events[eventID];
    for (std::vector<event_trigger_t>::iterator i = t_vec.begin(); i != t_vec.end(); i++)
    {
        event_trigger_t t = *i;
        if (i->eventtype == ET_MouseAxisX \
            || i->eventtype == ET_MouseAxisX \
            || i->eventtype == ET_MouseAxisY \
            || i->eventtype == ET_MouseAxisZ \
            || i->eventtype == ET_JoystickAxisAbs \
            || i->eventtype == ET_JoystickAxisRel \
            || i->eventtype == ET_JoystickSliderX \
            || i->eventtype == ET_JoystickSliderY)
            return true;
    }
    return false;
#endif //0
}

float InputEngine::getEventValue(int eventID, bool pure, int valueSource)
{
    float returnValue = 0;
    std::vector<event_trigger_t> t_vec = events[eventID];
    float value = 0;
    for (std::vector<event_trigger_t>::iterator i = t_vec.begin(); i != t_vec.end(); i++)
    {
        event_trigger_t t = *i;

        if (valueSource == 0 || valueSource == 1)
        {
            switch (t.eventtype)
            {
            case ET_NONE:
                break;
            case ET_Keyboard:
                if (!keyState[t.keyCode])
                    break;

                // only use explicite mapping, if two keys with different modifiers exist, i.e. F1 and SHIFT+F1.
                // check for modificators
                if (t.explicite)
                {
                    if (t.ctrl != (keyState[KC_LCONTROL] || keyState[KC_RCONTROL]))
                        break;
                    if (t.shift != (keyState[KC_LSHIFT] || keyState[KC_RSHIFT]))
                        break;
                    if (t.alt != (keyState[KC_LMENU] || keyState[KC_RMENU]))
                        break;
                }
                else
                {
                    if (t.ctrl && !(keyState[KC_LCONTROL] || keyState[KC_RCONTROL]))
                        break;
                    if (t.shift && !(keyState[KC_LSHIFT] || keyState[KC_RSHIFT]))
                        break;
                    if (t.alt && !(keyState[KC_LMENU] || keyState[KC_RMENU]))
                        break;
                }
                value = 1;
                break;
            case ET_MouseButton:
                //if (t.mouseButtonNumber == 0)
                // TODO: FIXME
                value = mouseState.buttonDown(MB_Left);
                break;
            case ET_JoystickButton:
                {
                    if (t.joystickNumber > free_joysticks || !mJoy[t.joystickNumber])
                    {
                        value = 0;
                        continue;
                    }
                    if (t.joystickButtonNumber >= (int)mJoy[t.joystickNumber]->getNumberOfComponents(OIS_Button))
                    {
#ifndef NOOGRE
                        LOG("*** Joystick has not enough buttons for mapping: need button "+TOSTRING(t.joystickButtonNumber) + ", availabe buttons: "+TOSTRING(mJoy[t.joystickNumber]->getNumberOfComponents(OIS_Button)));
#endif
                        value = 0;
                        continue;
                    }
                    value = joyState[t.joystickNumber].mButtons[t.joystickButtonNumber];
                }
                break;
            case ET_JoystickPov:
                {
                    if (t.joystickNumber > free_joysticks || !mJoy[t.joystickNumber])
                    {
                        value = 0;
                        continue;
                    }
                    if (t.joystickPovNumber >= (int)mJoy[t.joystickNumber]->getNumberOfComponents(OIS_POV))
                    {
#ifndef NOOGRE
                        LOG("*** Joystick has not enough POVs for mapping: need POV "+TOSTRING(t.joystickPovNumber) + ", availabe POVs: "+TOSTRING(mJoy[t.joystickNumber]->getNumberOfComponents(OIS_POV)));
#endif
                        value = 0;
                        continue;
                    }
                    if (joyState[t.joystickNumber].mPOV[t.joystickPovNumber].direction & t.joystickPovDirection)
                        value = 1;
                    else
                        value = 0;
                }
                break;
            }
        }
        if (valueSource == 0 || valueSource == 2)
        {
            switch (t.eventtype)
            {
            case ET_MouseAxisX:
                value = mouseState.X.abs / 32767;
                break;
            case ET_MouseAxisY:
                value = mouseState.Y.abs / 32767;
                break;
            case ET_MouseAxisZ:
                value = mouseState.Z.abs / 32767;
                break;

            case ET_JoystickAxisRel:
            case ET_JoystickAxisAbs:
                {
                    if (t.joystickNumber > free_joysticks || !mJoy[t.joystickNumber])
                    {
                        value = 0;
                        continue;
                    }
                    if (t.joystickAxisNumber >= (int)joyState[t.joystickNumber].mAxes.size())
                    {
#ifndef NOOGRE
                        LOG("*** Joystick has not enough axis for mapping: need axe "+TOSTRING(t.joystickAxisNumber) + ", availabe axis: "+TOSTRING(joyState[t.joystickNumber].mAxes.size()));
#endif
                        value = 0;
                        continue;
                    }
                    Axis axe = joyState[t.joystickNumber].mAxes[t.joystickAxisNumber];

                    if (t.eventtype == ET_JoystickAxisRel)
                    {
                        value = (float)axe.rel / (float)mJoy[t.joystickNumber]->MAX_AXIS;
                    }
                    else
                    {
                        value = (float)axe.abs / (float)mJoy[t.joystickNumber]->MAX_AXIS;
                        switch (t.joystickAxisRegion)
                        {
                        case 0:
                            // normal case, full axis used
                            value = (value + 1) / 2;
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
                                value = 1.0 - value;
                            if (!pure)
                                value = axisLinearity(value, t.joystickAxisLinearity);
                        }
                        else
                        {
                            //LOG("not half: "+TOSTRING(value)+" / "+TOSTRING(deadZone(value, t.joystickAxisDeadzone)) +" / "+TOSTRING(t.joystickAxisDeadzone) );
                            if (t.joystickAxisReverse)
                                value = 1 - value;
                            if (!pure)
                            // no deadzone when using oure value
                                value = deadZone(value, t.joystickAxisDeadzone);
                            if (!pure)
                                value = axisLinearity(value, t.joystickAxisLinearity);
                        }
                        // digital mapping of analog axis
                        if (t.joystickAxisUseDigital)
                            if (value >= 0.5)
                                value = 1;
                            else
                                value = 0;
                    }
                }
                break;
            case ET_JoystickSliderX:
            case ET_JoystickSliderY:
                {
                    if (t.joystickNumber > free_joysticks || !mJoy[t.joystickNumber])
                    {
                        value = 0;
                        continue;
                    }
                    if (t.eventtype == ET_JoystickSliderX)
                        value = (float)joyState[t.joystickNumber].mSliders[t.joystickSliderNumber].abX / (float)mJoy[t.joystickNumber]->MAX_AXIS;
                    else if (t.eventtype == ET_JoystickSliderY)
                        value = (float)joyState[t.joystickNumber].mSliders[t.joystickSliderNumber].abY / (float)mJoy[t.joystickNumber]->MAX_AXIS;
                    value = (value + 1) / 2; // full axis
                    if (t.joystickSliderReverse)
                        value = 1.0 - value; // reversed
                }
                break;
            }
        }
        // only return if grater zero, otherwise check all other bombinations
        if (value > returnValue)
            returnValue = value;
    }
    return returnValue;
}

bool InputEngine::isKeyDown(OIS::KeyCode key)
{
    if (!mKeyboard)
        return false;
    return this->mKeyboard->isKeyDown(key);
}

bool InputEngine::isKeyDownValueBounce(OIS::KeyCode mod, float time)
{
    if (event_times[-mod] > 0)
        return false;
    else
    {
        bool res = isKeyDown(mod);
        if (res)
            event_times[-mod] = time;
        return res;
    }
}

String InputEngine::getDeviceName(event_trigger_t evt)
{
    switch (evt.eventtype)
    {
    case ET_NONE:
        return "None";
    case ET_Keyboard:
        return "Keyboard";
    case ET_MouseButton:
    case ET_MouseAxisX:
    case ET_MouseAxisY:
    case ET_MouseAxisZ:
        return "Mouse";
    case ET_JoystickButton:
    case ET_JoystickAxisAbs:
    case ET_JoystickAxisRel:
    case ET_JoystickPov:
    case ET_JoystickSliderX:
    case ET_JoystickSliderY:
        return "Joystick: " + getJoyVendor(evt.joystickNumber);
    }
    return "unknown";
}

String InputEngine::getEventTypeName(int type)
{
    switch (type)
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

void InputEngine::addEvent(int eventID, event_trigger_t t)
{
    uniqueCounter++;
    t.suid = uniqueCounter;

    if (eventID == -1)
    //unknown event, discard
        return;
    if (events.find(eventID) == events.end())
    {
        events[eventID] = std::vector<event_trigger_t>();
        events[eventID].clear();
    }
    events[eventID].push_back(t);
}

void InputEngine::updateEvent(int eventID, event_trigger_t t)
{
    if (eventID == -1)
    //unknown event, discard
        return;
    if (events.find(eventID) == events.end())
    {
        events[eventID] = std::vector<event_trigger_t>();
        events[eventID].clear();
    }
    events[eventID].push_back(t);
}

bool InputEngine::processLine(char* line, int deviceID)
{
    static String cur_comment = "";

    char eventName[256] = "", evtype[256] = "";
    const char delimiters[] = "+";
    size_t linelen = strnlen(line, 1024);
    enum eventtypes eventtype = ET_NONE;

    int joyNo = 0;
    float defaultDeadzone = 0.1f;
    float defaultLinearity = 1.0f;
    if (line[0] == ';' || linelen < 5)
    {
        cur_comment += line;;
        return false;
    }
    sscanf(line, "%s %s", eventName, evtype);
    if (strnlen(eventName, 255) == 0 || strnlen(evtype, 255) == 0)
        return false;

    if (!strncmp(evtype, "Keyboard", 8))
        eventtype = ET_Keyboard;
    else if (!strncmp(evtype, "MouseButton", 10))
        eventtype = ET_MouseButton;
    else if (!strncmp(evtype, "MouseAxisX", 9))
        eventtype = ET_MouseAxisX;
    else if (!strncmp(evtype, "MouseAxisY", 9))
        eventtype = ET_MouseAxisY;
    else if (!strncmp(evtype, "MouseAxisZ", 9))
        eventtype = ET_MouseAxisZ;
    else if (!strncmp(evtype, "JoystickButton", 14))
        eventtype = ET_JoystickButton;
    else if (!strncmp(evtype, "JoystickAxis", 12))
        eventtype = ET_JoystickAxisAbs;
    //else if (!strncmp(evtype, "JoystickAxis", 250)) eventtype = ET_JoystickAxisRel;
    else if (!strncmp(evtype, "JoystickPov", 11))
        eventtype = ET_JoystickPov;
    else if (!strncmp(evtype, "JoystickSlider", 14))
        eventtype = ET_JoystickSliderX;
    else if (!strncmp(evtype, "JoystickSliderX", 15))
        eventtype = ET_JoystickSliderX;
    else if (!strncmp(evtype, "JoystickSliderY", 15))
        eventtype = ET_JoystickSliderY;
    else if (!strncmp(evtype, "None", 4))
        eventtype = ET_NONE;

    switch (eventtype)
    {
    case ET_Keyboard:
        {
            bool alt = false;
            bool shift = false;
            bool ctrl = false;
            bool expl = false;

            char* keycode = 0;
            char keycodes[256] = {};
            char keycodes_work[256] = {};

            OIS::KeyCode key = KC_UNASSIGNED;

            sscanf(line, "%s %s %s", eventName, evtype, keycodes);
            // separate all keys and construct the key combination
            //LOG("try to add key: " + String(eventName)+","+ String(evtype)+","+String(keycodes));
            strncpy(keycodes_work, keycodes, 255);
            keycodes_work[255] = '\0';
            char* token = strtok(keycodes_work, delimiters);

            while (token != NULL)
            {
                if (strncmp(token, "SHIFT", 5) == 0)
                    shift = true;
                else if (strncmp(token, "CTRL", 4) == 0)
                    ctrl = true;
                else if (strncmp(token, "ALT", 3) == 0)
                    alt = true;
                else if (strncmp(token, "EXPL", 4) == 0)
                    expl = true;
                keycode = token;
                token = strtok(NULL, delimiters);
            }

            allit = allkeys.find(keycode);
            if (allit == allkeys.end())
            {
#ifndef NOOGRE
                LOG("unknown key: " + string(keycodes));
#endif
                key = KC_UNASSIGNED;
            }
            else
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
            event_trigger_t t_key = newEvent();
            //memset(&t_key, 0, sizeof(event_trigger_t));
            t_key.eventtype = ET_Keyboard;
            t_key.shift = shift;
            t_key.ctrl = ctrl;
            t_key.alt = alt;
            t_key.keyCode = key;
            t_key.explicite = expl;

            strncpy(t_key.configline, keycodes, 128);
            strncpy(t_key.group, getEventGroup(eventName).c_str(), 128);
            strncpy(t_key.tmp_eventname, eventName, 128);

            strncpy(t_key.comments, cur_comment.c_str(), 1024);
            addEvent(eventID, t_key);

            return true;
        }
    case ET_JoystickButton:
        {
            int buttonNo = 0;
            char tmp2[256] = {};
            sscanf(line, "%s %s %d %d %s", eventName, evtype, &joyNo, &buttonNo, tmp2);
            event_trigger_t t_joy = newEvent();
            //memset(&t_joy, 0, sizeof(event_trigger_t));
            int eventID = resolveEventName(String(eventName));
            if (eventID == -1)
                return false;
            t_joy.eventtype = ET_JoystickButton;
            t_joy.joystickNumber = (deviceID == -1 ? joyNo : deviceID);
            t_joy.joystickButtonNumber = buttonNo;
            if (!strcmp(tmp2, "!NEW!"))
            {
                strncpy(t_joy.configline, tmp2, 128);
            }
            else
            {
                char tmp[256] = {};
                sprintf(tmp, "%d", buttonNo);
                strncpy(t_joy.configline, tmp, 128);
            }
            strncpy(t_joy.group, getEventGroup(eventName).c_str(), 128);
            strncpy(t_joy.tmp_eventname, eventName, 128);
            strncpy(t_joy.comments, cur_comment.c_str(), 1024);
            cur_comment = "";
            addEvent(eventID, t_joy);
            return true;
        }
    case ET_JoystickAxisRel:
    case ET_JoystickAxisAbs:
        {
            int axisNo = 0;
            char options[256] = {};
            sscanf(line, "%s %s %d %d %s", eventName, evtype, &joyNo, &axisNo, options);
            int eventID = resolveEventName(String(eventName));
            if (eventID == -1)
                return false;

            bool half = false;
            bool reverse = false;
            bool linear = false;
            bool relative = false;
            bool usedigital = false;
            float deadzone = defaultDeadzone;
            float linearity = defaultLinearity;
            int jAxisRegion = 0;
            //  0 = all
            // -1 = lower
            //  1 = upper
            char tmp[256] = {};
            strcpy(tmp, options);
            tmp[255] = '\0';
            char* token = strtok(tmp, delimiters);
            while (token != NULL)
            {
                if (strncmp(token, "HALF", 4) == 0)
                    half = true;
                else if (strncmp(token, "REVERSE", 7) == 0)
                    reverse = true;
                else if (strncmp(token, "LINEAR", 6) == 0)
                    linear = true;
                else if (strncmp(token, "UPPER", 5) == 0)
                    jAxisRegion = 1;
                else if (strncmp(token, "LOWER", 5) == 0)
                    jAxisRegion = -1;
                else if (strncmp(token, "RELATIVE", 8) == 0)
                    relative = true;
                else if (strncmp(token, "DIGITAL", 7) == 0)
                    usedigital = true;
                else if (strncmp(token, "DEADZONE", 8) == 0 && strnlen(token, 250) > 9)
                {
                    char tmp2[256] = {};
                    strcpy(tmp2, token + 9);
                    deadzone = atof(tmp2);
                    //LOG("got deadzone: " + TOSTRING(deadzone)+", "+String(tmp2));
                }
                else if (strncmp(token, "LINEARITY", 9) == 0 && strnlen(token, 250) > 10)
                {
                    char tmp2[256] = {};
                    strcpy(tmp2, token + 10);
                    linearity = atof(tmp2);
                }
                token = strtok(NULL, delimiters);
            }

            if (relative)
                eventtype = ET_JoystickAxisRel;

            event_trigger_t t_joy = newEvent();
            //memset(&t_joy, 0, sizeof(event_trigger_t));
            t_joy.eventtype = eventtype;
            t_joy.joystickAxisRegion = jAxisRegion;
            t_joy.joystickAxisUseDigital = usedigital;
            t_joy.joystickAxisDeadzone = deadzone;
            t_joy.joystickAxisHalf = half;
            t_joy.joystickAxisLinearity = linearity;
            t_joy.joystickAxisReverse = reverse;
            t_joy.joystickAxisNumber = axisNo;
            t_joy.joystickNumber = (deviceID == -1 ? joyNo : deviceID);
            strncpy(t_joy.configline, options, 128);
            strncpy(t_joy.group, getEventGroup(eventName).c_str(), 128);
            strncpy(t_joy.tmp_eventname, eventName, 128);
            strncpy(t_joy.comments, cur_comment.c_str(), 1024);
            cur_comment = "";
            addEvent(eventID, t_joy);
            //LOG("added axis: " + TOSTRING(axisNo));
            return true;
        }
    case ET_NONE:
        {
            int eventID = resolveEventName(String(eventName));
            if (eventID == -1)
                return false;
            event_trigger_t t_none = newEvent();
            t_none.eventtype = eventtype;
            //t_none.configline = "";
            strncpy(t_none.group, getEventGroup(eventName).c_str(), 128);
            strncpy(t_none.tmp_eventname, eventName, 128);
            strncpy(t_none.comments, cur_comment.c_str(), 1024);
            cur_comment = "";
            addEvent(eventID, t_none);
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
            if (eventID == -1)
                return false;

            int direction = OIS::Pov::Centered;
            if (!strcmp(dir, "North"))
                direction = OIS::Pov::North;
            if (!strcmp(dir, "South"))
                direction = OIS::Pov::South;
            if (!strcmp(dir, "East"))
                direction = OIS::Pov::East;
            if (!strcmp(dir, "West"))
                direction = OIS::Pov::West;
            if (!strcmp(dir, "NorthEast"))
                direction = OIS::Pov::NorthEast;
            if (!strcmp(dir, "SouthEast"))
                direction = OIS::Pov::SouthEast;
            if (!strcmp(dir, "NorthWest"))
                direction = OIS::Pov::NorthWest;
            if (!strcmp(dir, "SouthWest"))
                direction = OIS::Pov::SouthWest;

            event_trigger_t t_pov = newEvent();
            t_pov.eventtype = eventtype;
            t_pov.joystickNumber = (deviceID == -1 ? joyNo : deviceID);
            t_pov.joystickPovNumber = povNumber;
            t_pov.joystickPovDirection = direction;

            strncpy(t_pov.group, getEventGroup(eventName).c_str(), 128);
            strncpy(t_pov.tmp_eventname, eventName, 128);
            strncpy(t_pov.comments, cur_comment.c_str(), 1024);
            cur_comment = "";
            addEvent(eventID, t_pov);
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
            if (eventID == -1)
                return false;

            bool reverse = false;
            char tmp[256] = {};
            strncpy(tmp, options, 255);
            tmp[255] = '\0';
            char* token = strtok(tmp, delimiters);
            while (token != NULL)
            {
                if (strncmp(token, "REVERSE", 7) == 0)
                    reverse = true;

                token = strtok(NULL, delimiters);
            }

            event_trigger_t t_slider = newEvent();

            if (type == 'Y' || type == 'y')
                eventtype = ET_JoystickSliderY;
            else if (type == 'X' || type == 'x')
                eventtype = ET_JoystickSliderX;

            t_slider.eventtype = eventtype;
            t_slider.joystickNumber = (deviceID == -1 ? joyNo : deviceID);
            t_slider.joystickSliderNumber = sliderNumber;
            t_slider.joystickSliderReverse = reverse;
            // TODO: add region support to sliders!
            t_slider.joystickSliderRegion = 0;
            strncpy(t_slider.configline, options, 128);
            strncpy(t_slider.group, getEventGroup(eventName).c_str(), 128);
            strncpy(t_slider.tmp_eventname, eventName, 128);
            strncpy(t_slider.comments, cur_comment.c_str(), 1024);
            cur_comment = "";
            addEvent(eventID, t_slider);
            //LOG("added axis: " + TOSTRING(axisNo));
            return true;
        }
    default:
        return false;
    }
}

int InputEngine::getCurrentJoyButton(int& joystickNumber, int& button)
{
    for (int j = 0; j < free_joysticks; j++)
    {
        for (int i = 0; i < (int)joyState[j].mButtons.size(); i++)
        {
            if (joyState[j].mButtons[i])
            {
                joystickNumber = j;
                button = i;
                return 1;
            }
        }
    }
    return 0;
}

int InputEngine::getCurrentPovValue(int& joystickNumber, int& pov, int& povdir)
{
    for (int j = 0; j < free_joysticks; j++)
    {
        for (int i = 0; i < MAX_JOYSTICK_POVS; i++)
        {
            if (joyState[j].mPOV[i].direction != Pov::Centered)
            {
                joystickNumber = j;
                pov = i;
                povdir = joyState[j].mPOV[i].direction;
                return 1;
            }
        }
    }
    return 0;
}

event_trigger_t InputEngine::newEvent()
{
    event_trigger_t res;
    memset(&res, 0, sizeof(event_trigger_t));
    return res;
}

int InputEngine::getJoyComponentCount(OIS::ComponentType type, int joystickNumber)
{
    if (joystickNumber > free_joysticks || !mJoy[joystickNumber])
        return 0;
    return mJoy[joystickNumber]->getNumberOfComponents(type);
}

std::string InputEngine::getJoyVendor(int joystickNumber)
{
    if (joystickNumber > free_joysticks || !mJoy[joystickNumber])
        return "unknown";
    return mJoy[joystickNumber]->vendor();
}

JoyStickState* InputEngine::getCurrentJoyState(int joystickNumber)
{
    if (joystickNumber > free_joysticks)
        return 0;
    return &joyState[joystickNumber];
}

int InputEngine::getCurrentKeyCombo(String* combo)
{
    std::map<int, bool>::iterator i;
    int keyCounter = 0;
    int modCounter = 0;

    // list all modificators first
    for (i = keyState.begin(); i != keyState.end(); i++)
    {
        if (i->second)
        {
            if (i->first != KC_LSHIFT && i->first != KC_RSHIFT && i->first != KC_LCONTROL && i->first != KC_RCONTROL && i->first != KC_LMENU && i->first != KC_RMENU)
                continue;
            modCounter++;
            String keyName = getKeyNameForKeyCode((OIS::KeyCode)i->first);
            if (*combo == "")
                *combo = keyName;
            else
                *combo = *combo + "+" + keyName;
        }
    }

    // now list all keys
    for (i = keyState.begin(); i != keyState.end(); i++)
    {
        if (i->second)
        {
            if (i->first == KC_LSHIFT || i->first == KC_RSHIFT || i->first == KC_LCONTROL || i->first == KC_RCONTROL || i->first == KC_LMENU || i->first == KC_RMENU)
                continue;
            String keyName = getKeyNameForKeyCode((OIS::KeyCode)i->first);
            if (*combo == "")
                *combo = keyName;
            else
                *combo = *combo + "+" + keyName;
            keyCounter++;
        }
    }

    //
    if (modCounter > 0 && keyCounter == 0)
    {
        return -modCounter;
    }
    else if (keyCounter == 0 && modCounter == 0)
    {
        *combo = "(Please press a key)";
        return 0;
    }
    return keyCounter;
}

String InputEngine::getEventGroup(String eventName)
{
    const char delimiters[] = "_";
    char tmp[256] = {};
    strncpy(tmp, eventName.c_str(), 255);
    tmp[255] = '\0';
    char* token = strtok(tmp, delimiters);
    while (token != NULL)
    {
        return String(token);
    }
    return "";
}

bool InputEngine::appendLineToConfig(std::string line, std::string outfile)
{
    FILE* f = fopen(const_cast<char *>(outfile.c_str()), "a");
    if (!f)
        return false;
    fprintf(f, "%s\n", line.c_str());
    fclose(f);
    return true;
}

bool InputEngine::reloadConfig(std::string outfile)
{
    events.clear();
    loadMapping(outfile);
    return true;
}

bool InputEngine::updateConfigline(event_trigger_t* t)
{
    if (t->eventtype != ET_JoystickAxisAbs && t->eventtype != ET_JoystickSliderX && t->eventtype != ET_JoystickSliderY)
        return false;
    bool isSlider = (t->eventtype == ET_JoystickSliderX || t->eventtype == ET_JoystickSliderY);
    strcpy(t->configline, "");

    if (isSlider)
    {
        if (t->joystickSliderReverse && !strlen(t->configline))
            strcat(t->configline, "REVERSE");
        else if (t->joystickSliderReverse && strlen(t->configline))
            strcat(t->configline, "+REVERSE");

        if (t->joystickSliderRegion == 1 && !strlen(t->configline))
            strcat(t->configline, "UPPER");
        else if (t->joystickSliderRegion == 1 && strlen(t->configline))
            strcat(t->configline, "+UPPER");
        else if (t->joystickSliderRegion == -1 && !strlen(t->configline))
            strcat(t->configline, "LOWER");
        else if (t->joystickSliderRegion == -1 && strlen(t->configline))
            strcat(t->configline, "+LOWER");

        // is this is a slider, ignore the rest
        return true;
    }

    if (t->joystickAxisReverse && !strlen(t->configline))
        strcat(t->configline, "REVERSE");
    else if (t->joystickAxisReverse && strlen(t->configline))
        strcat(t->configline, "+REVERSE");

    if (t->joystickAxisRegion == 1 && !strlen(t->configline))
        strcat(t->configline, "UPPER");
    else if (t->joystickAxisRegion == 1 && strlen(t->configline))
        strcat(t->configline, "+UPPER");
    else if (t->joystickAxisRegion == -1 && !strlen(t->configline))
        strcat(t->configline, "LOWER");
    else if (t->joystickAxisRegion == -1 && strlen(t->configline))
        strcat(t->configline, "+LOWER");

    if (fabs(t->joystickAxisDeadzone - 0.1) > 0.0001f)
    {
        char tmp[256] = {};
        sprintf(tmp, "DEADZONE=%0.2f", t->joystickAxisDeadzone);
        if (strlen(t->configline))
        {
            strcat(t->configline, "+");
            strcat(t->configline, tmp);
        }
        else
            strcat(t->configline, tmp);
    }
    if (fabs(1.0f - t->joystickAxisLinearity) > 0.01f)
    {
        char tmp[256] = {};
        sprintf(tmp, "LINEARITY=%0.2f", t->joystickAxisLinearity);
        if (strlen(t->configline))
        {
            strcat(t->configline, "+");
            strcat(t->configline, tmp);
        }
        else
            strcat(t->configline, tmp);
    }
    return true;
}

bool InputEngine::saveMapping(String outfile, String hwnd, int joyNum)
{
    // -10 = all
    // -2  = keyboard
    // -3  = mouse
    // >0 joystick
    FILE* f = fopen(const_cast<char *>(outfile.c_str()), "w");
    if (!f)
        return false;

    bool created = false;

    if (!mInputManager && !hwnd.empty())
    {
        destroy();
        setup(hwnd, false, false);
        created = true;
    }

    int counter = 0;
    char curGroup[128] = "";
    std::map<int, std::vector<event_trigger_t>> controls = getEvents();
    std::map<int, std::vector<event_trigger_t>>::iterator mapIt;
    std::vector<event_trigger_t>::iterator vecIt;
    for (mapIt = controls.begin(); mapIt != controls.end(); mapIt++)
    {
        std::vector<event_trigger_t> vec = mapIt->second;

        for (vecIt = vec.begin(); vecIt != vec.end(); vecIt++ , counter++)
        {
            // filters
            if (vecIt->eventtype == ET_Keyboard && joyNum != -10 && joyNum != -2)
                continue;
            if ((vecIt->eventtype == ET_MouseAxisX || vecIt->eventtype == ET_MouseAxisY || vecIt->eventtype == ET_MouseAxisZ) && joyNum != -10 && joyNum != -3)
                continue;
            if ((vecIt->eventtype == ET_JoystickAxisAbs || vecIt->eventtype == ET_JoystickAxisRel || vecIt->eventtype == ET_JoystickButton || vecIt->eventtype == ET_JoystickPov || vecIt->eventtype == ET_JoystickSliderX || vecIt->eventtype == ET_JoystickSliderY) && joyNum >= 0 && vecIt->joystickNumber != joyNum)
                continue;

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
            }
            else if (vecIt->eventtype == ET_JoystickAxisAbs || vecIt->eventtype == ET_JoystickAxisRel)
            {
                fprintf(f, "%d ", vecIt->joystickNumber);
                fprintf(f, "%d ", vecIt->joystickAxisNumber);
                fprintf(f, "%s ", vecIt->configline);
            }
            else if (vecIt->eventtype == ET_JoystickSliderX || vecIt->eventtype == ET_JoystickSliderY)
            {
                fprintf(f, "%d ", vecIt->joystickNumber);
                char type = 'X';
                if (vecIt->eventtype == ET_JoystickSliderY)
                    type = 'Y';
                fprintf(f, "%c ", type);
                fprintf(f, "%d ", vecIt->joystickSliderNumber);
                fprintf(f, "%s ", vecIt->configline);
            }
            else if (vecIt->eventtype == ET_JoystickButton)
            {
                fprintf(f, "%d ", vecIt->joystickNumber);
                fprintf(f, "%d ", vecIt->joystickButtonNumber);
            }
            else if (vecIt->eventtype == ET_JoystickPov)
            {
                const char* dirStr = "North";
                if (vecIt->joystickPovDirection == OIS::Pov::North)
                    dirStr = "North";
                if (vecIt->joystickPovDirection == OIS::Pov::South)
                    dirStr = "South";
                if (vecIt->joystickPovDirection == OIS::Pov::East)
                    dirStr = "East";
                if (vecIt->joystickPovDirection == OIS::Pov::West)
                    dirStr = "West";
                if (vecIt->joystickPovDirection == OIS::Pov::NorthEast)
                    dirStr = "NorthEast";
                if (vecIt->joystickPovDirection == OIS::Pov::SouthEast)
                    dirStr = "SouthEast";
                if (vecIt->joystickPovDirection == OIS::Pov::NorthWest)
                    dirStr = "NorthWest";
                if (vecIt->joystickPovDirection == OIS::Pov::SouthWest)
                    dirStr = "SouthWest";

                fprintf(f, "%d %d %s", vecIt->joystickNumber, vecIt->joystickPovNumber, dirStr);
            }
            // end this line
            fprintf(f, "\n");
        }
    }
    fclose(f);
    return true;
}

void InputEngine::completeMissingEvents()
{
    if (!mappingLoaded)
        return;

    for (int i = 0; i < EV_MODE_LAST; i++)
    {
        if (events.find(eventInfo[i].eventID) == events.end())
        {
            if (eventInfo[i].defaultKey.empty())
                continue;
            if (eventInfo[i].defaultKey == "None")
                continue;

            // not existing, insert default
            char tmp[256] = "";
            sprintf(tmp, "%s %s", eventInfo[i].name.c_str(), eventInfo[i].defaultKey.c_str());
#ifndef NOOGRE
            //LOG("event mapping not existing, using default: '" + String(tmp) + "'");
#endif
            processLine(tmp);
        }
    }
}

bool InputEngine::loadMapping(String outfile, bool append, int deviceID)
{
    char line[1025] = "";
    int oldState = uniqueCounter;

    if (!append)
    {
        // clear everything
        resetKeys();
        events.clear();
    }

#ifndef NOOGRE
    LOG(" * Loading input mapping " + outfile);
    {
        DataStreamPtr ds;
        try
        {
            ds = ResourceGroupManager::getSingleton().openResource(outfile, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }
        catch (...)
        {
            return false;
        }
        while (!ds->eof())
        {
            size_t size = 1024;
            if (ds->tell() + size >= ds->size())
                size = ds->size() - ds->tell();
            if (ds->tell() >= ds->size())
                break;
            size_t readnum = ds->readLine(line, size);
            if (readnum > 5)
                processLine(line, deviceID);
        }
    }
#else
    FILE *f = fopen(outfile.c_str(), "r");
    if (!f)
        return false;
    while(fgets(line, 1024, f)!=NULL)
    {
        if (strnlen(line, 1024) > 5)
            processLine(line);
    }
    fclose(f);

#endif

    int newEvents = uniqueCounter - oldState;
#ifndef NOOGRE
    LOG(" * Input map successfully loaded: " + TOSTRING(newEvents) + " entries");
#endif
    return true;
}

int InputEngine::resolveEventName(String eventName)
{
    int i = 0;
    while (i != EV_MODE_LAST)
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
    int i = 0;
    while (i != EV_MODE_LAST)
    {
        if (eventInfo[i].eventID == eventID)
            return eventInfo[i].name;
        i++;
    }
    return "unknown";
}

void InputEngine::initAllKeys()
{
    allkeys["0"] = KC_0;
    allkeys["1"] = KC_1;
    allkeys["2"] = KC_2;
    allkeys["3"] = KC_3;
    allkeys["4"] = KC_4;
    allkeys["5"] = KC_5;
    allkeys["6"] = KC_6;
    allkeys["7"] = KC_7;
    allkeys["8"] = KC_8;
    allkeys["9"] = KC_9;
    allkeys["A"] = KC_A;
    allkeys["ABNT_C1"] = KC_ABNT_C1;
    allkeys["ABNT_C2"] = KC_ABNT_C2;
    allkeys["ADD"] = KC_ADD;
    allkeys["APOSTROPHE"] = KC_APOSTROPHE;
    allkeys["APPS"] = KC_APPS;
    allkeys["AT"] = KC_AT;
    allkeys["AX"] = KC_AX;
    allkeys["B"] = KC_B;
    allkeys["BACK"] = KC_BACK;
    allkeys["BACKSLASH"] = KC_BACKSLASH;
    allkeys["C"] = KC_C;
    allkeys["CALCULATOR"] = KC_CALCULATOR;
    allkeys["CAPITAL"] = KC_CAPITAL;
    allkeys["COLON"] = KC_COLON;
    allkeys["COMMA"] = KC_COMMA;
    allkeys["CONVERT"] = KC_CONVERT;
    allkeys["D"] = KC_D;
    allkeys["DECIMAL"] = KC_DECIMAL;
    allkeys["DELETE"] = KC_DELETE;
    allkeys["DIVIDE"] = KC_DIVIDE;
    allkeys["DOWN"] = KC_DOWN;
    allkeys["E"] = KC_E;
    allkeys["END"] = KC_END;
    allkeys["EQUALS"] = KC_EQUALS;
    allkeys["ESCAPE"] = KC_ESCAPE;
    allkeys["F"] = KC_F;
    allkeys["F1"] = KC_F1;
    allkeys["F10"] = KC_F10;
    allkeys["F11"] = KC_F11;
    allkeys["F12"] = KC_F12;
    allkeys["F13"] = KC_F13;
    allkeys["F14"] = KC_F14;
    allkeys["F15"] = KC_F15;
    allkeys["F2"] = KC_F2;
    allkeys["F3"] = KC_F3;
    allkeys["F4"] = KC_F4;
    allkeys["F5"] = KC_F5;
    allkeys["F6"] = KC_F6;
    allkeys["F7"] = KC_F7;
    allkeys["F8"] = KC_F8;
    allkeys["F9"] = KC_F9;
    allkeys["G"] = KC_G;
    allkeys["GRAVE"] = KC_GRAVE;
    allkeys["H"] = KC_H;
    allkeys["HOME"] = KC_HOME;
    allkeys["I"] = KC_I;
    allkeys["INSERT"] = KC_INSERT;
    allkeys["J"] = KC_J;
    allkeys["K"] = KC_K;
    allkeys["KANA"] = KC_KANA;
    allkeys["KANJI"] = KC_KANJI;
    allkeys["L"] = KC_L;
    allkeys["LBRACKET"] = KC_LBRACKET;
    allkeys["LCONTROL"] = KC_LCONTROL;
    allkeys["LEFT"] = KC_LEFT;
    allkeys["LMENU"] = KC_LMENU;
    allkeys["LSHIFT"] = KC_LSHIFT;
    allkeys["LWIN"] = KC_LWIN;
    allkeys["M"] = KC_M;
    allkeys["MAIL"] = KC_MAIL;
    allkeys["MEDIASELECT"] = KC_MEDIASELECT;
    allkeys["MEDIASTOP"] = KC_MEDIASTOP;
    allkeys["MINUS"] = KC_MINUS;
    allkeys["MULTIPLY"] = KC_MULTIPLY;
    allkeys["MUTE"] = KC_MUTE;
    allkeys["MYCOMPUTER"] = KC_MYCOMPUTER;
    allkeys["N"] = KC_N;
    allkeys["NEXTTRACK"] = KC_NEXTTRACK;
    allkeys["NOCONVERT"] = KC_NOCONVERT;
    allkeys["NUMLOCK"] = KC_NUMLOCK;
    allkeys["NUMPAD0"] = KC_NUMPAD0;
    allkeys["NUMPAD1"] = KC_NUMPAD1;
    allkeys["NUMPAD2"] = KC_NUMPAD2;
    allkeys["NUMPAD3"] = KC_NUMPAD3;
    allkeys["NUMPAD4"] = KC_NUMPAD4;
    allkeys["NUMPAD5"] = KC_NUMPAD5;
    allkeys["NUMPAD6"] = KC_NUMPAD6;
    allkeys["NUMPAD7"] = KC_NUMPAD7;
    allkeys["NUMPAD8"] = KC_NUMPAD8;
    allkeys["NUMPAD9"] = KC_NUMPAD9;
    allkeys["NUMPADCOMMA"] = KC_NUMPADCOMMA;
    allkeys["NUMPADENTER"] = KC_NUMPADENTER;
    allkeys["NUMPADEQUALS"] = KC_NUMPADEQUALS;
    allkeys["O"] = KC_O;
    allkeys["OEM_102"] = KC_OEM_102;
    allkeys["P"] = KC_P;
    allkeys["PAUSE"] = KC_PAUSE;
    allkeys["PERIOD"] = KC_PERIOD;
    allkeys["PGDOWN"] = KC_PGDOWN;
    allkeys["PGUP"] = KC_PGUP;
    allkeys["PLAYPAUSE"] = KC_PLAYPAUSE;
    allkeys["POWER"] = KC_POWER;
    allkeys["PREVTRACK"] = KC_PREVTRACK;
    allkeys["Q"] = KC_Q;
    allkeys["R"] = KC_R;
    allkeys["RBRACKET"] = KC_RBRACKET;
    allkeys["RCONTROL"] = KC_RCONTROL;
    allkeys["RETURN"] = KC_RETURN;
    allkeys["RIGHT"] = KC_RIGHT;
    allkeys["RMENU"] = KC_RMENU;
    allkeys["RSHIFT"] = KC_RSHIFT;
    allkeys["RWIN"] = KC_RWIN;
    allkeys["S"] = KC_S;
    allkeys["SCROLL"] = KC_SCROLL;
    allkeys["SEMICOLON"] = KC_SEMICOLON;
    allkeys["SLASH"] = KC_SLASH;
    allkeys["SLEEP"] = KC_SLEEP;
    allkeys["SPACE"] = KC_SPACE;
    allkeys["STOP"] = KC_STOP;
    allkeys["SUBTRACT"] = KC_SUBTRACT;
    allkeys["SYSRQ"] = KC_SYSRQ;
    allkeys["T"] = KC_T;
    allkeys["TAB"] = KC_TAB;
    allkeys["U"] = KC_U;
    //allkeys["UNASSIGNED"] = KC_UNASSIGNED;
    allkeys["UNDERLINE"] = KC_UNDERLINE;
    allkeys["UNLABELED"] = KC_UNLABELED;
    allkeys["UP"] = KC_UP;
    allkeys["V"] = KC_V;
    allkeys["VOLUMEDOWN"] = KC_VOLUMEDOWN;
    allkeys["VOLUMEUP"] = KC_VOLUMEUP;
    allkeys["W"] = KC_W;
    allkeys["WAKE"] = KC_WAKE;
    allkeys["WEBBACK"] = KC_WEBBACK;
    allkeys["WEBFAVORITES"] = KC_WEBFAVORITES;
    allkeys["WEBFORWARD"] = KC_WEBFORWARD;
    allkeys["WEBHOME"] = KC_WEBHOME;
    allkeys["WEBREFRESH"] = KC_WEBREFRESH;
    allkeys["WEBSEARCH"] = KC_WEBSEARCH;
    allkeys["WEBSTOP"] = KC_WEBSTOP;
    allkeys["X"] = KC_X;
    allkeys["Y"] = KC_Y;
    allkeys["YEN"] = KC_YEN;
    allkeys["Z"] = KC_Z;
}

void InputEngine::setupDefault(Ogre::String inputhwnd /* = "" */)
{
    // start input engine
    size_t hWnd = 0;
    RoR::App::GetOgreSubsystem()->GetRenderWindow()->getCustomAttribute("WINDOW", &hWnd);

    this->setup(TOSTRING(hWnd), true, true);
}

String InputEngine::getKeyForCommand(int eventID)
{
    std::map<int, std::vector<event_trigger_t>>::iterator it = events.find(eventID);

    if (it == events.end())
        return String();
    if (it->second.empty())
        return String();

    std::vector<event_trigger_t>::iterator it2 = it->second.begin();
    return getKeyNameForKeyCode((OIS::KeyCode)it2->keyCode);
}
