
#include "SimControls.as"

InputHandler input_handler;

// External interface - called on startup
bool SimPrepare(SimContext@ ctx)
{
    LogMessage("Running SimPrepare() ...");
    
    ctx.SetCameraPosition(-50,40,-50);
    ctx.CameraLookAt(0,0,0);
    
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



