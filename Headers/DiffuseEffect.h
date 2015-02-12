#pragma once
#include "Effect.h"

class DiffuseEffect : public Effect
{
public:
	//Constructor Destructor
	DiffuseEffect(NiteShader* parent, char* texture);
	virtual ~DiffuseEffect();

	//Base class overrides
	void Init() override;
	void Render(float dt, ID3DXMesh* mesh, int numMat) override;
	void Shutdown() override;
	void Update(float dt) override;
protected:
	//Texture info
	IDirect3DTexture9*			m_pTexture;
	char*						m_pTextureFile;
	//Handles
	D3DXHANDLE					m_pWorldViewProjHandle;
	D3DXHANDLE					m_pWorldHandle;
	D3DXHANDLE					m_pTextureHandle;
	D3DXHANDLE					m_pLightPositionHandle;
};