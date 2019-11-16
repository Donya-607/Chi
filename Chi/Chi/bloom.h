#pragma once
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <d3d11.h>
#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include <map>
#include <string>
#include "misc.h"
#include "resourceManager.h"

class bloom
{
	ID3D11VertexShader* m_vertex;
	ID3D11PixelShader* m_pixel;
	ID3D11PixelShader* m_first_pixel;
	ID3D11VertexShader* m_vertex_SRV;
	ID3D11PixelShader* m_pixel_SRV;
	ID3D11InputLayout* m_input;
	ID3D11Buffer* m_buffer;
	ID3D11RasterizerState* m_rasterize;
	ID3D11ShaderResourceView* m_shader;
	D3D11_TEXTURE2D_DESC m_tex2dDesc;
	ID3D11SamplerState* m_sampleState;
	ID3D11DepthStencilState* m_depth;
	ID3D11Buffer* constant_buffer;

	struct vertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	struct cbuffer
	{
		float screenWidth;
		float screenHeight;
		float blurValue;
		float dummy;
	};

public:
	bloom() {}
	~bloom() 
	{
		ResourceManager::ReleasePixelShader(m_pixel);
		ResourceManager::ReleasePixelShader(m_pixel_SRV);
	}
	void init(ID3D11Device* _device);
	void firstRender(ID3D11DeviceContext* _dContext, ID3D11ShaderResourceView** _SRV)const;
	void Render(ID3D11DeviceContext* _dContext, ID3D11ShaderResourceView** _SRV,
		float _blur_value) const;
	void createSRV(ID3D11DeviceContext* _dContext, ID3D11ShaderResourceView** _SRV,
		DirectX::XMFLOAT4 _judge_color);

};