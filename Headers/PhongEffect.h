#pragma once
#include "Effect.h"

class PhongEffect : public Effect
{
public:
	//Constructor/Destructor
	PhongEffect(NiteShader* parent, char* texture);
	virtual ~PhongEffect();

	//Interface
	void Init() override;
	void Shutdown() override;
	void Update(float dt) override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials);
protected:
	//Texture stuff
	IDirect3DTexture9*		m_pTexture;
	char*					m_pTextureFile;

	//Handles
	D3DXHANDLE				m_pWorldViewProjHandle;
	D3DXHANDLE				m_pWorldHandle;
	D3DXHANDLE				m_pTextureHandle;
	D3DXHANDLE				m_pEyePositionHandle;
	D3DXHANDLE				m_pLightPositionHandle;
	D3DXHANDLE				m_pLightDiffuseHandle;
	D3DXHANDLE				m_pLightSpecularHandle;
	D3DXHANDLE				m_pLightAttenuationHandle;
};