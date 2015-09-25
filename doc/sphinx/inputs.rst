:mod:`inputs` --- Input handling and event/mode updates
=======================================================

.. module:: inputs
   :synopsis: Input handling and event/mode updates.

.. moduleauthor:: Petr Ohlidal <_myname_@_mysurname_.cz>

Provides comfortable API for dealing with user input.

It works closely with the abstract Event/Mode system introduced by module
:mod:`application`. It enables binding :class:`application.Event` (action: fire) 
and :class:`application.Mode` (actions: enter, exit) to input actions. 

Rationale:

* The programmer should ask "did the user press a 'grab' hotkey?" rather
  than "did the user press 'G' key?". This enables clean configuration.
* The same event may be invoked by both input device (hotkey...) and
  different way (action menu...). So the best thing
  for the programmer to ask is "was 'grab mode' invoked?". 

Input management
----------------

.. class:: InputMapping

   Enables mapping an input event (key press, mouse button press) to
   application.Event
   
   Instances of this class are automatically created by InputHandler for
   any key or button you want to bind to. Each input device (key/button)
   gets one InputMapping object.

   .. attribute:: MODE_ACTIVATE
   
   Constant. Mode operation: activate.
   
   .. attribute:: MODE_DEACTIVATE
   
   Constant. Mode operation: deactivate.
   
   .. attribute:: MODE_DO_NOTHING
   
   Constant. Mode operation: no-operation.
        
   .. method:: set_mode(mode, mode_operation)
   
      Assign an :class:`application.Mode` operation to this binding. You must
      specify an operation which should be performed on the application.Mode.
        
   .. method:: set_event(self, event):
      
      Assign an :class:`application.Event` operation to this binding. 
      The application.Event will be fired when respective input event occurs.
      

.. class:: InputListener()

   Translate input key/mouse/other events into editor events
   
   .. attribute:: key_down_map
   
      Lookup dictionary for "key pressed" events
      (KeyCode => :class:`InputMapping`)
   
   .. attribute:: key_up_map
   
      Lookup dictionary for "key released" events 
      (KeyCode => :class:`InputMapping`)
   
   .. attribute:: mouse_down_map
   
      Lookup dictionary for "mouse button pressed" events
      (MouseButton => :class:`InputMapping`)
   
   .. attribute:: mouse_up_map
   
      Lookup dictionary for "mouse button released" events
      (MouseButton => :class:`InputMapping`)
   
   .. attribute:: mouse_state
   
      Instance of :class:`MouseState`
   
   .. method:: reset_inputs()
        
   .. method:: add_key_down_event_mapping(key_code, event)
   
      Maps application.Event to OIS key code (key press event)
    
   .. method:: add_key_up_event_mapping(key_code, event)
    
      Maps application.Event to OIS key code (key release event)
        
   .. method:: add_key_down_mode_mapping(key_code, mode, mode_op)
   
      Maps application.Event to OIS key code (key press event).
      You must specify the operation with Mode (enter or exit?).
        
   .. method:: add_key_up_mode_mapping(key_code, mode, mode_op)
   
      Maps application.Event to OIS key code (key release event)
      You must specify the operation with Mode (enter or exit?).
        
   .. method:: add_mouse_down_event_mapping(button_code, event)
   
      Maps application.Event to OIS mouse-button code (button press event)
    
   .. method:: add_mouse_up_event_mapping(button_code, event)
   
      Maps application.Event to OIS mouse-button code (button release event)
        
   .. method:: add_mouse_down_mode_mapping(button_code, mode, mode_op)
   
      Maps application.Mode to OIS mouse-button code (button press event)
      You must specify the operation with Mode (enter or exit?).
        
   .. method:: add_mouse_up_mode_mapping(button_code, mode, mode_op)
   
      Maps application.Mode to OIS mouse-button code (button release event)
      You must specify the operation with Mode (enter or exit?).
        
   **Utilities**
        
   .. method:: _add_event_mapping(table, input_code, event)
        
   .. method:: _add_mode_mapping(table, key_code, mode, mode_op)
        
   .. method:: _check_mapping(table, input_code)
            
   .. method:: _handle_callback(table, input_code)
    
   **Input callbacks**
    
   These implement input listener interface defined 
   by :func:`ror_system.register_input_listener`        
        
   .. method:: key_pressed_callback(key_code, key_value)
                
   .. method:: key_released_callback(key_code, key_value)
    
   .. method:: mouse_pressed_callback(button_code, x_abs, y_abs)
        
   .. method:: mouse_released_callback(button_code, x_abs, y_abs)
    
   .. method:: mouse_moved_or_scrolled_callback(x_abs, y_abs, x_rel, y_rel, z_rel)
        
Underlying device handling
--------------------------        
      
.. class:: class KeyCodes()

   Maps to OIS key codes. Copied from OIS library header. Copyright (c) 2005-2007 www.wreckedgames.com::
    
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



.. class:: MouseButtons:
    
   Maps to OIS button codes. Copied from OIS library header. Copyright (c) 2005-2007 www.wreckedgames.com::
    
    LEFT     = 0
    RIGHT    = 1
    MIDDLE   = 2
    BUTTON_3 = 4
    BUTTON_4 = 5
    BUTTON_5 = 6
    BUTTON_6 = 7
    BUTTON_7 = 8
    
    NUM_BUTTONS = 9

    
    
.. class:: MouseState()
    
   .. method:: reset
   
      Resets relative mouse moves (absolute positions are retained) 
      and button press/relase events (states are retained)
               
   .. method:: update(x_abs, y_abs, x_rel, y_rel, z_rel)
    
      Injects current mouse state. 
        
   .. method:: button_pressed(button_code)
      
      Inject mouse button press event
        
   .. method:: button_released(button_code)
      
      Injects mouse release event
        
   .. method:: is_left_button_pressed()
   
      Informs about button state
        
   .. method:: is_right_button_pressed()
   
      Informs about button state
        
   .. method:: is_middle_button_pressed()
   
      Informs about button state
    
                






        
