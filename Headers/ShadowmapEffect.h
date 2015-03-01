#pragma once
#include "Effect.h"

class Light;

class ShadowmapEffect : public Effect
{
public:

	virtual ~ShadowmapEffect();

	void Init() override;
	void Shutdown() override;
	void SetData(const ShaderData*) override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials) override;
	static Effect* Create(ID3DXBuffer**);
protected:
	ShadowmapEffect(const WCHAR* sphereTex, const WCHAR* groundTex);
	ShadowmapEffect() {}

	//Sphere texture
	IDirect3DTexture9*			m_pSphereTexture;
	const WCHAR*				m_pSphereTextureFile;

	//Ground Plane texture
	IDirect3DTexture9*			m_pGroundTexture;
	const WCHAR*				m_pGroundTextureFile;
	//Ground plane vertex buffer
	IDirect3DVertexBuffer9*		m_pGroundVB;
	IDirect3DIndexBuffer9*		m_pGroundIB;

	//Shadow map
	IDirect3DTexture9*			m_pShadowMapTexture;
	IDirect3DSurface9*			m_pShadowMapSurface;

	//Handles
	D3DXHANDLE					m_pWorldViewProjHandle;
	D3DXHANDLE					m_pWorldHandle;
	D3DXHANDLE					m_pLightSpaceHandle;
	D3DXHANDLE					m_pLightViewProjHandle;
	D3DXHANDLE					m_pTextureBiasHandle;
	D3DXHANDLE					m_pTextureHandle;
	D3DXHANDLE					m_pShadowMapHandle;
	D3DXHANDLE					m_pShadowMapResolutionHandle;

	D3DXMATRIX	m_lightSpace;
	D3DXMATRIX	m_textureSpace;
	D3DXMATRIX  m_textureBias;
	D3DXMATRIX  m_lightProj;

	//Helper functions
	void InitGround();
	//Draws the ground
	void DrawGround();
	//Set texture bias and projection
	void InitTextureBias();
	//Update light view
	void UpdateLightView(const Light* light);
	//Updates the light projection
	void UpdateLightProj();
};