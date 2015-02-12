#pragma once
#include "Effect.h"


class ShadowmapEffect : public Effect
{
public:
	ShadowmapEffect(NiteShader* parent, char* sphereTex, char* groundTex);
	virtual ~ShadowmapEffect();

	void Init() override;
	void Shutdown() override;
	void Update(float dt) override;
	void Render(float dt, ID3DXMesh* mesh, int numMaterials) override;
protected:
	//Sphere texture
	IDirect3DTexture9*			m_pSphereTexture;
	char*						m_pSphereTextureFile;

	//Ground Plane texture
	IDirect3DTexture9*			m_pGroundTexture;
	char*						m_pGroundTextureFile;
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

	//This stuff should really be integrated with the light itself but
	//would require too much code shuffling at this point
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
	void UpdateLightView();
	//Updates the light projection
	void UpdateLightProj();
};