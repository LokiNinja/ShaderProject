#include "Camera.h"
#include "Input.h"

//Constructor/Destructor
Camera::Camera(D3DXVECTOR3 pos)
{
	D3DXMatrixIdentity(&m_view);
	D3DXMatrixIdentity(&m_proj);
	D3DXMatrixIdentity(&m_viewProj);

	m_position = pos;
	m_cameraX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_cameraY    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_cameraZ  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	// Client should adjust to a value that makes sense for application's
	// unit scale, and the object the camera is attached to--e.g., car, jet,
	// human walking, etc.
	m_speed  = 100.0f;
}


Camera::~Camera()
{
}

//Accessors/mutators
const D3DXMATRIX&	Camera::GetView() const
{
	return m_view;
}

const D3DXMATRIX&	Camera::GetProjection() const
{
	return m_proj;
}

const D3DXMATRIX&	Camera::GetViewProj() const
{
	return m_viewProj;
}

//Returns the basis vectors
const D3DXVECTOR3&	Camera::GetCameraZ() const
{
	return m_cameraZ;
}

const D3DXVECTOR3&	Camera::GetCameraX() const
{
	return m_cameraX;
}

const D3DXVECTOR3&	Camera::GetCameraY() const
{
	return m_cameraY;
}

//Returns the position of the camera
const D3DXVECTOR3&		Camera::GetPos() const
{
	return m_position;
}

//Sets the camera speed
void				Camera::SetSpeed(float newSpeed)
{
	m_speed = newSpeed;
}

//update function
void				Camera::Update(float dt)
{
	//Relative direction of camera
	D3DXVECTOR3			dir(0.f, 0.f, 0.f);

	if (g_Input->KeyDown(DIK_W))
	{
		dir += m_cameraZ;
	}
	if (g_Input->KeyDown(DIK_S))
	{
		dir -= m_cameraZ;
	}
	if (g_Input->KeyDown(DIK_A))
	{
		dir -= m_cameraX;
	}
	if (g_Input->KeyDown(DIK_D))
	{
		dir += m_cameraX;
	}
	if (g_Input->KeyDown(DIK_Q))
	{
		dir -= m_cameraY;
	}
	if (g_Input->KeyDown(DIK_E))
	{
		dir += m_cameraY;
	}
	if (g_Input->KeyDown(DIK_C))
	{
		LookAt(D3DXVECTOR3(0.f, 0.f, -20.f), D3DXVECTOR3(0.f, 0.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f));
	}
	//Add relative position to current position based on 
	//elapsed time and speed
	D3DXVec3Normalize(&dir, &dir);
	m_position += dir * m_speed * dt;

	//Only move camera if left mouse button is down
	if (g_Input->MouseButtonDown(0))
	{
		float yaw, pitch, roll;
		//Get the mouse movements
		g_Input->MousePos(yaw, pitch, roll);
		yaw /= 300.f;
		pitch /= 300.f;

		//Rotate cameras look and up vectors around camera x
		D3DXMATRIX	   R;
		D3DXMatrixRotationAxis(&R, &m_cameraX, pitch);
		D3DXVec3TransformCoord(&m_cameraZ, &m_cameraZ, &R);
		D3DXVec3TransformCoord(&m_cameraY, &m_cameraY, &R);

		//Rotate cameras look and right vectors around world y
		D3DXMatrixRotationY(&R, yaw);
		D3DXVec3TransformCoord(&m_cameraX, &m_cameraX, &R);
		D3DXVec3TransformCoord(&m_cameraY, &m_cameraY, &R);
		D3DXVec3TransformCoord(&m_cameraZ, &m_cameraZ, &R);
	}

	//Build the NEW view matrix
	BuildView();
}

//Builds the camera view matrix
void				Camera::BuildView()
{
	//Make sure the axis stay orthogonal
	D3DXVec3Normalize(&m_cameraZ, &m_cameraZ);
	D3DXVec3Cross(&m_cameraY, &m_cameraZ, &m_cameraX);
	D3DXVec3Normalize(&m_cameraY, &m_cameraY);
	D3DXVec3Cross(&m_cameraX, &m_cameraY, &m_cameraZ);
	D3DXVec3Normalize(&m_cameraX, &m_cameraX);

	//Create view matrix
	float x = -D3DXVec3Dot(&m_position, &m_cameraX);
	float y = -D3DXVec3Dot(&m_position, &m_cameraY);
	float z = -D3DXVec3Dot(&m_position, &m_cameraZ);

	m_view(0, 0) = m_cameraX.x;
	m_view(1, 0) = m_cameraX.y;
	m_view(2, 0) = m_cameraX.z;
	m_view(3, 0) = x;

	m_view(0, 1) = m_cameraY.x;
	m_view(1, 1) = m_cameraY.y;
	m_view(2, 1) = m_cameraY.z;
	m_view(3, 1) = y;

	m_view(0, 2) = m_cameraZ.x;
	m_view(1, 2) = m_cameraZ.y;
	m_view(2, 2) = m_cameraZ.z;
	m_view(3, 2) = z;

	m_view(0, 3) = 0.f;
	m_view(1, 3) = 0.f;
	m_view(2, 3) = 0.f;
	m_view(3, 3) = 1.f;

	//Update viewProj matrix
	m_viewProj = m_view * m_proj;
}

void Camera::LookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up)
{
	D3DXVECTOR3 L = target - pos;
	D3DXVec3Normalize(&L, &L);

	D3DXVECTOR3 R;
	D3DXVec3Cross(&R, &up, &L);
	D3DXVec3Normalize(&R, &R);

	D3DXVECTOR3 U;
	D3DXVec3Cross(&U, &L, &R);
	D3DXVec3Normalize(&U, &U);

	m_position   = pos;
	m_cameraX = R;
	m_cameraY  = U;
	m_cameraZ  = L;

	BuildView();

	m_viewProj = m_view * m_proj;
}

//Sets the projections
void Camera::SetProjection(float fov, float aspect, float nearZ, float farZ)
{
	D3DXMatrixPerspectiveFovLH(&m_proj, fov, aspect, nearZ, farZ);
	m_viewProj = m_view * m_proj;
}