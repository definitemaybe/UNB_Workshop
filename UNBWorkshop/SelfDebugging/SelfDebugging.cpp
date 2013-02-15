// SelfDebugging.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
	DEBUG_EVENT de = {0};
	if(argc > 1)
	{
		//Child Code
		DWORD pPID=0;
		_snscanf(argv[1], 6, "%u", &pPID);
		//MessageBoxA(NULL, argv[1], "Child Running", 0); 
		if(!DebugActiveProcess(pPID))
		{
			MessageBoxA(NULL, "Debugger Detected!", "AntiDebug", 0);
		}
		else
		{
			WaitForDebugEvent(&de, INFINITE);
			ContinueDebugEvent(de.dwProcessId, de.dwProcessId, DBG_CONTINUE);
			DebugActiveProcessStop(pPID);
			MessageBoxA(NULL, "No Debugger Detected - execute malware code !", "AntiDebug", 0);
		}
	}
	else
	{
		//Parent Code
		char selfFileName[MAX_PATH];
		char cmdLine[MAX_PATH+10];
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		memset(&si,0,sizeof(si));
		memset(&pi,0,sizeof(pi));
		DWORD processId;
		GetModuleFileNameA((HMODULE)NULL, selfFileName, MAX_PATH);
		processId = GetCurrentProcessId();
		_snprintf(cmdLine, MAX_PATH+10, "\"%s\" %u", selfFileName, processId);
		CreateProcessA(selfFileName, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		if(DebugActiveProcess(pi.dwProcessId))
		{
			WaitForDebugEvent(&de, INFINITE);
			ContinueDebugEvent(de.dwProcessId, de.dwProcessId, DBG_CONTINUE);
			DebugActiveProcessStop(pi.dwProcessId);
			Sleep(2000);
		}
	}
}