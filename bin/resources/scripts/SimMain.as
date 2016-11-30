
#include "OisInputDefs.as"

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
    if (ctx.HasKbChanged())
    {
        if (ctx.WasKeyPressed(KC_ESCAPE))
        {
            LogMessage("SimUpdate(): Escape key pressed, leaving simulation");
            ctx.Quit();
        }
    }
    
    return true;
}

// External interface - called on exit
void SimCleanup()
{
    LogMessage("Running SimCleanup() ...");
}



