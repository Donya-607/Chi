#include "Camera.h"
#include "system.h"

Camera::Camera()
{
	position = DirectX::XMFLOAT3(0.0f, 500.0f, -500.0f);
	target = DirectX::XMFLOAT3(.0f, .0f, .0f);
	state = cameraNumber::MONITORING;
	float fov = DirectX::XMConvertToRadians(30.0f);
	float aspect = pSystem->SCREEN_WIDTH / pSystem->SCREEN_HEIGHT;
	SetPerspectiveMatrix(fov, aspect, 0.1f, 1000.0f);
}

void Camera::update()
{
	switch (state)
	{
	case cameraNumber::MONITORING:
		MonitoringCam();
		break;

	case cameraNumber::RELATIVE_POS:
		RelativePosCam();
		break;

	case cameraNumber::TPS:
		TPSCam();
		break;

	case cameraNumber::TRACK:
		TrackCam();
		break;

	case cameraNumber::FPS:
		FPSCam();
		break;
	}
}

void Camera::MonitoringCam()
{
}

void Camera::RelativePosCam()
{
}

void Camera::TrackCam()
{
}

void Camera::TPSCam()
{
}

void Camera::FPSCam()
{
}

void Camera::setPosition(DirectX::XMFLOAT3 _pos)
{
	position = _pos;
}

DirectX::XMMATRIX Camera::SetOrthographicMatrix(float _w, float _h, float _zNear, float _zFar)
{
	projection = DirectX::XMMatrixOrthographicLH(_w, _h, _zNear, _zFar);
	return projection;
}

DirectX::XMMATRIX Camera::SetPerspectiveMatrix(float _fov, float _aspect, float _zNear, float _zFar)
{
	projection = DirectX::XMMatrixPerspectiveFovLH(_fov, _aspect, _zNear, _zFar);
	return projection;
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	DirectX::XMVECTOR p, t, up;

	p = DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f);
	t = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	up = DirectX::XMVectorSet(0.0f, 10.0f, 0.0f, 1.0f);
	return DirectX::XMMatrixLookAtLH(p, t, up);
}

DirectX::XMMATRIX Camera::GetLightViewMatrix(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _target)
{
	DirectX::XMVECTOR p, t, up;

	p = DirectX::XMVectorSet(_pos.x, _pos.y, _pos.z, 1.0f);
	t = DirectX::XMVectorSet(_target.x,_target.y,_target.z, 1.0f);
	up = DirectX::XMVectorSet(0.0f, 10.0f, 0.0f, 1.0f);
	return DirectX::XMMatrixLookAtLH(p, t, up);
}

DirectX::XMMATRIX Camera::GetLightProjectionMatrix()
{
	return DirectX::XMMatrixOrthographicLH(pSystem->SCREEN_WIDTH/15, pSystem->SCREEN_HEIGHT/15, 0.1f, 1000.0f);
}
