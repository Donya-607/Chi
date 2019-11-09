#include "static_mesh.h"

static_mesh::~static_mesh()
{
	if (constant_buffer)
		constant_buffer->Release();
	if (index_buffer)
		index_buffer->Release();
	if (vertex_buffer)
		vertex_buffer->Release();
	if (depthStencilState)
		depthStencilState->Release();
	if (rasterizeFillOut)
		rasterizeFillOut->Release();
	if (rasterizeLine)
		rasterizeLine->Release();
	if (pixelShader)
		ResourceManager::ReleasePixelShader(pixelShader);
	if (vertexShader)
		ResourceManager::ReleaseVertexShader(vertexShader, layout);

}

void static_mesh::objInit(ID3D11Device* _device, const wchar_t* _objFileName, const std::wstring& _mtlFileName)
{

	std::vector<vertex> vertices;
	std::vector<u_int> indices;
	u_int current_index = 0;


	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texPos;
	std::wstring watch;

	std::wifstream fin(_objFileName);
	_ASSERT_EXPR(fin, L"'OBJ file not fuund.");


	wchar_t command[256];
	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"v"))
		{
			float x, y, z;
			fin >> x >> y >> z;
			positions.push_back(DirectX::XMFLOAT3(x, y, z));
			fin.ignore();
		}
		else if (0 == wcscmp(command, L"vt") && _mtlFileName.size())
		{
			float l, m;
			fin >> l >> m;
			texPos.push_back(DirectX::XMFLOAT2(l, 1 - m));
			fin.ignore();
		}
		else if (0 == wcscmp(command, L"vn"))
		{
			FLOAT i, j, k;
			fin >> i >> j >> k;
			normals.push_back(DirectX::XMFLOAT3(i, j, k));
			fin.ignore();
		}
		else if (0 == wcscmp(command, L"usemtl") && _mtlFileName.size())
		{
			if (!subsets.empty())
			{
				subsets.back().index_count = current_index - 1;
			}
			subsets.emplace_back();
			fin >> subsets.back().usemtl;
			subsets.back().index_start = current_index;
		}

		else if (0 == wcscmp(command, L"f"))
		{
			static u_int index = 0;
			for (u_int i = 0; i < 3; i++)
			{
				vertex vertex;
				u_int v, vt, vn;

				fin >> v;
				vertex.position = positions[v - 1];
				if (L'/' == fin.peek())
				{
					fin.ignore();
					//std::streampos fpos = fin.tellg();
					if (L'/' != fin.peek())
					{

						//	fin >> watch;
						//	fin.seekg(fpos);
						fin >> vt;
						if (_mtlFileName.size())
							vertex.texcoord = texPos[vt - 1];
					}

					if (L'/' == fin.peek())
					{
						fin.ignore();
						fin >> vn;
						vertex.normal = normals[vn - 1];
					}
				}
				vertices.push_back(vertex);
				indices.push_back(current_index++);
			}
			fin.ignore();
		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}
	fin.close();
	if (!subsets.empty())
	{
		subsets.back().index_count = current_index - 1;
	}

	createBuffer(_device, vertices.data(), (UINT)vertices.size(), indices.data(), (UINT)indices.size());

	if (!_mtlFileName.size())
		return;


	std::wstring directory(_mtlFileName.substr(0, _mtlFileName.find_last_of(L"/") + 1));

	std::wifstream file(_mtlFileName.c_str());
	_ASSERT_EXPR(file, L"'mtl file not fuund.");
	while (file)
	{
		file >> command;
		if (0 == wcscmp(command, L"newmtl"))
		{
			file.ignore();
			materials.emplace_back();
			file >> materials.back().newmtl;
			file.ignore();
		}
		else if (0 == wcscmp(command, L"Ka"))
		{
			file.ignore();
			file >> materials.back().Ka.x >> materials.back().Ka.y >> materials.back().Ka.z;
			file.ignore();
		}
		else if (0 == wcscmp(command, L"Kd"))
		{
			file.ignore();
			file >> materials.back().Kd.x >> materials.back().Kd.y >> materials.back().Kd.z;
			file.ignore();
		}
		else if (0 == wcscmp(command, L"Ks"))
		{
			file.ignore();
			file >> materials.back().Ks.x >> materials.back().Ks.y >> materials.back().Ks.z;
			file.ignore();
		}
		else if (0 == wcscmp(command, L"illum"))
		{
			file.ignore();
			file >> materials.back().illum;
			file.ignore();
		}
		else if (0 == wcscmp(command, L"map_Kd"))
		{

			file.ignore();
			file >> watch;
			materials.back().map_Kd += directory.c_str() + watch;
			file.ignore();
		}

		else
		{
			file.ignore(1024, L'\n');
		}
	}

	file.close();

}

