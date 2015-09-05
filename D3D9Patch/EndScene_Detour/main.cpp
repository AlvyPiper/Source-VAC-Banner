/*

I made this to prove to some dumb furry that VAC does nothing in GMod.
In this, we bytepatch CreateMove, PaintTraverse, BeginScene, and EndScene, if this amount of sketchy hooks doesn't get you VAC banned...I have no clue what will.

Credits:
CodeProject, helping me understand MSDetours.
UnknownCheats, tutorial on patching VTables with MSDetours.
nanocat, CreateInterface template
vec, wherever he C+P'd the GetD3D device stuff from.
aethi, dumpass who posted this on facepunch for me
sonador, being so dumb it inspired me to make this

Sources:
http://www.unknowncheats.me/wiki/DirectX_VTable_hook_using_the_games_device_pointer_FEAR
http://www.codeproject.com/Articles/30140/API-Hooking-with-MS-Detours

*/

#include "main.h"
#include "GetD3D.h"

void	*client;
void	*panel;

typedef void(__thiscall *CreateMoveFn)(void *, int, float, bool);
CreateMoveFn org_createmove;
void __fastcall hk_createmove(void *thishook, void *, int sequence_number, float frametime, bool active)
{
	return org_createmove(thishook, sequence_number, frametime, active);
}

typedef void(__thiscall *PaintTraverseFn)(void*, unsigned int, bool, bool);
PaintTraverseFn org_painttraverse;
void __fastcall hk_painttraverse(void* thishook, void*, unsigned int panel, bool forcerepaint, bool forceallow)
{
	return org_painttraverse(thishook, panel, forcerepaint, forceallow);
}

typedef HRESULT(__thiscall* BeginSceneFn)(LPDIRECT3DDEVICE9 thisdevice);
BeginSceneFn org_beginscene;
HRESULT __stdcall hk_beginscene(LPDIRECT3DDEVICE9 thisdevice)
{
	return org_beginscene(thisdevice);
}

typedef HRESULT(__thiscall* EndSceneFn)(LPDIRECT3DDEVICE9 thisdevice);
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

	client = CreateInterface<void *>("client", "VClient017");
	panel = CreateInterface<void *>("vgui2", "VGUI_Panel009");

	CD3DHook d3dinterface;
	IDirect3DDevice9 *device = d3dinterface.getdevice(); //gets the D3D9 device

	DWORD* d3dvtable = (DWORD *) device;
	d3dvtable = (DWORD *) d3dvtable[0]; //turns pointer of device into vtable

	DWORD* clienttable = (DWORD *) client;
	clienttable = (DWORD*) clienttable[0]; //grab client virtual table

	DWORD* paneltable = (DWORD *) panel;
	paneltable = (DWORD*) paneltable[0]; //grab panel virtual table

	org_beginscene = (BeginSceneFn) ((DWORD*) d3dvtable[41]); //41 is BeginScene
	org_endscene = (EndSceneFn) ((DWORD*) d3dvtable[42]); //42 is EndScene
	org_createmove = (CreateMoveFn) ((DWORD*) clienttable[21]); //21 is CreateMove
	org_painttraverse = (PaintTraverseFn) ((DWORD*) paneltable[41]); //41 is PaintTraverse

	DetourTransactionBegin(); //initiates transaction
	DetourUpdateThread(GetCurrentThread()); //gets the current thread to update with the transaction
	DetourAttach(&(void*&) org_beginscene, hk_beginscene); //first parameter is the parameter to the function being detoured, second is replacement function
	DetourAttach(&(void*&) org_endscene, hk_endscene); //replace original endscene with hooked endscene
	DetourAttach(&(void*&) org_painttraverse, hk_painttraverse); //replace original painttraverse with hooked painttraverse
	DetourAttach(&(void*&) org_createmove, hk_createmove); //replace original createmove with hooked createmove
	DetourTransactionCommit(); //call DetourTransactionCommit() will make the detour do its job, also apparently checks for success/failure(?)
}

bool __stdcall DllMain(HINSTANCE hInstance, DWORD dwReasonOfCall, LPVOID lpReserved)
{
	if (dwReasonOfCall == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls((HMODULE) hInstance);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE) doinject, 0, 0, 0);
	}
	TerminateThread(doinject, 0);

	return 1;
}
