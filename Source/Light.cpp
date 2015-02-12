#include "Light.h"
#include "Input.h"

//Constructor/Destructor
Light::Light()
{
	m_position = D3DXVECTOR3(0.f, 10.f, -15.f);
	m_diffuse = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_specular = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_attenuation = D3DXVECTOR3(0.f, .5f, 0.f);
	m_distance = sqrt(m_position.x*m_position.x + m_position.y*m_position.y + m_position.z*m_position.z);
	m_phi = acosf(m_position.y/m_distance);
	m_theta = acosf(m_position.x/(m_distance*sinf(m_phi)));
}

Light::~Light()
{
}

//Update for position changes and reset
void Light::Update(float dt)
{
	//Reset the light
	if (g_Input->KeyDown(DIK_L))
	{
		m_position = D3DXVECTOR3(0.f, 10.f, -15.f);
		m_distance = sqrt(m_position.x*m_position.x + m_position.y*m_position.y + m_position.z*m_position.z);
		m_phi = acosf(m_position.y/m_distance);
		m_theta = acosf(m_position.x/(m_distance*sinf(m_phi)));
	}
	//Mouse light when right mouse is pressed
	if (g_Input->MouseButtonDown(1))
	{
		float x, y, z;
		g_Input->MousePos(x, y, z);
		m_phi += .5f * dt * y;
		m_theta += .5f * dt * x;
		m_position = D3DXVECTOR3((m_distance * sinf(m_phi) * cosf(m_theta)), (m_distance * cosf(m_phi)), (m_distance * sinf(m_phi)* sinf(m_theta)));
	}
}

//Accessors and mutators
void Light::SetDiffuse(D3DXVECTOR3& diffuse)
{
	m_diffuse = diffuse;
}

void Light::SetSpecular(D3DXVECTOR3& specular)
{
	m_specular = m_specular;
}

void Light::SetAttenuation(D3DXVECTOR3& attenuation)
{
	m_attenuation = attenuation;
}

void Light::SetDistance(float distance)
{
	m_distance = distance;
	D3DXVec3Normalize(&m_position, &m_position);
	m_position *= m_distance;
}

const D3DXVECTOR3&  Light::GetSpecular() const
{
	return m_specular;
}

const D3DXVECTOR3&  Light::GetDiffuse() const
{
	return m_diffuse;
}

const D3DXVECTOR3&	Light::GetPosition() const
{
	return m_position;
}

const D3DXVECTOR3&  Light::GetAttenuation() const
{
	return m_attenuation;
}

const float			Light::GetDistance() const
{
	return m_distance;
}
