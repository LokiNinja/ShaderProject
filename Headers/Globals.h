
#include <DxErr.h>
#include <d3dx9.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define NEW new(_NORMAL_BLOCK,__FILE__,__LINE__)

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef D3D_DEBUG_INFO
		#define D3D_DEBUG_INFO
	#endif
	#ifndef HR
	#define HR(x)\
		{\
		HRESULT hr = x;\
		if (FAILED(hr))\
			{\
			LPCTSTR err = DXGetErrorString(hr);\
			OutputDebugString(err);\
			DXTrace(__FILE__, __LINE__, hr, err, TRUE);\
			}\
		}
	#endif
#else
#ifndef HR
#define HR(x)\
		x
#endif
#endif


struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 binormal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR2 texture;

	static IDirect3DVertexDeclaration9* decl;
};


#define WINDOWWIDTH GetSystemMetrics(SM_CXSCREEN)
#define WINDOWHEIGHT GetSystemMetrics(SM_CYSCREEN)

//For easy releasing of com objects
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if (x) { x->Release(); x = 0; }
#endif
