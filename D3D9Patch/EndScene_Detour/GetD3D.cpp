#include "GetD3D.h"

LRESULT __stdcall MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

IDirect3DDevice9 *CD3DHook::getdevice() {
	IDirect3D9 *d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	IDirect3DDevice9 *d3ddevice = 0;

	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandleA(0), 0, 0, 0, 0, "DX", 0 };
	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("DX", 0, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, GetDesktopWindow(), 0, wc.hInstance, 0);

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	d3dpp.Windowed = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddevice);

	this->device = d3ddevice;

	return(d3ddevice);
}