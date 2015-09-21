'''
The key/mouse button codes used here were copied from OIS library header.

--------------------------------- OIS LICENSE ----------------------------------
The zlib/libpng License

Copyright (c) 2005-2007 Phillip Castaneda (pjcast -- www.wreckedgames.com)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that
		you wrote the original software. If you use this software in a product,
		an acknowledgment in the product documentation would be appreciated but is
		not required.

    2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
------------------------------- END OIS LICENSE --------------------------------    
'''

class KeyCodes:
    'Maps to OIS key codes. Copied from OIS library header. Copyright (c) 2005-2007 www.wreckedgames.com'
    UNASSIGNED  = 0x00
    ESCAPE      = 0x01
    KEYPAD1     = 0x02
    KEYPAD2     = 0x03
    KEYPAD3     = 0x04
    KEYPAD4     = 0x05
    KEYPAD5     = 0x06
    KEYPAD6     = 0x07
    KEYPAD7     = 0x08
    KEYPAD8     = 0x09
    KEYPAD9     = 0x0A
    KEYPAD0     = 0x0B
    MINUS       = 0x0C    # - on main keyboard
    EQUALS      = 0x0D
    BACK        = 0x0E    # backspace
    TAB         = 0x0F
    Q           = 0x10
    W           = 0x11
    E           = 0x12
    R           = 0x13
    T           = 0x14
    Y           = 0x15
    U           = 0x16
    I           = 0x17
    O           = 0x18
    P           = 0x19
    LBRACKET    = 0x1A
    RBRACKET    = 0x1B
    RETURN      = 0x1C    # Enter on main keyboard
    LCONTROL    = 0x1D
    A           = 0x1E
    S           = 0x1F
    D           = 0x20
    F           = 0x21
    G           = 0x22
    H           = 0x23
    J           = 0x24
    K           = 0x25
    L           = 0x26
    SEMICOLON   = 0x27
    APOSTROPHE  = 0x28
    GRAVE       = 0x29    # accent
    LSHIFT      = 0x2A
    BACKSLASH   = 0x2B
    Z           = 0x2C
    X           = 0x2D
    C           = 0x2E
    V           = 0x2F
    B           = 0x30
    N           = 0x31
    M           = 0x32
    COMMA       = 0x33
    PERIOD      = 0x34    # . on main keyboard
    SLASH       = 0x35    # / on main keyboard
    RSHIFT      = 0x36
    MULTIPLY    = 0x37    # * on numeric keypad
    LMENU       = 0x38    # left Alt
    SPACE       = 0x39
    CAPITAL     = 0x3A
    F1          = 0x3B
    F2          = 0x3C
    F3          = 0x3D
    F4          = 0x3E
    F5          = 0x3F
    F6          = 0x40
    F7          = 0x41
    F8          = 0x42
    F9          = 0x43
    F10         = 0x44
    NUMLOCK     = 0x45
    SCROLL      = 0x46    # Scroll Lock
    NUMPAD7     = 0x47
    NUMPAD8     = 0x48
    NUMPAD9     = 0x49
    SUBTRACT    = 0x4A    # - on numeric keypad
    NUMPAD4     = 0x4B
    NUMPAD5     = 0x4C
    NUMPAD6     = 0x4D
    ADD         = 0x4E    # + on numeric keypad
    NUMPAD1     = 0x4F
    NUMPAD2     = 0x50
    NUMPAD3     = 0x51
    NUMPAD0     = 0x52
    DECIMAL     = 0x53    # . on numeric keypad
    OEM_102     = 0x56    # < > | on UK/Germany keyboards
    F11         = 0x57
    F12         = 0x58
    F13         = 0x64    #                     (NEC PC98)
    F14         = 0x65    #                     (NEC PC98)
    F15         = 0x66    #                     (NEC PC98)
    KANA        = 0x70    # (Japanese keyboard)
    ABNT_C1     = 0x73    # / ? on Portugese (Brazilian) keyboards
    CONVERT     = 0x79    # (Japanese keyboard)
    NOCONVERT   = 0x7B    # (Japanese keyboard)
    YEN         = 0x7D    # (Japanese keyboard)
    ABNT_C2     = 0x7E    # Numpad . on Portugese (Brazilian) keyboards
    NUMPADEQUALS= 0x8D    # = on numeric keypad (NEC PC98)
    PREVTRACK   = 0x90    # Previous Track (CIRCUMFLEX on Japanese keyboard)
    AT          = 0x91    #                     (NEC PC98)
    COLON       = 0x92    #                     (NEC PC98)
    UNDERLINE   = 0x93    #                     (NEC PC98)
    KANJI       = 0x94    # (Japanese keyboard)
    STOP        = 0x95    #                     (NEC PC98)
    AX          = 0x96    #                     (Japan AX)
    UNLABELED   = 0x97    #                        (J3100)
    NEXTTRACK   = 0x99    # Next Track
    NUMPADENTER = 0x9C    # Enter on numeric keypad
    RCONTROL    = 0x9D
    MUTE        = 0xA0    # Mute
    CALCULATOR  = 0xA1    # Calculator
    PLAYPAUSE   = 0xA2    # Play / Pause
    MEDIASTOP   = 0xA4    # Media Stop
    VOLUMEDOWN  = 0xAE    # Volume -
    VOLUMEUP    = 0xB0    # Volume +
    WEBHOME     = 0xB2    # Web home
    NUMPADCOMMA = 0xB3    # , on numeric keypad (NEC PC98)
    DIVIDE      = 0xB5    # / on numeric keypad
    SYSRQ       = 0xB7
    RMENU       = 0xB8    # right Alt
    PAUSE       = 0xC5    # Pause
    HOME        = 0xC7    # Home on arrow keypad
    UP          = 0xC8    # UpArrow on arrow keypad
    PGUP        = 0xC9    # PgUp on arrow keypad
    LEFT        = 0xCB    # LeftArrow on arrow keypad
    RIGHT       = 0xCD    # RightArrow on arrow keypad
    END         = 0xCF    # End on arrow keypad
    DOWN        = 0xD0    # DownArrow on arrow keypad
    PGDOWN      = 0xD1    # PgDn on arrow keypad
    INSERT      = 0xD2    # Insert on arrow keypad
    DELETE      = 0xD3    # Delete on arrow keypad
    LWIN        = 0xDB    # Left Windows key
    RWIN        = 0xDC    # Right Windows key
    APPS        = 0xDD    # AppMenu key
    POWER       = 0xDE    # System Power
    SLEEP       = 0xDF    # System Sleep
    WAKE        = 0xE3    # System Wake
    WEBSEARCH   = 0xE5    # Web Search
    WEBFAVORITES= 0xE6    # Web Favorites
    WEBREFRESH  = 0xE7    # Web Refresh
    WEBSTOP     = 0xE8    # Web Stop
    WEBFORWARD  = 0xE9    # Web Forward
    WEBBACK     = 0xEA    # Web Back
    MYCOMPUTER  = 0xEB    # My Computer
    MAIL        = 0xEC    # Mail
    MEDIASELECT = 0xED    # Media Select



