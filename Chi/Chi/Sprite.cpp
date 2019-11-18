#include "Sprite.h"
#include "util.h"

void Sprite::RoadSprite(ID3D11Device* _device, const wchar_t* _fileName)
{
	HRESULT hr = S_OK;

	vertex vertices[] =
	{
		{ DirectX::XMFLOAT4(-0.5,+0.5,0,1.0),DirectX::XMFLOAT4(1,1,1,1) },
		{ DirectX::XMFLOAT4(+0.5,+0.5,0,1.0),DirectX::XMFLOAT4(1,0,0,1) },
		{ DirectX::XMFLOAT4(-0.5,-0.5,0,1.0),DirectX::XMFLOAT4(0,1,0,1) },
		{ DirectX::XMFLOAT4(+0.5,-0.5,0,1.0),DirectX::XMFLOAT4(0,0,1,1) },
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
	if (!ResourceManager::LoadVertexShader(_device, "./Data/shader/sprite_vs.cso", input_element_desc, numElements, &m_vertex, &m_input))
	{
		assert(0 && "頂点シェーダーの読み込みに失敗");
		return;
	}

	//Pixel Shader
	if (!ResourceManager::LoadPixelShader(_device, "./Data/shader/sprite_ps.cso", &m_pixel))
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

	if (!ResourceManager::LoadShaderResourceView(_device, _fileName, &m_shader, &m_tex2dDesc))
	{
		assert(0 && "テクスチャ画像読み込み失敗");
		return;
	}
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

	size = { (float)m_tex2dDesc.Width ,(float)m_tex2dDesc.Height };
}

void Sprite::Render(ID3D11DeviceContext* _dContext, float _dx, float _dy, float _dw, float _dh,
	float _sx, float _sy, float _sw, float _sh, float _angle, float _r, float _g, float _b, float _a, float _cx, float _cy,
	bool _flipX, bool _flipY) const
{
	UINT stride = sizeof(vertex);
	UINT offset = 0;

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	_dContext->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	vector2 rate = { (_dw) / (_sw),(_dh) / (_sh) };
	// Set each sprite's vertices coordinate to screen spaceenum BLEND_STATE
	// 左上
	float x0 = _dx - _cx * rate.x;
	float y0 = _dy - _cy * rate.y;
	// 右上
	float x1 = _dx + _dw - _cx * rate.x;
	float y1 = _dy - _cy * rate.y;
	// 左下
	float x2 = _dx - _cx * rate.x;
	float y2 = _dy + _dh - _cy * rate.y;
	// 右下
	float x3 = _dx + _dw - _cx * rate.x;
	float y3 = _dy + _dh - _cy * rate.y;

	// 回転、拡大のために中心をセットする
	float mx = _dx + _cx;
	float my = _dy + _cy;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;
	x2 -= mx;
	y2 -= my;
	x3 -= mx;
	y3 -= my;

	// 画像の回転
	float rx, ry;
	float cos = cosf(_angle * 0.01745f);
	float sin = sinf(_angle * 0.01745f);
	rx = x0;
	ry = y0;
	x0 = cos * rx + -sin * ry;
	y0 = sin * rx + cos * ry;
	rx = x1;
	ry = y1;
	x1 = cos * rx + -sin * ry;
	y1 = sin * rx + cos * ry;
	rx = x2;
	ry = y2;
	x2 = cos * rx + -sin * ry;
	y2 = sin * rx + cos * ry;
	rx = x3;
	ry = y3;
	x3 = cos * rx + -sin * ry;
	y3 = sin * rx + cos * ry;

	// Translate sprite's centre to original position
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;
	x2 += mx;
	y2 += my;
	x3 += mx;
	y3 += my;

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


	if (_flipX)
	{
		vertices[1].texcoord.x = static_cast<FLOAT>(_sx) / m_tex2dDesc.Width;
		vertices[0].texcoord.x = static_cast<FLOAT>(_sx + _sw) / m_tex2dDesc.Width;
		vertices[3].texcoord.x = static_cast<FLOAT>(_sx) / m_tex2dDesc.Width;
		vertices[2].texcoord.x = static_cast<FLOAT>(_sx + _sw) / m_tex2dDesc.Width;

	}
	else
	{
		vertices[0].texcoord.x = static_cast<FLOAT>(_sx) / m_tex2dDesc.Width;
		vertices[1].texcoord.x = static_cast<FLOAT>(_sx + _sw) / m_tex2dDesc.Width;
		vertices[2].texcoord.x = static_cast<FLOAT>(_sx) / m_tex2dDesc.Width;
		vertices[3].texcoord.x = static_cast<FLOAT>(_sx + _sw) / m_tex2dDesc.Width;

	}
	if (_flipY)
	{
		vertices[2].texcoord.y = static_cast<FLOAT>(_sy) / m_tex2dDesc.Height;
		vertices[3].texcoord.y = static_cast<FLOAT>(_sy) / m_tex2dDesc.Height;
		vertices[0].texcoord.y = static_cast<FLOAT>(_sy + _sh) / m_tex2dDesc.Height;
		vertices[1].texcoord.y = static_cast<FLOAT>(_sy + _sh) / m_tex2dDesc.Height;

	}
	else
	{
		vertices[0].texcoord.y = static_cast<FLOAT>(_sy) / m_tex2dDesc.Height;
		vertices[1].texcoord.y = static_cast<FLOAT>(_sy) / m_tex2dDesc.Height;
		vertices[2].texcoord.y = static_cast<FLOAT>(_sy + _sh) / m_tex2dDesc.Height;
		vertices[3].texcoord.y = static_cast<FLOAT>(_sy + _sh) / m_tex2dDesc.Height;

	}

	DirectX::XMFLOAT4 color(_r, _g, _b, _a);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;


	_dContext->Unmap(m_buffer, 0);


	_dContext->IASetVertexBuffers(0, 1, &m_buffer, &stride, &offset);

	_dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	_dContext->IASetInputLayout(m_input);

	_dContext->RSSetState(m_rasterize);

	_dContext->VSSetShader(m_vertex, nullptr, 0);

	_dContext->PSSetShader(m_pixel, nullptr, 0);

	_dContext->PSSetShaderResources(0, 1, &m_shader);
	_dContext->PSSetSamplers(0, 1, &m_sampleState);

	_dContext->OMSetDepthStencilState(m_depth, 1);



	_dContext->Draw(4, 0);
}

void Sprite::textout(ID3D11DeviceContext* _dContext, std::string _string, float _x, float _y, float _w, float _h, float _r, float _g, float _b, float _a) const
{
	float sw = static_cast<float>(m_tex2dDesc.Width / 16);
	float sh = static_cast<float>(m_tex2dDesc.Height / 16);
	float cursor = 0;
	for (auto c : _string)
	{
		LONG sx = c % 0x0F;
		Render(_dContext, _x + cursor, _y, _w, _h, sw * (c & 0x0F), sh * (c >> 4), sw, sh, 0, _r, _g, _b, _a);
		cursor += _w;
	}


}

sprite_batch::sprite_batch(ID3D11Device* device, const wchar_t* file_name, size_t max_instance) : MAX_INSTANCES(max_instance)
{
	HRESULT hr = S_OK;

	vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT2(1, 0) },
		{ DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT2(0, 1) },
		{ DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 1) },
	};

	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
	subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NDC_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT numElements = ARRAYSIZE(input_element_desc);


	if (!ResourceManager::LoadVertexShader(device, "./Data/shader/sprite_vs.cso", input_element_desc, numElements, &vertex_shader, &input_layout))
	{
		assert(0 && "頂点シェーダーの読み込みに失敗");
		return;
	}

	if (!ResourceManager::LoadPixelShader(device, "./Data/shader/sprite_ps.cso", &pixel_shader))
	{
		assert(0 && "ピクセルシェーダーの作成に失敗");
		return;
	}

	//Vertex Shader
	{
		//	//csoファイルのロード(ファイル操作)

		//	FILE* fp = 0;
		//	fopen_s(&fp, "sprite_vs.cso", "rb");
		//	_ASSERT_EXPR_A(fp, "CSO File not found");
		//	fseek(fp, 0, SEEK_END);
		//	long cso_sz = ftell(fp);
		//	fseek(fp, 0, SEEK_SET);
		//	unsigned char* cso_data = new unsigned char[cso_sz];
		//	fread(cso_data, cso_sz, 1, fp);
		//	fclose(fp);

		//	hr = device->CreateVertexShader(cso_data, cso_sz, nullptr, &vertex_shader);
		//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		//	hr = device->CreateInputLayout(input_element_desc, ARRAYSIZE(input_element_desc), cso_data, cso_sz, &input_layout);
		//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//	delete[] cso_data;
	}

	//Pixel Shader
	{
		////csoファイルのロード(ファイル操作)
		//FILE* fp;
		//fopen_s(&fp, "sprite_ps.cso", "rb");
		//_ASSERT_EXPR_A(fp, "CSO File not found");
		//fseek(fp, 0, SEEK_END);
		//long cso_sz = ftell(fp);
		//fseek(fp, 0, SEEK_SET);
		//unsigned char* cso_data = new unsigned char[cso_sz];
		//fread(cso_data, cso_sz, 1, fp);
		//fclose(fp);

		//hr = device->CreatePixelShader(cso_data, cso_sz, nullptr, &pixel_shader);
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//delete[] cso_data;
	}

	instance* instances = new instance[MAX_INSTANCES];
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(instance) * MAX_INSTANCES;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = instances;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &instance_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	delete[] instances;

	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizer_desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = FALSE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (!ResourceManager::LoadShaderResourceView(device, file_name, &shader_resource_view, &texture2d_desc))
	{
		assert(0 && "テクスチャ画像読み込み失敗");
		return;
	}
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
	hr = device->CreateSamplerState(&sampler_desc, &sampler_state);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
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
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void sprite_batch::begin(ID3D11DeviceContext* immediate_context)
{
	HRESULT hr = S_OK;

	UINT strides[2] = { sizeof(vertex), sizeof(instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* vbs[2] = { vertex_buffer, instance_buffer };
	immediate_context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(input_layout);
	immediate_context->OMSetDepthStencilState(depth_stencil_state, 1);
	immediate_context->RSSetState(rasterizer_state);
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);
	immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);
	immediate_context->PSSetSamplers(0, 1, &sampler_state);

	UINT num_viewports = 1;
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = immediate_context->Map(instance_buffer, 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	instances = static_cast<instance*>(mapped_buffer.pData);

	count_instance = 0;
}
void sprite_batch::render(float dx, float dy, float dw, float dh,
	float sx, float sy, float sw, float sh, float angle/*degree*/, float r, float g, float b, float a)
{
	_ASSERT_EXPR(count_instance < MAX_INSTANCES, L"Number of instances must be less than MAX_INSTANCES.");

	float cx = dw * 0.5f, cy = dh * 0.5f; /*Center of Rotation*/
#if 0
	DirectX::XMVECTOR scaling = DirectX::XMVectorSet(dw, dh, 1.0f, 0.0f);
	DirectX::XMVECTOR origin = DirectX::XMVectorSet(cx, cy, 0.0f, 0.0f);
	DirectX::XMVECTOR translation = DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f);
	DirectX::XMMATRIX M = DirectX::XMMatrixAffineTransformation2D(scaling, origin, angle * 0.01745f, translation);
	DirectX::XMMATRIX N(
		2.0f / viewport.Width, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / viewport.Height, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&instances[count_instance].ndc_transform, DirectX::XMMatrixTranspose(M * N)); //column_major
#else
	FLOAT c = cosf(angle * 0.01745f);
	FLOAT s = sinf(angle * 0.01745f);
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;
	instances[count_instance].ndc_transform._11 = w * dw * c;
	instances[count_instance].ndc_transform._21 = h * dw * s;
	instances[count_instance].ndc_transform._31 = 0.0f;
	instances[count_instance].ndc_transform._41 = 0.0f;
	instances[count_instance].ndc_transform._12 = w * dh * -s;
	instances[count_instance].ndc_transform._22 = h * dh * c;
	instances[count_instance].ndc_transform._32 = 0.0f;
	instances[count_instance].ndc_transform._42 = 0.0f;
	instances[count_instance].ndc_transform._13 = 0.0f;
	instances[count_instance].ndc_transform._23 = 0.0f;
	instances[count_instance].ndc_transform._33 = 1.0f;
	instances[count_instance].ndc_transform._43 = 0.0f;
	instances[count_instance].ndc_transform._14 = w * (-cx * c + -cy * -s + cx + dx) - 1.0f;
	instances[count_instance].ndc_transform._24 = h * (-cx * s + -cy * c + cy + dy) + 1.0f;
	instances[count_instance].ndc_transform._34 = 0.0f;
	instances[count_instance].ndc_transform._44 = 1.0f;
#endif
	float tw = static_cast<float>(texture2d_desc.Width);
	float th = static_cast<float>(texture2d_desc.Height);
	instances[count_instance].texcoord_transform = DirectX::XMFLOAT4(sx / tw, sy / th, sw / tw, sh / th);
	instances[count_instance].color = DirectX::XMFLOAT4(r, g, b, a);

	count_instance++;
}

