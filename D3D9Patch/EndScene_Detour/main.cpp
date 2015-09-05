/*

I made this to prove to some dumb furry that VAC in GMod is useless as fuck.
Lets bytepatch the renderer, specifically at EndScene.

Credits:
UnknownCheats
vec, wherever he C+P'd the GetD3D device stuff from.

Sources:
http://www.unknowncheats.me/wiki/DirectX_VTable_hook_using_the_games_device_pointer_FEAR
http://www.codeproject.com/Articles/30140/API-Hooking-with-MS-Detours

*/

#include "main.h"
#include "GetD3D.h"

typedef HRESULT(__stdcall* BeginSceneFn)(LPDIRECT3DDEVICE9 thisdevice);
BeginSceneFn org_beginscene;
HRESULT __stdcall hk_beginscene(LPDIRECT3DDEVICE9 thisdevice)
{
	return org_beginscene(thisdevice);
}

typedef HRESULT(__stdcall* EndSceneFn)(LPDIRECT3DDEVICE9 thisdevice);
EndSceneFn org_endscene;
HRESULT __stdcall hk_endscene(LPDIRECT3DDEVICE9 thisdevice)
{
	return org_endscene(thisdevice);
}

void doinject()
{
	while (FindWindowA("Valve001", NULL) == NULL)
		Sleep(100);

	while (GetModuleHandleA("engine.dll") == NULL || GetModuleHandleA("client.dll") == NULL)
		Sleep(100);

	CD3DHook d3dinterface;
	IDirect3DDevice9 *device = d3dinterface.getdevice(); //gets the D3D9 device

	DWORD* d3dvtable = (DWORD *) device;
	d3dvtable = (DWORD *) d3dvtable[0]; //turns pointer of device into vtable

	org_beginscene = (BeginSceneFn) ((DWORD*) d3dvtable[41]); //41 is BeginScene
	org_endscene = (EndSceneFn) ((DWORD*) d3dvtable[42]); //42 is EndScene


	DetourTransactionBegin(); //initiates transaction
	DetourUpdateThread(GetCurrentThread()); //gets the current thread to update with the transaction
	DetourAttach(&(PVOID&) org_beginscene, hk_beginscene); //first parameter is the parameter to the function being detoured, second is replacement function
	DetourAttach(&(PVOID&) org_endscene, hk_endscene); //replace original endscene with hooked endscene
	DetourTransactionCommit(); //call DetourTransactionCommit() will make the detour do its job, also apparently checks for success/failure(?)
}

bool __stdcall DllMain(HINSTANCE hInstance, DWORD dwReasonOfCall, LPVOID lpReserved)
{
	//DestroyHeaderPE( hInstance );
	if (dwReasonOfCall == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls((HMODULE) hInstance);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE) doinject, 0, 0, 0);
	}
	TerminateThread(doinject, 0);

	return 1;
}
