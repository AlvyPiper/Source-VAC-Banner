#pragma once

#include "windows.h"
#include <iostream>
#include <sstream>

/*D3D9 shitttt*/

#include "d3dx9.h"
#include "d3d9types.h"
#include "d3d9caps.h"
#include "d3dx9tex.h"

#pragma comment( lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3d9.lib" ) //fuck this shit
#pragma comment( lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx9.lib" )

/*Detour shitttt*/

#include "src/detours.h"
#include "src/detver.h"

#pragma comment( lib, "C:\\Program Files (x86)\\Microsoft Research\\Detours Express 3.0\\lib.X86\\detours.lib" )

template<typename I>
inline I CreateInterface(const char *dll, const char *name)
{
	return (I) (((void *(*)(const char *, void *))GetProcAddress(GetModuleHandle(dll), "CreateInterface"))(name, 0));
}

class IClient
{
public:
	
};