
# ==============================================================================
# Rigs of Rods - Rig Editor
# Main script
# ==============================================================================

# This function has full control of what happens after player clicks
# the [Rig editor] button in main menu. It may start the default RigEditor
# or perform any other set of actions.

print('Hello, Rig editor here!')

try:

    import application
    app = application.Application()
    app.go()

except:
    
    import crash_reporter
    reporter = crash_reporter.CrashReporter()
    reporter.report_current_exception()