void static_mesh::TextureInit(ID3D11Device* _device)
{
	HRESULT hr;
	for (auto& it : materials)
	{
		if (!ResourceManager::LoadShaderResourceView(_device, it.map_Kd.c_str(), &it.Resource, &it.tex2dDesc))
		{
			assert(0 && "テクスチャ画像読み込み失敗");
			return;
		}
	}
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC; //UNIT.06
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	DirectX::XMFLOAT4 dummy(0.0f, 0.0f, 0.0f, 0.0f);
	memcpy(sampler_desc.BorderColor, &dummy, sizeof(DirectX::XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _device->CreateSamplerState(&sampler_desc, &sampleState);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}


void static_mesh::loadStaticMesh(ID3D11Device* _device, const wchar_t* _objFileName)
{
	D3D11_INPUT_ELEMENT_DESC elements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(elements);
	std::string vsName;
	std::string psName;

	vsName = "./Data/shader/geometric_primitive_vs.cso";
	psName = "./Data/shader/geometric_phong_ps.cso";
	init(_device, vsName, elements, numElements, psName);

	objInit(_device, _objFileName, L"");
}

void static_mesh::loadStaticMeshMTL(ID3D11Device* _device, const wchar_t* _objFileName, const std::wstring& _mtlFileName)
{
	D3D11_INPUT_ELEMENT_DESC elements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(elements);
	std::string vsName;
	std::string psName;

	if (!_mtlFileName.empty())
	{
		vsName = "./Data/shader/obj_vs.cso";
		psName = "./Data/shader/geometric_phong_ps.cso";
	}
	else
	{
		vsName = "./Data/shader/geometric_primitive_vs.cso";
		psName = "./Data/shader/geometric_phong_ps.cso";
	}
	init(_device, vsName, elements, numElements, psName);

	objInit(_device, _objFileName, _mtlFileName);

	vsName = "./Data/shader/geometric_primitive_vs.cso";
	psName = "./Data/shader/geometric_phong_ps.cso";

	ResourceManager::LoadVertexShader(_device, vsName, elements, numElements, &noTexVS, &noTexLayout);
	ResourceManager::LoadPixelShader(_device, psName, &noTexPS);

	if (!materials.empty())
		TextureInit(_device);
}

void static_mesh::init(ID3D11Device* device, std::string vsName, D3D11_INPUT_ELEMENT_DESC* inputElementDescs, int numElement, std::string psName)
{
	HRESULT hr = S_OK;
	//vertexShader
	ResourceManager::LoadVertexShader(device, vsName, inputElementDescs, numElement, &vertexShader, &layout);
	//pixelShader
	ResourceManager::LoadPixelShader(device, psName, &pixelShader);
	//rasterizerLine
	D3D11_RASTERIZER_DESC line_desc;
	line_desc.FillMode = D3D11_FILL_WIREFRAME;	//塗りつぶし設定
	line_desc.CullMode = D3D11_CULL_BACK;		//カリング設定
	line_desc.FrontCounterClockwise = FALSE;	//頂点がどっち周りですか？
	line_desc.DepthBias = 0;
	line_desc.DepthBiasClamp = 0;
	line_desc.SlopeScaledDepthBias = 0;
	line_desc.DepthClipEnable = FALSE;
	line_desc.ScissorEnable = FALSE;
	line_desc.MultisampleEnable = FALSE;
	line_desc.AntialiasedLineEnable = FALSE;

	device->CreateRasterizerState(&line_desc, &rasterizeLine);

	D3D11_RASTERIZER_DESC fillOut_desc;
	fillOut_desc.FillMode = D3D11_FILL_SOLID;	//塗りつぶし設定
	fillOut_desc.CullMode = D3D11_CULL_BACK;		//カリング設定
	fillOut_desc.FrontCounterClockwise = FALSE;	//頂点がどっち周りですか？
	fillOut_desc.DepthBias = 0;
	fillOut_desc.DepthBiasClamp = 0;
	fillOut_desc.SlopeScaledDepthBias = 0;
	fillOut_desc.DepthClipEnable = FALSE;
	fillOut_desc.ScissorEnable = FALSE;
	fillOut_desc.MultisampleEnable = FALSE;
	fillOut_desc.AntialiasedLineEnable = FALSE;

	device->CreateRasterizerState(&fillOut_desc, &rasterizeFillOut);



	//Zバッファの設定(ここをいじると描画周りがいじれる)
	D3D11_DEPTH_STENCIL_DESC depthStencil_desc = {};
	//Zバッファのオンオフ(TRUE : on,FALSE : off)
	depthStencil_desc.DepthEnable = TRUE;
	//Zバッファへの書き込み(ZERO : off,ALL : on)
	depthStencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//Zテストの条件
	depthStencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencil_desc.StencilEnable = FALSE;
	//depthStencil_desc.StencilReadMask = 0xFF;
	//depthStencil_desc.StencilWriteMask = 0xFF;
	//depthStencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//depthStencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//depthStencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//depthStencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//depthStencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//depthStencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = device->CreateDepthStencilState(&depthStencil_desc, &depthStencilState);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


}

void static_mesh::createPrimitive(ID3D11Device* _device)
{
	D3D11_INPUT_ELEMENT_DESC input_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT numElements = ARRAYSIZE(input_desc);
	std::string vsName = "./Data/shader/geometric_primitive_vs.cso";
	std::string psName = "./Data/shader/geometric_phong_ps.cso";

	init(_device, vsName, input_desc, numElements, psName);
}

bool static_mesh::createBuffer(ID3D11Device* device, vertex* vertices, int numV, unsigned int* indices, int numI)
{
	HRESULT hr;
	D3D11_BUFFER_DESC vertexBuffer_desc;
	vertexBuffer_desc.ByteWidth = numV * sizeof(vertex);
	vertexBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//バインド時の識別
	vertexBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBuffer_desc.MiscFlags = 0;
	vertexBuffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBuffer_data;
	vertexBuffer_data.pSysMem = vertices;
	vertexBuffer_data.SysMemPitch = 0;
	vertexBuffer_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vertexBuffer_desc, &vertexBuffer_data, &vertex_buffer);
	if (FAILED(hr))return false;

	if (indices)
	{
		D3D11_BUFFER_DESC indexBuffer_desc;
		indexBuffer_desc.ByteWidth = numI * sizeof(unsigned int);
		indexBuffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBuffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBuffer_desc.CPUAccessFlags = 0;
		indexBuffer_desc.MiscFlags = 0;
		indexBuffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexBuffer_data;
		indexBuffer_data.pSysMem = indices;
		indexBuffer_data.SysMemPitch = 0;
		indexBuffer_data.SysMemSlicePitch = 0;

		numIndices = numI;

		hr = device->CreateBuffer(&indexBuffer_desc, &indexBuffer_data, &index_buffer);
		if (FAILED(hr))return false;
	}

	D3D11_BUFFER_DESC constantBuffer_desc;
	constantBuffer_desc.ByteWidth = sizeof(cbuffer);
	constantBuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constantBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBuffer_desc.CPUAccessFlags = 0;
	constantBuffer_desc.MiscFlags = 0;
	constantBuffer_desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&constantBuffer_desc, nullptr, &constant_buffer);
	if (FAILED(hr))return false;

	return true;
}