class MouseButtons:
    'Maps to OIS button codes. Copied from OIS library header. Copyright (c) 2005-2007 www.wreckedgames.com'
    LEFT     = 0
    RIGHT    = 1
    MIDDLE   = 2
    BUTTON_3 = 4
    BUTTON_4 = 5
    BUTTON_5 = 6
    BUTTON_6 = 7
    BUTTON_7 = 8
    
    NUM_BUTTONS = 9

    
    
class MouseState():
    def __init__(self):
        self.absolute_x = 0
        self.absolute_y = 0
        self.relative_x = 0
        self.relative_y = 0
        self.relative_z = 0
        self.button_pressed_states = [
            False, False, False, False, False, False, False, False, False ]
        self.button_down_events = [
            False, False, False, False, False, False, False, False, False ]
        self.button_up_events = [
            False, False, False, False, False, False, False, False, False ]
    
    def reset(self):
        self.relative_x = 0
        self.relative_y = 0
        self.relative_z = 0
        for i in range(0, MouseButtons.NUM_BUTTONS-1):
            self.button_down_events[i] = False
            self.button_up_events[i] = False
               
    def update(self, x_abs, y_abs, x_rel, y_rel, z_rel):
        self.absolute_x = x_abs
        self.absolute_y = y_abs
        self.relative_x += x_rel
        self.relative_y += y_rel
        self.relative_z += z_rel
        
    def button_pressed(self, button_code):
        self.button_pressed_states[button_code] = True
        self.button_down_events[button_code] = True
        
    def button_released(self, button_code):
        self.button_pressed_states[button_code] = False
        self.button_up_events[button_code] = True
        
    def is_left_button_pressed(self):
        return self.button_pressed_states[MouseButtons.LEFT]
        
    def is_right_button_pressed(self):
        return self.button_pressed_states[MouseButtons.RIGHT]
        
    def is_middle_button_pressed(self):
        return self.button_pressed_states[MouseButtons.MIDDLE]
    
                

class InputMapping:
    MODE_ACTIVATE = 1
    MODE_DEACTIVATE = -1
    MODE_DO_NOTHING = 0

    def __init__(self):
        self.mode = None
        self.event = None
        self.mode_operation = InputMapping.MODE_DO_NOTHING
        
    def set_mode(self, mode, mode_operation):
        self.mode = mode
        self.mode_operation = mode_operation
        
    def set_event(self, event):
        self.event = event



