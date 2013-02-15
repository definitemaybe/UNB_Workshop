#include "AntiDebug.h"
#include "windows.h"

void *getPEBBaseAddress()
{
	void *pPEB = NULL;
	PROCESS_BASIC_INFORMATION processBasicInfo;
	ULONG retSize = 0;
	HMODULE pNTDLL = LoadLibraryA("ntdll.dll");
	PNtQueryInformationProcess pNtQueryInformationProcess = \
		(PNtQueryInformationProcess)GetProcAddress(pNTDLL,"NtQueryInformationProcess");
	if(pNtQueryInformationProcess)
	{
		if(!(*pNtQueryInformationProcess)((HANDLE)-1, 0, &processBasicInfo, sizeof(processBasicInfo), &retSize))
		{
			pPEB = processBasicInfo.PebBaseAddress;
		}
	}
	return pPEB;
}

void *getCurrentPEB()
{
	__asm {
		MOV EAX, FS:[0x30]
	}
}

int checkIsDebuggerPresent()
{
	if(IsDebuggerPresent())
		return 1;
	return 0;
}

int checkIsDebuggerPresentImpl()
{
	char *peb = (char *)getCurrentPEB();
	if (*(peb+2))
		return 1;
	return 0;
}

int checkNtGlobalFlags()
{
	char *peb = (char *)getCurrentPEB();
	if( *(peb + 0x68) & (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS))
		return 1;
	return 0;
}

int checkProcessHeapFlag()
{
	char *peb = (char *)getCurrentPEB();
	unsigned *heap = (unsigned *)(peb+0x18);
	unsigned *heapFlags = (unsigned *)(*heap + 0x10);
	if(*heapFlags != 0)
		return 1;
	return 0;
}

int checkCheckRemoteDebuggerPresent()
{
	BOOL debuggerPresent=FALSE;
	CheckRemoteDebuggerPresent((HANDLE)-1, &debuggerPresent);
	return debuggerPresent;
}

int checkNtQueryInformation()
{
	ULONG retSize = 0;
	HMODULE pNTDLL = LoadLibraryA("ntdll.dll");
	DWORD debugPort = 0;
	PNtQueryInformationProcess pNtQueryInformationProcess = \
		(PNtQueryInformationProcess)GetProcAddress(pNTDLL,"NtQueryInformationProcess");
	if(pNtQueryInformationProcess)
	{
		if(!(*pNtQueryInformationProcess)((HANDLE)-1, 7, &debugPort, sizeof(debugPort), &retSize))
		{
			if(debugPort != 0)
				return 1;
		}
	}
	return 0;
}

int checkCloseHandleNt()
{
	HMODULE hNTDLL = LoadLibraryA("ntdll.dll");
	typedef NTSTATUS (WINAPI *PNtClose)(_In_  HANDLE Handle);
	PNtClose pNtClose = (PNtClose)GetProcAddress(hNTDLL, "NtClose");
	if(pNtClose)
	{
		__try
		{
			(*pNtClose)((HANDLE)123123);
			return 0;
		}
		__except(1)
		{
			return 1;
		}
	}
	return 0;
}

int checkCloseHandle()
{
	__try
	{
		CloseHandle((HANDLE)12344321);
		return 0;
	}
	__except (1)
	{
		return 1;
	}
}

int checkUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER unhandledExceptionHandler)
{
	SetUnhandledExceptionFilter(unhandledExceptionHandler);
	int x=0,y=1;
	x = y/x;
	return 0;
}

void hideCodeFromDebugger()
{
	typedef NTSTATUS (WINAPI *PNtSetInformationThread)\
		(_In_	HANDLE ThreadHandle,
		_In_  DWORD ThreadInformationClass,
		_In_  PVOID ThreadInformation,
		_In_  ULONG ThreadInformationLength);

	HMODULE hNTDLL = LoadLibraryA("ntdll.dll");
	PNtSetInformationThread pNtSetInformationThread = (PNtSetInformationThread) GetProcAddress(hNTDLL, "NtSetInformationThread");
	//0x11 - thread hide from debugger
	(*pNtSetInformationThread)((HANDLE)-2, 0x11, 0,0);
}

int checkOutputDebugString()
{
	char *msg = "Being Debugged";
	OutputDebugStringA(msg);
	__asm{
		CMP EAX,0x01
		JNE LABEL_2
		JMP LABEL_1
	}
LABEL_1:	return 0;
LABEL_2:	return 1;
}

int checkINT3()
{
	__try
	{
		__asm{
			INT 3
		}
	}
	__except(1)
	{
		return 0;
	}
	return 1;
}

int checkINT2D()
{
	__try
	{
		__asm{
			INT 0x2D
			NOP
			JMP LABEL_INT3
		}
	}
	__except(1)
	{
		return 0;
	}
LABEL_INT3:	return 1;
}

/*int checkIceBreak()
{

}*/

int newCheckInt2D()
{
	__try{
		__asm
		{
			XOR EAX,EAX
			JNE INT2DLAB
			JMP INT2DLAB+1
	INT2DLAB:	mov eax,0x90902DCD
			JMP INT2DDEB
		}
	}
	__except(1)
	{
		return 0;
	}
INT2DDEB: 
	return 1;
}

int checkNewHeapFlags()
{
	char *peb = (char *)getPEBBaseAddress();
	unsigned *numHeaps = (unsigned *)(peb+0x88);
	if(*numHeaps > 1)
	{
		unsigned *heapList = (unsigned *)(peb+0x90);
		unsigned *heapPointer = (unsigned *)*heapList;
		heapPointer++;
		unsigned *heapFlags = (unsigned *)(*heapPointer + 0x10);
		if(*heapFlags != 0)
			return 1;
		return 0;
	}
	else
	{
		__asm
		{
			xor EAX,EAX
			call EAX
		}
		return 0;
	}
}