void static_mesh::createCube(ID3D11Device* _device)
{
	createPrimitive(_device);
	vertex vertices[4 * 6] = {};
	unsigned int indices[3 * 2 * 6] = {};

	int numV = 0, numI = 0;

	//	上面
	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	下面
	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	//	右面
	vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	左面
	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	//	後面
	vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	前面
	vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	if (!createBuffer(_device, vertices, numV, indices, numI))
		assert(0 && "error");
}

void static_mesh::createSphere(ID3D11Device* _device, u_int slices, u_int stacks)
{
	createPrimitive(_device);

	std::vector<vertex> vertices;
	std::vector<u_int> indices;


	float r = 0.5f;		//	半径 0.5f = 直径 1.0f

						//
						// Compute the vertices stating at the top pole and moving down the stacks.
						//

						// Poles: note that there will be texture coordinate distortion as there is
						// not a unique point on the texture map to assign to the pole when mapping
						// a rectangular texture onto a sphere.
	vertex top_vertex;
	top_vertex.position = DirectX::XMFLOAT3(0.0f, +r, 0.0f);
	top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

	vertex bottom_vertex;
	bottom_vertex.position = DirectX::XMFLOAT3(0.0f, -r, 0.0f);
	bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices.push_back(top_vertex);

	float phi_step = DirectX::XM_PI / stacks;
	float theta_step = 2.0f * DirectX::XM_PI / slices;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (u_int i = 1; i <= stacks - 1; ++i)
	{
		float phi = i * phi_step;
		float rs_phi = r * sinf(phi), rc_phi = r * cosf(phi);

		// Vertices of ring.
		for (u_int j = 0; j <= slices; ++j)
		{
			float theta = j * theta_step;

			vertex v;

			// spherical to cartesian
			v.position.x = rs_phi * cosf(theta);
			v.position.y = rc_phi;
			v.position.z = rs_phi * sinf(theta);

			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottom_vertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//
	for (UINT i = 1; i <= slices; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	u_int base_index = 1;
	u_int ring_vertex_count = slices + 1;
	for (u_int i = 0; i < stacks - 2; ++i)
	{
		u_int i_rvc = i * ring_vertex_count;
		u_int i1_rvc = (i + 1) * ring_vertex_count;

		for (u_int j = 0; j < slices; ++j)
		{
			indices.push_back(base_index + i_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j);

			indices.push_back(base_index + i1_rvc + j);
			indices.push_back(base_index + i_rvc + j + 1);
			indices.push_back(base_index + i1_rvc + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	u_int south_pole_index = (u_int)vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	base_index = south_pole_index - ring_vertex_count;

	for (u_int i = 0; i < slices; ++i)
	{
		indices.push_back(south_pole_index);
		indices.push_back(base_index + i);
		indices.push_back(base_index + i + 1);
	}
	createBuffer(_device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void static_mesh::createBillboard(ID3D11Device* _device, const wchar_t* _textureName)
{
	D3D11_INPUT_ELEMENT_DESC input_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(input_desc);
	std::string vsName = "./Data/shader/obj_vs.cso";
	std::string psName = "./Data/shader/obj_ps.cso";
	orientation = Donya::Quaternion::Make(Donya::Vector3::Up(), 0);
	materials.emplace_back();
	materials.back().Ka = { 0,0,0 };
	materials.back().Kd = { 0,0,0 };
	materials.back().Ks = { 0,0,0 };
	materials.back().illum = 0;
	materials.back().map_Kd = _textureName;

	if (!ResourceManager::LoadShaderResourceView(_device, materials.back().map_Kd.c_str(), &materials.back().Resource, &materials.back().tex2dDesc))
	{
		assert(0 && "テクスチャ画像読み込み失敗");
		return;
	}

	init(_device, vsName, input_desc, numElements, psName);

	vertex vertices[4] = {};
	unsigned int indices[6] = {};

	int numV = 0, numI = 0;

	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, 0);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, 0);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, 0);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, 0);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	vertices[0].texcoord = { 0,0 };
	vertices[1].texcoord = { 1,0 };
	vertices[2].texcoord = { 0,1 };
	vertices[3].texcoord = { 1,1 };

	numV = 4;
	numI = 6;
	subsets.emplace_back();
	subsets.back().index_count = numI;
	subsets.back().index_start = 0;

	for (auto& p : indices)
	{
		subsets.back().m_vertexes.push_back(vertices[p]);
	}
	createBuffer(_device, vertices, numV, indices, numI);

}

void static_mesh::createPlane(ID3D11Device* _device, u_int _vertical, u_int _side)
{
	createPrimitive(_device);

	vertex vertices[8] = {};
	unsigned int indices[12] = {};

	int numV = 0, numI = 0;

	//	上面
	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f * _side, .0f, +0.5f * _vertical);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f * _side, .0f, +0.5f * _vertical);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f * _side, .0f, -0.5f * _vertical);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f * _side, .0f, -0.5f * _vertical);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	下面
	vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f * _side, .0f, +0.5f * _vertical);
	vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f * _side, .0f, +0.5f * _vertical);
	vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f * _side, .0f, -0.5f * _vertical);
	vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f * _side, .0f, -0.5f * _vertical);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	subsets.emplace_back();
	subsets.back().index_count = numI;
	subsets.back().index_start = 0;
	for (auto& p : indices)
	{
		subsets.back().m_vertexes.push_back(vertices[p]);
	}
	createBuffer(_device, vertices, numV, indices, numI);

}

