#ifndef ANTIDEBUG_H_F21CBE70_1FA3_4443_AB82_88BD5C18F967
#define ANTIDEBUG_H_F21CBE70_1FA3_4443_AB82_88BD5C18F967

#include "windows.h"

#define FLG_HEAP_ENABLE_TAIL_CHECK 0x10 
#define FLG_HEAP_ENABLE_FREE_CHECK 0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40

typedef NTSTATUS (WINAPI *PNtQueryInformationProcess)(
  _In_       HANDLE ProcessHandle,
  _In_       DWORD ProcessInformationClass,
  _Out_      PVOID ProcessInformation,
  _In_       ULONG ProcessInformationLength,
  _Out_opt_  PULONG ReturnLength
);

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

void *getCurrentPEB();
int checkIsDebuggerPresent();
int checkIsDebuggerPresentImpl();
int checkNtGlobalFlags();
int checkProcessHeapFlag();
int checkNewHeapFlags();
int checkCheckRemoteDebuggerPresent();
int checkCloseHandle();
int checkCloseHandleNt();
int checkUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER);
void hideCodeFromDebugger();
int checkOutputDebugString();
int checkINT3();
int checkINT2D();
int newCheckInt2D();
#endif