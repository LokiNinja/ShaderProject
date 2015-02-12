#pragma once
#include "Effect.h"


class BumpEffect : public Effect
{
public:
	//Constructor/Destructor
	BumpEffect(NiteShader* parent, char* texture, char* normalMap);
	~BumpEffect();

	void Init() override;
	void Shutdown() override;
	void Update(float dt) override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials) override;
private:
	//Texture and normal map stuff
	IDirect3DTexture9*		m_pTexture;
	char*					m_pTextureFile;
	IDirect3DTexture9*		m_pNormalMap;
	char*					m_pNormalMapFile;

	D3DXHANDLE				m_pWorldViewProjHandle;
	D3DXHANDLE				m_pWorldHandle;
	D3DXHANDLE				m_pLightDiffuseHandle;
	D3DXHANDLE				m_pLightAttenuationHandle;
	D3DXHANDLE				m_pWorldInvHandle;
	D3DXHANDLE				m_pLightDirHandle;
	D3DXHANDLE				m_pEyePosHandle;
	D3DXHANDLE				m_pTextureHandle;
	D3DXHANDLE				m_pNormalMapHandle;
};