float sprite_batch::textout(std::string s,
	float x, float y, float sx, float sy,
	float r, float g, float b, float a)
{
	float tw = static_cast<float>(texture2d_desc.Width / 16.0);
	float th = static_cast<float>(texture2d_desc.Height / 16.0);
	float cursor = 0.0f;
	for (const auto& c : s)
	{
		render(x, y, sx, sy, tw * (c & 0x0F), th * (c >> 4),
			tw, th, 0, r, g, b, a);
		cursor += tw * sx;
	}
	return th * sy;
}

void sprite_batch::end(ID3D11DeviceContext* immediate_context)
{
	immediate_context->Unmap(instance_buffer, 0);

	immediate_context->DrawInstanced(4, count_instance, 0, 0);
}






//スプライトバッチ　もらってきたやつ

//==========================================================================
//
//      SpriteBatchクラス
//
//==========================================================================

//--------------------------------
//  コンストラクタ
//--------------------------------
SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* fileName, size_t maxInstance)
{
	MAX_INSTANCES = maxInstance;

	//VertexBufferの作成
	vertex vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT2(1, 0) },
		{ DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT2(0, 1) },
		{ DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 1) },
	};
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0; //Not use for vertex buffers.
	subresourceData.SysMemSlicePitch = 0; //Not use for vertex buffers.
	if (FAILED(device->CreateBuffer(&bufferDesc, &subresourceData, &buffer)))
	{
		assert(!"頂点バッファの作成に失敗(SpriteBatch)");
		return;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT numElements = ARRAYSIZE(layout);
	if (!ResourceManager::LoadVertexShader(device, "./Data/shader/sprite_batch_vs.cso", layout, numElements, &vertexShader, &inputLayout))
	{
		assert(!"頂点シェーダーの読み込みに失敗(SpriteBatch)");
		return;
	}
	if (!ResourceManager::LoadPixelShader(device, "./Data/shader/sprite_batch_ps.cso", &pixelShader))
	{
		assert(!"ピクセルシェーダーの作成に失敗(SpriteBatch)");
		return;
	}

	instance* inst = new instance[MAX_INSTANCES];
	{
		D3D11_BUFFER_DESC bd = {};
		D3D11_SUBRESOURCE_DATA sd = {};

		bd.ByteWidth = sizeof(instance) * MAX_INSTANCES;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		sd.pSysMem = inst;
		sd.SysMemPitch = 0; //Not use for vertex buffers.mm 
		sd.SysMemSlicePitch = 0; //Not use for vertex buffers.
		if (FAILED(device->CreateBuffer(&bd, &sd, &instanceBuffer)))
		{
			assert(!"バッファの生成に失敗(SpriteBatch)");
			return;
		}
	}
	delete[] inst;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	if (FAILED(device->CreateRasterizerState(&rasterizerDesc, &rasterizerState)))
	{
		assert(!"ラスタライザステートの作成に失敗(SpriteBatch)");
		return;
	}

	if (!ResourceManager::LoadShaderResourceView(device, fileName, &shaderResourceView, &tex2dDesc))
	{
		assert(!"テクスチャ画像読み込み失敗(SpriteBatch)");
		return;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	for (int i = 0; i < 4; i++) samplerDesc.BorderColor[i] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(device->CreateSamplerState(&samplerDesc, &samplerState)))
	{
		assert(!"サンプラーステートの生成に失敗(SpriteBatch)");
		return;
	}

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	if (FAILED(device->CreateDepthStencilState(&dsDesc, &depthStencilState)))
	{
		assert(!"デプスステンシルステートの作成に失敗(SpriteBatch)");
		return;
	}
}

