
#include "SimControls.as"

InputHandler input_handler;

// External interface - called on startup
bool SimPrepare(SimContext@ ctx)
{
    LogMessage("Running SimPrepare() ...");
    
    // Initial camera pos
    ctx.SetCameraPosition(Vector3(-50,40,-50));
    
    // Initial camera rotation
    Vector3 rot_axis(1,0,0); // global X
    Radian rot_angle(3.14 * -0.1); // slight tilt
    ctx.SetCameraOrientation(Quaternion(rot_angle, rot_axis));
    
    input_handler.ResetControlStates();
    
    return true;
}

// External interface 
bool SimUpdate(SimContext@ ctx, uint dt_milis)
{
    input_handler.UpdateControls(ctx);
    
    if (input_handler.GetControl(CONTROL_GENERAL_EXIT_TO_MENU).is_active)
    {
        LogMessage("SimUpdate(): Returning to main menu");
        ctx.Quit(); // Request exit
    }
    
    return true;
}

// External interface - called on exit
void SimCleanup()
{
    LogMessage("Running SimCleanup() ...");
}



