#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class Light
{
public:
	//Constructor/Destructor
	Light();
	~Light();

	//Update for position changes and reset
	void Update(float dt);

	//Accessors and mutators
	void SetDiffuse(D3DXVECTOR3& diffuse);
	void SetSpecular(D3DXVECTOR3& specular);
	void SetAttenuation(D3DXVECTOR3& attenuation);
	void SetDistance(float distance);

	const D3DXVECTOR3&	GetPosition() const;
	const float			GetDistance() const;
	const D3DXVECTOR3&	GetDiffuse() const;
	const D3DXVECTOR3&	GetSpecular() const;
	const D3DXVECTOR3&  GetAttenuation() const;
	const D3DXMATRIX&	GetLightSpace() const;
	const D3DXMATRIX&	GetLightTextureSpace() const;
private:
	//For position calculations
	D3DXVECTOR3	m_position;
	float		m_phi;
	float		m_theta;
	float		m_distance;

	//Light components
	D3DXVECTOR3	m_diffuse;
	D3DXVECTOR3 m_specular;
	D3DXVECTOR3 m_attenuation;

	//Light Matrix
	D3DXMATRIX	m_lightSpace;
	//Texturespace
	D3DXMATRIX	m_lightTextureSpace;
};