class InputListener:
    'Translate input key/mouse/other events into editor events'
    
    def __init__(self):
        self.key_down_map = {}
        self.key_up_map = {}
        self.mouse_down_map = {}
        self.mouse_up_map = {}
        self.mouse_state = MouseState()
        
    def reset_inputs(self):
        self.mouse_state.reset()
        
    def add_key_down_event_mapping(self, key_code, event):
        InputListener._add_event_mapping(self.key_down_map, key_code, event)
    
    def add_key_up_event_mapping(self, key_code, event):
        InputListener._add_event_mapping(self.key_up_map, key_code, event)
        
    def add_key_down_mode_mapping(self, key_code, mode, mode_op):
        InputListener._add_mode_mapping(self.key_down_map, key_code, mode, mode_op)
        
    def add_key_up_mode_mapping(self, key_code, mode, mode_op):
        InputListener._add_mode_mapping(self.key_down_map, key_code, mode, mode_op)
        
    def add_mouse_down_event_mapping(self, button_code, event):
        InputListener._add_event_mapping(self.mouse_down_map, button_code, event)
    
    def add_mouse_up_event_mapping(self, button_code, event):
        InputListener._add_event_mapping(self.mouse_up_map, button_code, event)
        
    def add_mouse_down_mode_mapping(self, button_code, mode, mode_op):
        InputListener._add_mode_mapping(self.mouse_down_map, button_code, mode, mode_op)
        
    def add_mouse_up_mode_mapping(self, button_code, mode, mode_op):
        InputListener._add_mode_mapping(self.mouse_down_map, button_code, mode, mode_op)
        
    # -- Utils --
        
    def _add_event_mapping(table, input_code, event):
        InputListener._check_mapping(table, input_code)
        table[input_code].set_event(event)
        
    def _add_mode_mapping(table, key_code, mode, mode_op):
        InputListener._check_mapping(table, input_code)
        table[input_code].set_mode(mode, mode_op)
        
    def _check_mapping(table, input_code):
        if input_code not in table:
            table[input_code] = InputMapping()
            
    def _handle_callback(table, input_code):
        if input_code in table:
            mapping = table[input_code]
            if (mapping.event is not None):
                event.was_fired = True
            if mapping.mode is not None:
                if mapping.mode_operation is InputMapping.MODE_ACTIVATE:
                    mapping.mode.activate()
                elif mapping.mode_operation is InputMapping.MODE_DEACTIVATE:
                    mapping.mode.deactivate()
    
    # --------------------------------------------------------------------------
    # Keyboard callbacks
        
    def key_pressed_callback(self, key_code, key_value):
        InputListener._handle_callback(self.key_down_map, key_code)
                
    def key_released_callback(self, key_code, key_value):
        InputListener._handle_callback(self.key_up_map, key_code)
        
    # --------------------------------------------------------------------------
    # Mouse callbacks
    
    def mouse_pressed_callback(self, button_code, x_abs, y_abs):
        self.mouse_state.update(x_abs, y_abs, 0, 0, 0)
        self.mouse_state.button_pressed(button_code)
        InputListener._handle_callback(self.mouse_down_map, button_code)
        
    def mouse_released_callback(self, button_code, x_abs, y_abs):
        self.mouse_state.update(x_abs, y_abs, 0, 0, 0)
        self.mouse_state.button_released(button_code)
        InputListener._handle_callback(self.mouse_up_map, button_code)
    
    def mouse_moved_or_scrolled_callback(self, x_abs, y_abs, x_rel, y_rel, z_rel):
        self.mouse_state.update(x_abs, y_abs, x_rel, y_rel, z_rel)
        
        
"""    # Only mouse buttons can have events    
    def add_mouse_event_mapping(self, mouse_button_code, event):
        if self.mouse_button_map[mouse_button_code] == None:
            self.mouse_button_map[mouse_button_code] = InputMapping()
        self.mouse_button_map[mouse_button_code].set_event(event)
    
    # Only mouse buttons can have modes    
    def add_mouse_mode_mapping(self, mouse_button_code, mode, mode_op):
        if self.mouse_button_map[mouse_button_code] == None:
            self.mouse_button_map[mouse_button_code] = InputMapping()
        self.mouse_button_map[mouse_button_code].set_mode(mode, mode_op) 
"""        
"""    def _add_mouse_event_mapping(mousemap, btn_code, event):
        InputListener._check_mouse_mapping(mousemap, btn_code)
        mousemap[btn_code].set_event(event)
        
    def _add_mouse_event_mapping(mousemap, btn_code, mode, mode_op):
        InputListener._check_mouse_mapping(mousemap, btn_code)
        mousemap[btn_code].set_mode(mode, mode_op)
            
    def _check_mouse_mapping(mousemap, mouse_button_code):
        if mousemap[mouse_button_code] == None:
            mousemap[mouse_button_code] = InputMapping() 
"""

        