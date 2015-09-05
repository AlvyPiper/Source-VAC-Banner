#pragma once

#include "main.h"

class CD3DHook {
public:
	IDirect3DDevice9 *getdevice();
private:
	IDirect3DDevice9 *device;
};