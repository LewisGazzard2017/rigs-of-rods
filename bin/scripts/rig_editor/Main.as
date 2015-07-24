// Rig editor main script

// Include the Application class file
#include "Application.as"

// Declare an instance of Application class
// It persists until RoR is shut down.
Application@ app;

// This function has full control of what happens after player clicks
// the [Rig editor] button in main menu. It may start the default RigEditor
// or perform any other set of actions.
// At the moment, the only thing possible to do here is loading and starting
// RigEditor, but that may change in the future.
void Main()
{
    LogMessage("function Main() enters");
    if (app is null)
    {
        LogMessage("Main(): Creating Application object");
        @app = Application();
        LogMessage("Main(): Application object created");
    }
    
    app.Go();
}