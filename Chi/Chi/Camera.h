#ifndef CAMERA_H_
#define CAMERA_H_

#include	<DirectXMath.h>

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
	DirectX::XMMATRIX projection;	//ìäâeçsóÒ

	Camera();
	void update();
	void MonitoringCam();
	void RelativePosCam();
	void TrackCam();
	void TPSCam();
	void FPSCam();
	void setPosition(DirectX::XMFLOAT3 _pos);
	DirectX::XMFLOAT4 getCameraPos() { return{ position.x,position.y,position.z,1.0 }; }
	DirectX::XMMATRIX SetOrthographicMatrix(float _w, float _h, float _zNear, float _zFar);
	DirectX::XMMATRIX SetPerspectiveMatrix(float _fov, float _aspect, float _zNear, float _zFar);
	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetProjectionMatrix() { return projection; }

	DirectX::XMMATRIX GetLightViewMatrix(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _direct);
	DirectX::XMMATRIX GetLightProjectionMatrix();

private:
	int state;
};


#endif // !CAMERA_H_
