// TLSCallback.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

typedef struct _MY_IMAGE_TLS_DIRECTORY32 {
    DWORD   StartAddressOfRawData;
    DWORD   EndAddressOfRawData;
    DWORD   AddressOfIndex;             // PDWORD
    DWORD   AddressOfCallBacks;         // PIMAGE_TLS_CALLBACK *
    DWORD   SizeOfZeroFill;
    DWORD   Characteristics;
} MY_IMAGE_TLS_DIRECTORY32;

void runEvilFunction()
{
		typedef int (WINAPI *PMessageBoxA)(HWND, LPSTR, LPSTR, UINT);
		HMODULE hUser32 = LoadLibraryA("User32.dll");
		PMessageBoxA pMessageBoxA = (PMessageBoxA) GetProcAddress(hUser32,"MessageBoxA");
		if(pMessageBoxA)
		{
			(*pMessageBoxA)(NULL,"Fail: Already Running !","Ouch", 0);
		}
		else
		{
			MessageBox(NULL,L"Will Not be printed",L"Unseen",0);
		}
}

void WINAPI myTLSCallback(void *,int reason, void *)
{
	if (reason == 1)
	{
		runEvilFunction();
	}
}

int indices[2]={1,0};
void(WINAPI *callback_list[2])(void*,int,void*)={&myTLSCallback,0};

extern "C"
{
	MY_IMAGE_TLS_DIRECTORY32 _tls_used = { 0, 0, (DWORD)indices, (DWORD)callback_list, 0, 0 };
}

int main(int argc, char *argv[])
{
	return 0;
}