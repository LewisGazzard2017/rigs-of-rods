
// External interface - called on startup
bool SimPrepare()
{
    LogMessage("Running SimPrepare() ...");
    return true;
}

// External interface 
bool SimUpdate(SimContext@ ctx, uint dt_milis)
{
    LogMessage("Running SimUpdate() ...");
    if (ctx.HasKbChanged())
    {
        // ... process keyboard
    }
    
    return true;
}

// External interface - called on exit
void SimCleanup()
{
    LogMessage("Running SimCleanup() ...");
}



