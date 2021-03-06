// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "cViewer.h"
#include <CppRevEng.h>

// First of all, the base address
BaseAddressDeclare;

// When the game calls SetRenderType(0xF), it's to show an hologram
// We will hook (detour) it so that the hologram is never shown

member_detour(RenderType_detour, App::cViewer, void(int, bool)) 
{
	void detoured(int renderType, bool arg1) 
	{
		if (renderType == 0xF) {
			// When the game tries to use hologram, use the default render type instead
			original_function(this, 0, arg1);
		}
		else {
			original_function(this, renderType, arg1);
		}
	}
};  // it's a class, don't forget the semicolon

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		// Must be the first thing you do
		InitCppRevEng();

		PrepareDetours(hModule);
		RenderType_detour::attach(GetAddress(App::cViewer, SetRenderType));
		CommitDetours();

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:

		PrepareDetours(hModule);
		RenderType_detour::detach();
		CommitDetours();

        break;
    }
    return TRUE;
}

