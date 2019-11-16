#include "bloom.h"

void bloom::init(ID3D11Device* _device)
{
	HRESULT hr = S_OK;

	vertex vertices[] =
	{
		{ DirectX::XMFLOAT4(-0.5,+0.5,0,1.0),DirectX::XMFLOAT4(1,1,1,1),DirectX::XMFLOAT2(0,0) },
		{ DirectX::XMFLOAT4(+0.5,+0.5,0,1.0),DirectX::XMFLOAT4(1,0,0,1),DirectX::XMFLOAT2(1,0) },
		{ DirectX::XMFLOAT4(-0.5,-0.5,0,1.0),DirectX::XMFLOAT4(0,1,0,1),DirectX::XMFLOAT2(0,1) },
		{ DirectX::XMFLOAT4(+0.5,-0.5,0,1.0),DirectX::XMFLOAT4(0,0,1,1),DirectX::XMFLOAT2(1,1) },
	};

	D3D11_BUFFER_DESC i_buffer = {};
	i_buffer.ByteWidth = sizeof(vertices);
	i_buffer.Usage = D3D11_USAGE_DYNAMIC; // UNIT.03
	i_buffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	i_buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // UNIT.03
	i_buffer.MiscFlags = 0;
	i_buffer.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA i_subresouce = {};
	i_subresouce.pSysMem = vertices;
	i_subresouce.SysMemPitch = 0; //Not use for vertex buffers.
	i_subresouce.SysMemSlicePitch = 0; //Not use for vertex buffers.
	hr = _device->CreateBuffer(&i_buffer, &i_subresouce, &m_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	_ASSERT_EXPR(!m_input, L"'input_layout' must be uncreated.");
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(input_element_desc);


	//Vertex Shader
	if (!ResourceManager::LoadVertexShader(_device, "./Data/shader/bloom_vs.cso", input_element_desc, numElements, &m_vertex, &m_input))
	{
		assert(0 && "頂点シェーダーの読み込みに失敗");
		return;
	}

	//Pixel Shader
	if (!ResourceManager::LoadPixelShader(_device, "./Data/shader/bloom_ps.cso", &m_pixel))
	{
		assert(0 && "ピクセルシェーダーの作成に失敗");
		return;
	}

	if (!ResourceManager::LoadPixelShader(_device, "./Data/shader/bloom_first_ps.cso", &m_first_pixel))
	{
		assert(0 && "ピクセルシェーダーの作成に失敗");
		return;
	}

	if (!ResourceManager::LoadVertexShader(_device, "./Data/shader/sprite_vs.cso", input_element_desc, numElements, &m_vertex_SRV, &m_input))
	{
		assert(0 && "頂点シェーダーの読み込みに失敗");
		return;
	}

	//Pixel Shader
	if (!ResourceManager::LoadPixelShader(_device, "./Data/shader/bloomSRV.cso", &m_pixel_SRV))
	{
		assert(0 && "ピクセルシェーダーの作成に失敗");
		return;
	}

	D3D11_RASTERIZER_DESC i_rastrizer;
	i_rastrizer.FillMode = D3D11_FILL_SOLID;
	i_rastrizer.CullMode = D3D11_CULL_NONE;
	i_rastrizer.FrontCounterClockwise = FALSE;
	i_rastrizer.DepthBias = 0;
	i_rastrizer.DepthBiasClamp = 0;
	i_rastrizer.SlopeScaledDepthBias = 0;
	i_rastrizer.DepthClipEnable = FALSE;
	i_rastrizer.ScissorEnable = FALSE;
	i_rastrizer.MultisampleEnable = FALSE;
	i_rastrizer.AntialiasedLineEnable = FALSE;
	_device->CreateRasterizerState(&i_rastrizer, &m_rasterize);


	DirectX::XMFLOAT4 dummy(0.0f, 0.0f, 0.0f, 0.0f);
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC; //UNIT.06
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(sampler_desc.BorderColor, &dummy, sizeof(DirectX::XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _device->CreateSamplerState(&sampler_desc, &m_sampleState);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//Zバッファの設定(ここをいじると描画周りがいじれる)
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	//Zバッファのオンオフ(TRUE : on,FALSE : off)
	depth_stencil_desc.DepthEnable = FALSE;
	//Zバッファへの書き込み(ZERO : off,ALL : on)
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//Zテストの条件
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = _device->CreateDepthStencilState(&depth_stencil_desc, &m_depth);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_BUFFER_DESC constantBuffer_desc;
	constantBuffer_desc.ByteWidth = sizeof(cbuffer);
	constantBuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constantBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBuffer_desc.CPUAccessFlags = 0;
	constantBuffer_desc.MiscFlags = 0;
	constantBuffer_desc.StructureByteStride = 0;

	hr = _device->CreateBuffer(&constantBuffer_desc, nullptr, &constant_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void bloom::firstRender(ID3D11DeviceContext* _dContext, ID3D11ShaderResourceView** _SRV) const
{
	UINT stride = sizeof(vertex);
	UINT offset = 0;

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	_dContext->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// 左上
	float x0 = 0;
	float y0 = 0;
	// 右上
	float x1 = screen_width;
	float y1 = 0;
	// 左下
	float x2 = 0;
	float y2 = screen_height;
	// 右下
	float x3 = screen_width;
	float y3 = screen_height;

	// NDC座標に書き換える
	x0 = 2.0f * x0 / screen_width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / screen_height;
	x1 = 2.0f * x1 / screen_width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / screen_height;
	x2 = 2.0f * x2 / screen_width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / screen_height;
	x3 = 2.0f * x3 / screen_width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / screen_height;


	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = _dContext->Map(m_buffer, 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices = static_cast<vertex*>(mapped_buffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[2].position.x = x2;
	vertices[2].position.y = y2;
	vertices[3].position.x = x3;
	vertices[3].position.y = y3;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;
	vertices[0].position.w = vertices[1].position.w = vertices[2].position.w = vertices[3].position.w = 1.0f;

	vertices[0].texcoord.x = 0;
	vertices[1].texcoord.x = 1;
	vertices[2].texcoord.x = 0;
	vertices[3].texcoord.x = 1;

	vertices[0].texcoord.y = 0;
	vertices[1].texcoord.y = 0;
	vertices[2].texcoord.y = 1;
	vertices[3].texcoord.y = 1;


	DirectX::XMFLOAT4 color = {1,1,1,1};
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

	_dContext->Unmap(m_buffer, 0);
	_dContext->IASetVertexBuffers(0, 1, &m_buffer, &stride, &offset);

	cbuffer cb;
	cb.blurValue = 0;
	cb.screenWidth = screen_width;
	cb.screenHeight = screen_height;
	_dContext->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	_dContext->VSSetConstantBuffers(0, 1, &constant_buffer);
	_dContext->PSSetConstantBuffers(0, 1, &constant_buffer);



	_dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	_dContext->IASetInputLayout(m_input);

	_dContext->RSSetState(m_rasterize);

	_dContext->VSSetShader(m_vertex, nullptr, 0);

	_dContext->PSSetShader(m_first_pixel, nullptr, 0);

	_dContext->PSSetShaderResources(0, 1, _SRV);
	_dContext->PSSetSamplers(0, 1, &m_sampleState);

	_dContext->OMSetDepthStencilState(m_depth, 1);

	_dContext->Draw(4, 0);
}

void bloom::Render(ID3D11DeviceContext* _dContext,ID3D11ShaderResourceView** _SRV, float _blur_value) const
{
	UINT stride = sizeof(vertex);
	UINT offset = 0;

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	_dContext->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// 左上
	float x0 = 0;
	float y0 = 0;
	// 右上
	float x1 = screen_width;
	float y1 = 0;
	// 左下
	float x2 = 0;
	float y2 = screen_height;
	// 右下
	float x3 = screen_width;
	float y3 = screen_height;

	// NDC座標に書き換える
	x0 = 2.0f * x0 / screen_width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / screen_height;
	x1 = 2.0f * x1 / screen_width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / screen_height;
	x2 = 2.0f * x2 / screen_width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / screen_height;
	x3 = 2.0f * x3 / screen_width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / screen_height;


	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = _dContext->Map(m_buffer, 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices = static_cast<vertex*>(mapped_buffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[2].position.x = x2;
	vertices[2].position.y = y2;
	vertices[3].position.x = x3;
	vertices[3].position.y = y3;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;
	vertices[0].position.w = vertices[1].position.w = vertices[2].position.w = vertices[3].position.w = 1.0f;

	vertices[0].texcoord.x = 0;
	vertices[1].texcoord.x = 1;
	vertices[2].texcoord.x = 0;
	vertices[3].texcoord.x = 1;

	vertices[0].texcoord.y = 0;
	vertices[1].texcoord.y = 0;
	vertices[2].texcoord.y = 1;
	vertices[3].texcoord.y = 1;


	DirectX::XMFLOAT4 color = {1,1,1,1};
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;
	
	_dContext->Unmap(m_buffer, 0);
	cbuffer cb;
	cb.blurValue = _blur_value;
	cb.screenWidth = screen_width;
	cb.screenHeight = screen_height;
	_dContext->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	_dContext->VSSetConstantBuffers(0, 1, &constant_buffer);
	_dContext->PSSetConstantBuffers(0, 1, &constant_buffer);


	_dContext->IASetVertexBuffers(0, 1, &m_buffer, &stride, &offset);

	_dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	_dContext->IASetInputLayout(m_input);

	_dContext->RSSetState(m_rasterize);

	_dContext->VSSetShader(m_vertex, nullptr, 0);

	_dContext->PSSetShader(m_pixel, nullptr, 0);

	_dContext->PSSetShaderResources(0, 1, _SRV);
	_dContext->PSSetSamplers(0, 1, &m_sampleState);

	_dContext->OMSetDepthStencilState(m_depth, 1);

	_dContext->Draw(4, 0);

}

void bloom::createSRV(ID3D11DeviceContext* _dContext, ID3D11ShaderResourceView** _SRV, DirectX::XMFLOAT4 _judge_color)
{
	UINT stride = sizeof(vertex);
	UINT offset = 0;

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	_dContext->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// 左上
	float x0 = 0;
	float y0 = 0;
	// 右上
	float x1 = screen_width;
	float y1 = 0;
	// 左下
	float x2 = 0;
	float y2 = screen_height;
	// 右下
	float x3 = screen_width;
	float y3 = screen_height;

	// NDC座標に書き換える
	x0 = 2.0f * x0 / screen_width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / screen_height;
	x1 = 2.0f * x1 / screen_width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / screen_height;
	x2 = 2.0f * x2 / screen_width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / screen_height;
	x3 = 2.0f * x3 / screen_width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / screen_height;


	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = _dContext->Map(m_buffer, 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices = static_cast<vertex*>(mapped_buffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[2].position.x = x2;
	vertices[2].position.y = y2;
	vertices[3].position.x = x3;
	vertices[3].position.y = y3;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;
	vertices[0].position.w = vertices[1].position.w = vertices[2].position.w = vertices[3].position.w = 1.0f;

	vertices[0].texcoord.x = 0;
	vertices[1].texcoord.x = 1;
	vertices[2].texcoord.x = 0;
	vertices[3].texcoord.x = 1;

	vertices[0].texcoord.y = 0;
	vertices[1].texcoord.y = 0;
	vertices[2].texcoord.y = 1;
	vertices[3].texcoord.y = 1;


	DirectX::XMFLOAT4 color = _judge_color;
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;


	_dContext->Unmap(m_buffer, 0);


	_dContext->IASetVertexBuffers(0, 1, &m_buffer, &stride, &offset);

	_dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	_dContext->IASetInputLayout(m_input);

	_dContext->RSSetState(m_rasterize);

	_dContext->VSSetShader(m_vertex_SRV, nullptr, 0);

	_dContext->PSSetShader(m_pixel_SRV, nullptr, 0);

	_dContext->PSSetShaderResources(0, 1, _SRV);
	_dContext->PSSetSamplers(0, 1, &m_sampleState);

	_dContext->OMSetDepthStencilState(m_depth, 1);

	_dContext->Draw(4, 0);

}