//--------------------------------
//  デストラクタ
//--------------------------------
SpriteBatch::~SpriteBatch()
{
	safe_release(instanceBuffer);
	safe_release(depthStencilState);
	safe_release(samplerState);
	ResourceManager::ReleaseShaderResourceView(shaderResourceView);
	safe_release(rasterizerState);
	safe_release(buffer);
	ResourceManager::ReleasePixelShader(pixelShader);
	ResourceManager::ReleaseVertexShader(vertexShader, inputLayout);
}

//--------------------------------
//  前処理（描画前に1度呼ぶ）
//--------------------------------
void SpriteBatch::begin(ID3D11DeviceContext* context)
{
	HRESULT hr = S_OK;

	UINT strides[2] = { sizeof(vertex), sizeof(instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* vbs[2] = { buffer, instanceBuffer };
	context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(inputLayout);
	context->OMSetDepthStencilState(depthStencilState, 1);
	context->RSSetState(rasterizerState);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
	context->PSSetShaderResources(0, 1, &shaderResourceView);
	context->PSSetSamplers(0, 1, &samplerState);

	UINT numViewports = 1;
	context->RSGetViewports(&numViewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	hr = context->Map(instanceBuffer, 0, map, 0, &mappedBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	instances = static_cast<instance*>(mappedBuffer.pData);

	instanceCount = 0;
}

//--------------------------------
//  描画準備
//--------------------------------
void SpriteBatch::render(
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
	const DirectX::XMFLOAT2& center, float angle/*radian*/,
	const DirectX::XMFLOAT4& color)
{
	if (instanceCount >= MAX_INSTANCES)
	{
		assert(!"Number of instances must be less than MAX_INSTANCES.(SpriteBatch)");
		return;
	}

	if (scale.x * scale.y == 0.0f) return;
	float tw = texSize.x;
	float th = texSize.y;
	if (texSize.x <= 0.0f || texSize.y <= 0.0f)
	{
		tw = (float)tex2dDesc.Width;
		th = (float)tex2dDesc.Height;
	}

	//	float cx = dw*0.5f, cy = dh*0.5f; /*Center of Rotation*/
	float cx = center.x;
	float cy = center.y;
	cx *= scale.x;
	cy *= scale.y;

#if 0
	DirectX::XMDirectX::XMFLOAT scaling = DirectX::XMVectorSet(dw, dh, 1.0f, 0.0f);
	DirectX::XMDirectX::XMFLOAT origin = DirectX::XMVectorSet(cx, cy, 0.0f, 0.0f);
	DirectX::XMDirectX::XMFLOAT translation = DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f);
	DirectX::XMMATRIX M = DirectX::XMMatrixAffineTransformation2D(scaling, origin, angle * 0.01745f, translation);
	DirectX::XMMATRIX N(
		2.0f / viewport.Width, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / viewport.Height, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&instances[count_instance].ndc_transform, DirectX::XMMatrixTranspose(M * N)); //column_major
#else
	FLOAT c = cosf(angle);
	FLOAT s = sinf(angle);
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;

	instances[instanceCount].ndcTransform._11 = w * scale.x * tw * c;
	instances[instanceCount].ndcTransform._21 = h * scale.x * tw * s;
	instances[instanceCount].ndcTransform._31 = 0.0f;
	instances[instanceCount].ndcTransform._41 = 0.0f;
	instances[instanceCount].ndcTransform._12 = w * scale.y * th * -s;
	instances[instanceCount].ndcTransform._22 = h * scale.y * th * c;
	instances[instanceCount].ndcTransform._32 = 0.0f;
	instances[instanceCount].ndcTransform._42 = 0.0f;
	instances[instanceCount].ndcTransform._13 = 0.0f;
	instances[instanceCount].ndcTransform._23 = 0.0f;
	instances[instanceCount].ndcTransform._33 = 1.0f;
	instances[instanceCount].ndcTransform._43 = 0.0f;
	instances[instanceCount].ndcTransform._14 = w * (-cx * c + -cy * -s + cx * 0 + position.x) - 1.0f;
	instances[instanceCount].ndcTransform._24 = h * (-cx * s + -cy * c + cy * 0 + position.y) + 1.0f;
	instances[instanceCount].ndcTransform._34 = 0.0f;
	instances[instanceCount].ndcTransform._44 = 1.0f;
#endif
	float tex_width = static_cast<float>(tex2dDesc.Width);
	float tex_height = static_cast<float>(tex2dDesc.Height);
	instances[instanceCount].texcoordTransform = DirectX::XMFLOAT4(texPos.x / tex_width, texPos.y / tex_height, tw / tex_width, th / tex_height);
	instances[instanceCount].color = color;

	instanceCount++;
}

//--------------------------------
//  描画準備
//--------------------------------
void SpriteBatch::render(
	float x, float y, float sx, float sy,
	float tx, float ty, float tw, float th,
	float cx, float cy, float angle/*radian*/,
	float r, float g, float b, float a)
{
	render(DirectX::XMFLOAT2(x, y), DirectX::XMFLOAT2(sx, sy),
		DirectX::XMFLOAT2(tx, ty), DirectX::XMFLOAT2(tw, th),
		DirectX::XMFLOAT2(cx, cy), angle,
		DirectX::XMFLOAT4(r, g, b, a));
}

//--------------------------------
//  テキスト描画準備
//--------------------------------
float SpriteBatch::textout(std::wstring s,
	const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT4& color)
{
	float tw = static_cast<float>(tex2dDesc.Width / 16.0);
	float th = static_cast<float>(tex2dDesc.Height / 16.0);
	float cursor = 0.0f;
	for (const auto& c : s)
	{
		render(DirectX::XMFLOAT2(pos.x + cursor, pos.y), scale, DirectX::XMFLOAT2(tw * (c & 0x0F), th * (c >> 4)),
			DirectX::XMFLOAT2(tw, th), DirectX::XMFLOAT2(0, 0), 0, color);
		cursor += tw * scale.x;
	}
	return th * scale.y;
}

//--------------------------------
//  テキスト描画準備
//--------------------------------
float SpriteBatch::textout(std::wstring s,
	float x, float y, float sx, float sy,
	float r, float g, float b, float a)
{
	return textout(s, DirectX::XMFLOAT2(x, y), DirectX::XMFLOAT2(sx, sy), DirectX::XMFLOAT4(r, g, b, a));
}

//--------------------------------
//  後処理（描画準備後に1度呼ぶ）
//--------------------------------
void SpriteBatch::end(ID3D11DeviceContext* context)
{
	context->Unmap(instanceBuffer, 0);
	context->DrawInstanced(4, instanceCount, 0, 0);
}


