#pragma once
#include "Effect.h"


class SubsurfaceScatterEffect : public Effect
{
public:
	//Constructor/Destructor
	SubsurfaceScatterEffect(NiteShader* parent, char* texture);
	virtual ~SubsurfaceScatterEffect();

	//Interface
	void Init() override;
	void Shutdown() override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials);
	void Update(float dt);

private:
	IDirect3DTexture9*			m_pGradientTexture;
	char*						m_pTextureFile;
	float						m_grow;
	//Render to surface
	IDirect3DSurface9*			m_pSurface;

	IDirect3DTexture9*			m_pDepthTexture;
	D3DVIEWPORT9				m_viewport;

	//Matrix for light space
	D3DXMATRIX					m_lightSpace;
	D3DXMATRIX					m_lightProj;
	D3DXMATRIX					m_textureBias;


	D3DXHANDLE			m_pWorldViewProjHandle;
	D3DXHANDLE			m_pWorldHandle;
	D3DXHANDLE			m_pWorldViewHandle;
	D3DXHANDLE			m_pLightTexMatrixHandle;
	D3DXHANDLE			m_pLightMatrixHandle;
	D3DXHANDLE			m_pGrowHandle;
	D3DXHANDLE			m_pGradientTextureHandle;
	D3DXHANDLE			m_pLightDepthTextureHandle;

	void				CreateTextureBias();
};