
#include "OisInputDefs.as"

enum ControlEvent
{
    CONTROL_INVALID,
    
    CONTROL_GENERAL_EXIT_TO_MENU,
    
    CONTROL_FREECAM_MOVE_FORWARD,
    CONTROL_FREECAM_MOVE_BACKWARD,
    CONTROL_FREECAM_MOVE_LEFT,
    CONTROL_FREECAM_MOVE_RIGHT,
    CONTROL_FREECAM_MOVE_UP,
    CONTROL_FREECAM_MOVE_DOWN
};

class ControlInfo
{
    ControlInfo()
    {
        ResetDef();
        ResetState();
    }
    
    void ResetDef()
    {
        keyb_code    = 0;
        mouse_code   = 0;
        mod_shift    = false;
        mod_alt      = false;
        mod_ctrl     = false;
        mod_mouse_l  = false;
        mod_mouse_r  = false;
        mod_mouse_m  = false;
    }
    
    void ResetState()
    {
        is_active    = false;
        is_fresh_on  = false;
        is_fresh_off = false;
    }

    int  keyb_code;  ///< 0 = Unused
    int  mouse_code; ///< 0 = Unused
    bool mod_shift;
    bool mod_alt;
    bool mod_ctrl;
    bool mod_mouse_l;
    bool mod_mouse_r;
    bool mod_mouse_m;
    
    // State (current frame)
    bool is_active;
    bool is_fresh_on;
    bool is_fresh_off;
};

class InputHandler
{
    InputHandler() // Sets up all events. TODO: make configurable...
    {
        ControlInfo control;
        // CONTROL_INVALID - skipped
        
        // CONTROL_GENERAL_EXIT_TO_MENU
        control.keyb_code = KC_ESCAPE;
        m_controls.insertLast(control);
        
        // CONTROL_FREECAM_MOVE_FORWARD
        control.keyb_code = KC_W;
        m_controls.insertLast(control);        
        
        // CONTROL_FREECAM_MOVE_BACKWARD
        control.keyb_code = KC_S;
        m_controls.insertLast(control);
        
        // CONTROL_FREECAM_MOVE_LEFT
        control.keyb_code = KC_A;
        m_controls.insertLast(control);
        
        // CONTROL_FREECAM_MOVE_RIGHT
        control.keyb_code = KC_D;
        m_controls.insertLast(control);
        
        // CONTROL_FREECAM_MOVE_UP
        control.ResetDef();
        control.keyb_code = KC_W;
        control.mod_mouse_r = true;
        m_controls.insertLast(control);
        
        // CONTROL_FREECAM_MOVE_DOWN
        control.ResetDef();
        control.keyb_code = KC_S;
        control.mod_mouse_r = true;
        m_controls.insertLast(control);              
    }
    
    void UpdateControls(SimContext@ ctx)
    {
        if (!ctx.HasKbChanged())
            return;
    
        bool shift   = ctx.IsKeyDown(KC_RSHIFT)   || ctx.IsKeyDown(KC_LSHIFT);
        bool ctrl    = ctx.IsKeyDown(KC_RCONTROL) || ctx.IsKeyDown(KC_LCONTROL);
        bool alt     = ctx.IsKeyDown(KC_RMENU)    || ctx.IsKeyDown(KC_LMENU);
        
        for (uint i = 0; i < m_controls.length(); ++i)
        {
            ControlInfo c = m_controls[i]; // Copy out
            c.is_fresh_on = false;
            c.is_fresh_off = false;
            
            bool kb_mods_match = (c.mod_shift == shift) && (c.mod_alt == alt) && (c.mod_ctrl == ctrl);
            if (c.is_active)
            {
                if (!kb_mods_match || !ctx.IsKeyDown(c.keyb_code))
                {
                    c.is_active = false;
                    c.is_fresh_off = true;
                }
            }
            else
            {
                if (kb_mods_match && ctx.IsKeyDown(c.keyb_code))
                {
                    c.is_active = true;
                    c.is_fresh_on = true;
                }
            }
            
            m_controls[i] = c; // Copy back
        }
    }
    
    void ResetControlStates()
    {
        for (uint i = 0; i < m_controls.length(); ++i)
        {
            m_controls[i].ResetState();
        }
    }
    
    ControlInfo GetControl(ControlEvent control)
    {
        return m_controls[control - 1];                   
    }
    
    private array<ControlInfo> m_controls; // Indexed by `enum ControlEvent` minus 1
}