void static_mesh::render(
	ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& SynthesisMatrix,
	const DirectX::XMFLOAT4X4& worldMatrix,
	const DirectX::XMFLOAT4& camPos,
	line_light& _lightAmbient,
	std::vector<point_light>& _point_light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireFlg
)
{
	int size = _point_light.size();

	if (!materials.empty())
		for (auto& it : materials)
		{
			//	定数バッファの作成
			cbuffer cb;
			cb.world_view_projection = SynthesisMatrix;
			cb.world = worldMatrix;
			cb.camPos = camPos;
			cb.lineLight = _lightAmbient.getInfo();
			for (int i = 0; i < 5; i++)
			{
				if (size <= i)
				{
					cb.pntLight[i].pos.w = 0;
					continue;
				}
				cb.pntLight[i] = _point_light[i].getInfo();
			}
			cb.b_material.ambient = { it.Ka.x,it.Ka.y,it.Ka.z,1.0f };
			cb.b_material.diffuse = { it.Kd.x,it.Kd.y,it.Kd.z,1.0f };
			cb.b_material.specular = { it.Ks.x,it.Ks.y,it.Ks.z,0.5f };
			context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer);

			// 頂点バッファのバインド
			UINT stride = sizeof(vertex);
			UINT offset = 0;
			context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

			//	インデックスバッファのバインド
			context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

			//	プリミティブモードの設定
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


			//	ラスタライザーの設定
			if (!wireFlg)	context->RSSetState(rasterizeFillOut);
			else		context->RSSetState(rasterizeLine);

			if (it.Resource)
			{
				//	入力レイアウトのバインド
				context->IASetInputLayout(layout);
				//	シェーダー(2種)の設定
				context->VSSetShader(vertexShader, nullptr, 0);
				context->PSSetShader(pixelShader, nullptr, 0);
			}
			else
			{
				//	入力レイアウトのバインド
				context->IASetInputLayout(noTexLayout);
				//	シェーダー(2種)の設定
				context->VSSetShader(noTexVS, nullptr, 0);
				context->PSSetShader(noTexPS, nullptr, 0);
			}
			//	深度テストの設定
			context->OMSetDepthStencilState(depthStencilState, 0);

			context->PSSetShaderResources(0, 1, &it.Resource);
			context->PSSetSamplers(0, 1, &sampleState);

			for (auto& itr : subsets)
			{
				//	プリミティブの描画
				if (it.newmtl != itr.usemtl)
					continue;
				context->DrawIndexed(itr.index_count, itr.index_start, 0);
			}
		}
	else
	{
		//	定数バッファの作成
		cbuffer cb;
		cb.world_view_projection = SynthesisMatrix;
		cb.world = worldMatrix;
		cb.camPos = camPos;
		cb.lineLight = _lightAmbient.getInfo();
		for (int i = 0; i < 5; i++)
		{
			if (size <= i)
			{
				cb.pntLight[i].pos.w = 0;
				continue;
			}
			cb.pntLight[i] = _point_light[i].getInfo();
		}
		cb.b_material.ambient = materialColor;
		cb.b_material.diffuse = { 1,1,1,1 };
		cb.b_material.specular = { 1,1,1,1 };
		context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
		context->VSSetConstantBuffers(0, 1, &constant_buffer);
		context->PSSetConstantBuffers(0, 1, &constant_buffer);

		// 頂点バッファのバインド
		UINT stride = sizeof(vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

		//	インデックスバッファのバインド
		context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

		//	プリミティブモードの設定
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//	入力レイアウトのバインド
		context->IASetInputLayout(layout);

		//	ラスタライザーの設定
		if (!wireFlg)	context->RSSetState(rasterizeFillOut);
		else		context->RSSetState(rasterizeLine);

		//	シェーダー(2種)の設定
		context->VSSetShader(vertexShader, nullptr, 0);

		context->PSSetShader(pixelShader, nullptr, 0);


		//	深度テストの設定
		context->OMSetDepthStencilState(depthStencilState, 0);

		//	プリミティブの描画
		context->DrawIndexed(numIndices, 0, 0);

	}
}

