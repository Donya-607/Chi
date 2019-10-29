#ifndef LIGHT_H_
#define LIGHT_H_

#include <d3d11.h>
#include <DirectXMath.h>

struct PointLight
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 attenuate;
};

struct LineLight
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 color;

};

class line_light
{
private:
	LineLight _line_light;
public:
	line_light()
	{
		_line_light.direction = { 1,1,-1,1 };
		_line_light.color = { 1,1,1,1 };
	}
	void setLineLight(const DirectX::XMFLOAT4& _lightDirection, const DirectX::XMFLOAT4& _lightColor)
	{
		_line_light.direction = _lightDirection;
		_line_light.color = _lightColor;
	}
	DirectX::XMFLOAT4 getLineLight()
	{
		return _line_light.direction;
	}
	DirectX::XMFLOAT4 getLightColor()
	{
		return _line_light.color;
	}
	LineLight& getInfo()
	{
		return _line_light;
	}
};

class point_light
{
private:
	int index;
	PointLight pointLight;

public:
	point_light()
	{
		index = 0;
		pointLight.pos = { .0f,.0f,.0f,.0f };
		pointLight.attenuate = { 0.5f,0.5f,0.5f,1.0f };
	}
	void setPointLight(const PointLight& info)
	{
		pointLight = info;
	}
	int getIndex()
	{
		return index;
	}

	void setIndex(int _index)
	{
		index = _index;
	}

	PointLight getInfo()
	{
		return pointLight;
	}
};

//class spot_light
//{
//
//};


#endif // !LIGHT_H_
