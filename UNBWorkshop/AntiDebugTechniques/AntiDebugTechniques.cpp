// AntiDebugTechniques.cpp : Defines the entry point for the console application.
//

#include "AntiDebug.h"
#include "windows.h"

LONG WINAPI unhandledExceptionHandler(struct _EXCEPTION_POINTERS *exceptPtr)
{
	MessageBoxA(NULL, "Not Being Debugged", "AntiDebug Techiniques", 0);
	return 1;
}

int main(int argc, char* argv[])
{
	int isBeingDebugged = -1;
	if(argc > 1)
	{
		isBeingDebugged = checkIsDebuggerPresentImpl();
	}
	else
	{
//		isBeingDebugged = checkIsDebuggerPresent();
//		isBeingDebugged = checkNtGlobalFlags();
//		isBeingDebugged = checkProcessHeapFlag();
//		isBeingDebugged = checkCheckRemoteDebuggerPresent();
//		isBeingDebugged = checkCheckRemoteDebuggerPresent();
//		isBeingDebugged = checkCloseHandle();
//		isBeingDebugged = checkCloseHandleNt();
//		isBeingDebugged = checkUnhandledExceptionFilter(unhandledExceptionHandler);
/*		hideCodeFromDebugger();
		MessageBoxA(NULL, "There can be no breakpoint now", "Antidebug", 0);
		return 0;*/
//		isBeingDebugged = checkOutputDebugString();
//		isBeingDebugged = checkINT3();
		isBeingDebugged = checkINT2D();
//		isBeingDebugged = newCheckInt2D();
	}
	if (isBeingDebugged)
		MessageBoxA(NULL, "Being Debugged", "Message", 0);
	else
		MessageBoxA(NULL, "Not being debugged", "Message", 0);
	return 0;
}
