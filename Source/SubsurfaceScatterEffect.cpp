#include "ShaderData.h"
#include "Light.h"
#include "SubsurfaceScatterEffect.h"
#include "Globals.h"
#include "Camera.h"

//Constructor/Destructor
SubsurfaceScatterEffect::SubsurfaceScatterEffect(const WCHAR* texture)
{
	m_pGradientTexture = 0;
	m_pDepthTexture = 0;
	m_pTextureFile = texture;
	m_pSurface = 0;
	m_grow = 7.5f;
	//handles
	m_pWorldViewProjHandle = 0;
	m_pWorldHandle = 0;
	m_pLightTexMatrixHandle = 0;
	m_pLightMatrixHandle = 0;
	m_pGrowHandle = 0;
	m_pGradientTextureHandle = 0;
	m_pLightDepthTextureHandle = 0;
	m_pLightViewProj = 0;
}

SubsurfaceScatterEffect::~SubsurfaceScatterEffect()
{
	Shutdown();
}

//Interface
void SubsurfaceScatterEffect::Init()
{
	CreateTextureBias();
	//Get Handles
	m_pWorldViewProjHandle = m_pEffect->GetParameterByName(0, "worldViewProj");
	m_pLightViewProj = m_pEffect->GetParameterByName(0, "lightViewProj");
	m_pWorldHandle = m_pEffect->GetParameterByName(0, "world");
	m_pWorldViewHandle = m_pEffect->GetParameterByName(0, "worldView");
	m_pLightTexMatrixHandle = m_pEffect->GetParameterByName(0, "lightTexMatrix");
	m_pLightMatrixHandle = m_pEffect->GetParameterByName(0, "lightMatrix");
	m_pGrowHandle = m_pEffect->GetParameterByName(0, "grow");
	m_pLightDepthTextureHandle = m_pEffect->GetParameterByName(0, "lightDepthTex");
	m_pGradientTextureHandle = m_pEffect->GetParameterByName(0, "gradientTex");

	D3DXMatrixOrthoLH(&m_lightProj, 16.f, 16.f, 1.f, 100.f);

	HR(D3DXCreateTextureFromFile(m_pDevice, m_pTextureFile, &m_pGradientTexture));

	//Get technique handle
	m_pTechniqueHandle = m_pEffect->GetTechniqueByName("SubSurfaceScatter");
	HR(m_pEffect->SetTechnique(m_pTechniqueHandle));
	//Set values
	HR(m_pEffect->SetTexture(m_pGradientTextureHandle, m_pGradientTexture));
	//Render to texture
	HR(D3DXCreateTexture(m_pDevice, 512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F,
		D3DPOOL_DEFAULT, &m_pDepthTexture));
	HR(m_pDepthTexture->GetSurfaceLevel(0, &m_pSurface));
}

void SubsurfaceScatterEffect::Shutdown()
{
	SAFE_RELEASE(m_pGradientTexture);
	SAFE_RELEASE(m_pDepthTexture);
	SAFE_RELEASE(m_pSurface);
}

void SubsurfaceScatterEffect::Render(float dt, ID3DXMesh* mesh, int numMaterials)
{
	IDirect3DSurface9*			m_pBackBuffer = nullptr;
	UINT passes;
	//Set up pass 1;
	HR(m_pDevice->GetRenderTarget(0, &m_pBackBuffer));
	HR(m_pDevice->SetRenderTarget(0, m_pSurface));
	HR(m_pDevice->Clear(0, 0, D3DCLEAR_TARGET , D3DXCOLOR(0.0f, 0.7f, 0.5f, 1.0f), 1.0f, 0));
	HR(m_pDevice->BeginScene());
	HR(m_pEffect->Begin(&passes, 0));
	//Pass1 -- Depth
	
	HR(m_pEffect->BeginPass(0));
	for (int i = 0; i < numMaterials; i++)
	{
		HR(mesh->DrawSubset(i));
	}
	HR(m_pEffect->EndPass());
	HR(m_pEffect->End());
	HR(m_pDevice->EndScene());
	HR(m_pDevice->SetRenderTarget(0, m_pBackBuffer));
	HR(m_pDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.7f, 0.5f, 1.0f), 1.0f, 0));
	HR(m_pEffect->SetTexture(m_pLightDepthTextureHandle, m_pDepthTexture));
	HR(m_pDevice->BeginScene());
	HR(m_pEffect->Begin(&passes, 0));
	//Pass 2 -- Draw Pass
	HR(m_pEffect->BeginPass(1));
	for (int i = 0; i < numMaterials; i++)
	{
		HR(mesh->DrawSubset(i));
	}
	HR(m_pEffect->EndPass());
	HR(m_pEffect->End());
	SAFE_RELEASE(m_pBackBuffer);
	HR(m_pDevice->EndScene());
}

void SubsurfaceScatterEffect::SetData(const ShaderData *data)
{
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&m_lightSpace);
	D3DXVECTOR3 toOrigin, lightPos, test;
	lightPos = data->light->GetPosition();
	D3DXVec3Normalize(&toOrigin, &(D3DXVECTOR3(0.f, 0.f, 0.f) - lightPos));
	D3DXVec3Cross(&test, &toOrigin, &D3DXVECTOR3(1.f, 0.f, 0.f));
	//No way to know orientation so test for 2 cross products if necessary
	if (test == D3DXVECTOR3(0.f, 0.f, 0.f))
	{
		D3DXVec3Cross(&test, &D3DXVECTOR3(0.f, 1.f, 0.f), &toOrigin);
	}
	D3DXMatrixLookAtLH(&temp, &lightPos, &D3DXVECTOR3(0.f, 0.f, 0.f), &test);
	m_lightSpace = *(data->world) * temp;

	HR(m_pEffect->SetMatrix(m_pLightMatrixHandle, &(m_lightSpace)));
	HR(m_pEffect->SetMatrix(m_pLightTexMatrixHandle, &(m_lightSpace * m_lightProj * m_textureBias)));
	HR(m_pEffect->SetFloat(m_pGrowHandle, m_grow));
	HR(m_pEffect->SetMatrix(m_pLightViewProj, &(m_lightSpace * m_lightProj)));
	temp = *(data->world) * *(data->view) * *(data->proj);
	HR(m_pEffect->SetMatrix(m_pWorldViewProjHandle, &temp));
	HR(m_pEffect->SetMatrix(m_pWorldHandle, data->world));
}

void SubsurfaceScatterEffect::CreateTextureBias()
{
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&m_textureBias);
	D3DXMatrixScaling(&temp, .5f, .5f, .5f);
	m_textureBias *= temp;
	D3DXMatrixTranslation(&temp, .5f, .5f, .5f);
	m_textureBias *= temp;
}

Effect* SubsurfaceScatterEffect::Create(ID3DXBuffer** errors)
{
	SubsurfaceScatterEffect* newEffect = NEW SubsurfaceScatterEffect(L"Textures/SubsurfaceTexture.bmp");
	D3DXCreateEffectFromFile(m_pDevice, L"Shaders/SubsurfaceScatter.fx", 0, 0, D3DXSHADER_DEBUG, 0, &newEffect->m_pEffect, errors);
	return newEffect;
}
