#pragma once
#include <d3d11.h>  
#include <DXGI.h>    

// D3D DEBUG 
#include <dxgidebug.h>
#pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

#define DX_SAFE_RELEASE(dx_resource)\
 if ((dx_resource) != nullptr) {\
	dx_resource->Release();\
	dx_resource = nullptr;\
 }
