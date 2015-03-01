#pragma once
#include <d3dx9.h>

class NiteShader;
struct ShaderData;

class Effect
{
public:
	Effect();
	virtual ~Effect();

	virtual void Init() = 0;
	virtual void Render(float dt, ID3DXMesh* mesh, int numMat) = 0;
	virtual void SetData(const ShaderData*) = 0;
	virtual void Shutdown();
	static void SetDevice(IDirect3DDevice9* dev);

	virtual void OnLostDevice();
	virtual void OnResetDevice();
	
protected:
	static IDirect3DDevice9*	m_pDevice;
	ID3DXEffect*				m_pEffect;
	D3DXHANDLE					m_pTechniqueHandle;
};