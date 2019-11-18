#ifndef CAMERA_H_
#define CAMERA_H_

#include	<DirectXMath.h>
#include <cstdlib>
#include <ctime>
class Camera
{
public:
	enum cameraNumber
	{
		MONITORING = 0,
		RELATIVE_POS,
		TRACK,
		TPS,
		FPS,
		end,
	};

	DirectX::XMFLOAT3 position;		//à íu
	DirectX::XMFLOAT3 target;		//íçéãì_
	float fov;
	float aspect;
	float near_panel;
	float far_panel;
	float shake_power;
	DirectX::XMFLOAT2 shakepos;
	DirectX::XMFLOAT2 shakeSpeed;
	float timer;

	Camera();
	void update(float elapsed_time);
	void startShake(float _shake_power, float _time);
	void MonitoringCam();
	void RelativePosCam();
	void TrackCam();
	void TPSCam();
	void FPSCam();
	void setPosition(DirectX::XMFLOAT3 _pos);
	void setTarget(DirectX::XMFLOAT3 _target)
	{
		target = _target;
	}
	DirectX::XMFLOAT4 getCameraPos() { return{ position.x,position.y,position.z,1.0 }; }
	DirectX::XMFLOAT4 getCamTarget() { return { target.x,target.y,target.z,1.0f }; } DirectX::XMMATRIX SetOrthographicMatrix(float _w, float _h, float _zNear, float _zFar);
	DirectX::XMMATRIX SetPerspectiveMatrix(float _fov, float _aspect, float _zNear, float _zFar);
	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetProjectionMatrix()
	{
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, near_panel, far_panel);
		return projection;
	}

	DirectX::XMMATRIX GetLightViewMatrix(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _direct);
	DirectX::XMMATRIX GetLightProjectionMatrix();

private:
	int state;
};


#endif // !CAMERA_H_
