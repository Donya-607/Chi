#include "Camera.h"
#include "system.h"

Camera::Camera()
{
	position = DirectX::XMFLOAT3(0.0f, 300.0f, -300.0f);
	target = DirectX::XMFLOAT3(.0f, .0f, .0f);
	state = cameraNumber::MONITORING;
	fov = DirectX::XMConvertToRadians(30.0f);
	aspect = static_cast<float>(pSystem->SCREEN_WIDTH) / static_cast<float>(pSystem->SCREEN_HEIGHT);
	near_panel = 0.1f;
	far_panel = 100000.0f;
	timer = 0;
	shakepos = { 0,0 };
	shakeSpeed = { 0,0 };
	shake_power = 0;
	srand((unsigned int)time(NULL));
}

void Camera::update(float elapsed_time)
{
	if (timer <= 0)
	{
		timer = 0;
		shakepos = { 0,0 };
		return;
	}
	timer -= elapsed_time;

	if (shakepos.x < -shake_power || shakepos.x > shake_power)
	{
		float add = shake_power / (1 + rand() % 20);
		if (rand()%2)
			add *= -1.0f;
		shakeSpeed.x += add;
		shakeSpeed.x *= -1;
	}
	if (shakepos.y < -shake_power || shakepos.y > shake_power)
	{
		float add = shake_power / (1 + rand() % 20);
		if (rand()%2)
			add *= -1.0f;
		shakeSpeed.y += add;
		shakeSpeed.y *= -1;
	}
	shakepos.x += shakeSpeed.x;
	shakepos.y += shakeSpeed.y;

	//if (shakepos.x < -shake_power)
	//	shakepos.x = -shake_power;

	//if (shakepos.x > shake_power)
	//	shakepos.x = shake_power;

	//if (shakepos.y < -shake_power)
	//	shakepos.y = -shake_power;

	//if (shakepos.y > shake_power)
	//	shakepos.y = shake_power;

}

void Camera::startShake(float _shake_power, float _time)
{
	shake_power = _shake_power;
	timer = _time;
	if (rand() % 100 < 50)
		shakeSpeed.x = shake_power / 10;
	else
		shakeSpeed.x = -shake_power / 10;

	if (rand() % 200 < 100)
		shakeSpeed.y = shake_power / 10;
	else
		shakeSpeed.y = -shake_power / 10;


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
	DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(_w, _h, _zNear, _zFar);
	return projection;
}

DirectX::XMMATRIX Camera::SetPerspectiveMatrix(float _fov, float _aspect, float _zNear, float _zFar)
{
	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(_fov, _aspect, _zNear, _zFar);
	return projection;
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	DirectX::XMVECTOR p, t, up;

	p = DirectX::XMVectorSet(position.x + shakepos.x, position.y + shakepos.y, position.z, 1.0f);
	t = DirectX::XMVectorSet(target.x+shakepos.x, target.y+shakepos.y, target.z, 1.0f);
	up = DirectX::XMVectorSet(0.0f, 10.0f, 0.0f, 1.0f);
	return DirectX::XMMatrixLookAtLH(p, t, up);
}

DirectX::XMMATRIX Camera::GetLightViewMatrix(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _target)
{
	DirectX::XMVECTOR p, t, up;

	p = DirectX::XMVectorSet(_pos.x, _pos.y, _pos.z, 1.0f);
	t = DirectX::XMVectorSet(_target.x, _target.y, _target.z, 1.0f);
	up = DirectX::XMVectorSet(0.0f, 10.0f, 0.0f, 1.0f);
	return DirectX::XMMatrixLookAtLH(p, t, up);
}

DirectX::XMMATRIX Camera::GetLightProjectionMatrix()
{
	return DirectX::XMMatrixOrthographicLH(pSystem->SCREEN_WIDTH / 15.0f, pSystem->SCREEN_HEIGHT / 15.0f, 0.1f, 1000.0f);
}