void static_mesh::billboardRender(
	ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& SynthesisMatrix,
	const DirectX::XMFLOAT4& pos,
	const DirectX::XMFLOAT2 scale,
	const float angle,
	const DirectX::XMFLOAT4& camPos,
	const DirectX::XMFLOAT2& texpos, const DirectX::XMFLOAT2& texsize)
{

	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = context->Map(vertex_buffer, 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices = static_cast<vertex*>(mapped_buffer.pData);

	DirectX::XMINT2 tex = { (int)materials.back().tex2dDesc.Width,(int)materials.back().tex2dDesc.Height };
	vertices[0].texcoord = { texpos.x / tex.x * 1.0f,texpos.y / tex.y * 1.0f };
	vertices[1].texcoord = { (texpos.x + texsize.x) / tex.x * 1.0f,texpos.y / tex.y * 1.0f };
	vertices[2].texcoord = { texpos.x / tex.x * 1.0f,(texpos.y + texsize.y) / tex.y * 1.0f };
	vertices[3].texcoord = { (texpos.x + texsize.x) / tex.x * 1.0f,(texpos.y + texsize.y) / tex.y * 1.0f };

	vertices[0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, 0);
	vertices[1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, 0);
	vertices[2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, 0);
	vertices[3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, 0);


	vertices[0].normal = vertices[1].normal =
		vertices[2].normal = vertices[3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);

	context->Unmap(vertex_buffer, 0);

	//transformVertex

	DirectX::XMFLOAT3 direction = { camPos.x - pos.x,camPos.y - pos.y,camPos.z - pos.z };
	float size = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	direction = { -direction.x / size,-direction.y / size,-direction.z / size };

	orientation = Quaternion::LookAt(orientation, direction).Normalized();
	Quaternion front = Donya::Quaternion::Make(orientation.LocalFront(), angle * 3.14f / 180.0f);
	Quaternion rotate = orientation;
	rotate.RotateBy(front);
	rotate.Normalized();

	DirectX::XMMATRIX S, R, T, world;
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	拡大・縮小
	S = DirectX::XMMatrixScaling(scale.x, scale.y, 1);

	//	回転
	R = DirectX::XMLoadFloat4x4(&rotate.RequireRotationMatrix());

	world = S * R * T;

	DirectX::XMMATRIX viewProjection = DirectX::XMLoadFloat4x4(&SynthesisMatrix);
	DirectX::XMFLOAT4X4 WVP, W;
	//	Matrix -> Float4x4 変換
	DirectX::XMStoreFloat4x4(&WVP, world * viewProjection);
	DirectX::XMStoreFloat4x4(&W, world);

	cbuffer cb;
	cb.world_view_projection = WVP;
	cb.world = W;
	cb.camPos = camPos;
	cb.lineLight.color = { 0,0,0,0 };
	cb.lineLight.direction = { 0,0,0,0 };

	for (int i = 0; i < 5; i++)
	{
		cb.pntLight[i].pos = { 0,0,0,0 };
		cb.pntLight[i].attenuate = { 0,0,0,0 };

	}
	cb.b_material.ambient = { 0,0,0,1.0f };
	cb.b_material.diffuse = { 0,0,0,1.0f };
	cb.b_material.specular = { 0,0,0,0.5f };
	context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constant_buffer);

	// 頂点バッファのバインド
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//	インデックスバッファのバインド
	context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//	プリミティブモードの設定
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//	ラスタライザーの設定
	context->RSSetState(rasterizeFillOut);

	//	入力レイアウトのバインド
	context->IASetInputLayout(layout);
	//	シェーダー(2種)の設定
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
	//	深度テストの設定
	context->OMSetDepthStencilState(depthStencilState, 0);

	context->PSSetShaderResources(0, 1, &materials.back().Resource);
	context->PSSetSamplers(0, 1, &sampleState);

	context->DrawIndexed(subsets.back().index_count, subsets.back().index_start, 0);

}

void static_mesh::renderFirst(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& SynthesisMatrix,
	const DirectX::XMFLOAT4X4& worldMatrix
)
{
	shadow_cbuffer cb;
	cb.world_view_projection = SynthesisMatrix;
	cb.world = worldMatrix;
	context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constant_buffer);
	context->PSSetConstantBuffers(0, 1, &constant_buffer);

	// 頂点バッファのバインド
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//	インデックスバッファのバインド
	context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//	プリミティブモードの設定
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	入力レイアウトのバインド
	context->IASetInputLayout(layout);

	//	ラスタライザーの設定
	context->RSSetState(rasterizeFillOut);


	//	シェーダー(2種)の設定

	//context->OMGetRenderTargets();
	//	深度テストの設定
	context->OMSetDepthStencilState(depthStencilState, 0);

	//	プリミティブの描画
	context->DrawIndexed(numIndices, 0, 0);


}

void static_mesh::renderSecond(ID3D11DeviceContext*, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4&, const DirectX::XMFLOAT4&, ID3D11ShaderResourceView*, bool)
{
}
