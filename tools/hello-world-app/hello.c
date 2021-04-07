// Source: https://tldp.org/REF/palmdevqs/index.html

#include <PalmOS.h>

// ---------------------------------------------------------------------
// PilotMain is called by the startup code and implements a simple event
// handling loop.
// ---------------------------------------------------------------------
UInt32 PilotMain( UInt16 cmd, void *cmdPBP, UInt16 launchFlags )
{
    EventType event;


    if (cmd == sysAppLaunchCmdNormalLaunch) {

        //  Display a string.
        WinDrawChars( "Hello, world!", 13, 55, 60 );

        //  Main event loop:
        do {
            //  Doze until an event arrives.
            EvtGetEvent( &event, evtWaitForever );

            //  System gets first chance to handle the event.
            SysHandleEvent( &event );

            //  Normally, we would do other event processing here.

        // Return from PilotMain when an appStopEvent is received.
        } while (event.eType != appStopEvent);
    }
    return 0;
}

