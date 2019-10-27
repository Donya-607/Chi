#include "primitive.h"

namespace GameLib
{
	Primitive::Primitive(ID3D11Device* device) :
		vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr), buffer(nullptr), rasterizerState(nullptr)
	{
		//VertexBufferの作成
		vertex vertices[130] = { DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(0, 0, 0, 0) };
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(vertices);			// 頂点バッファのサイズ
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファ
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// GPU→読み取りのみ　CPU→書き込みのみ
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vertices;	// 頂点のアドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;
		if (FAILED(device->CreateBuffer(&bd, &initData, &buffer)))
		{
			assert(!"頂点バッファの作成に失敗(Primitive)");
			return;
		}

		// 頂点宣言
		// 入力レイアウトの定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,     D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		//	頂点シェーダーの読み込み
		if (!ResourceManager::LoadVertexShader(device, "./Data/shader/primitive_vs.cso", layout, numElements, &vertexShader, &inputLayout))
		{
			assert(!"頂点シェーダーの読み込みに失敗(Primitive)");
			return;
		}
		//	ピクセルシェーダーの作成
		if (!ResourceManager::LoadPixelShader(device, "./Data/shader/primitive_ps.cso", &pixelShader))
		{
			assert(!"ピクセルシェーダーの作成に失敗(Primitive)");
			return;
		}

		//	ラスタライザステートの設定
		D3D11_RASTERIZER_DESC rsDesc = {};
		rsDesc.FillMode = D3D11_FILL_SOLID;//
		rsDesc.CullMode = D3D11_CULL_NONE;//	カリング
		rsDesc.FrontCounterClockwise = false;
		rsDesc.DepthBias = 0;
		rsDesc.DepthBiasClamp = 0;
		rsDesc.SlopeScaledDepthBias = 0;
		rsDesc.DepthClipEnable = false;
		rsDesc.ScissorEnable = false;
		rsDesc.MultisampleEnable = false;
		rsDesc.AntialiasedLineEnable = false;
		if (FAILED(device->CreateRasterizerState(&rsDesc, &rasterizerState)))
		{
			assert(!"ラスタライザステートの作成に失敗(Primitive)");
			return;
		}

		D3D11_DEPTH_STENCIL_DESC dsDesc;
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.StencilEnable = FALSE;
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
			assert(!"デプスステンシルステートの作成に失敗(Primitive)");
			return;
		}
	}
	Primitive::~Primitive()
	{
		safe_release(depthStencilState);
		safe_release(rasterizerState);
		safe_release(buffer);
		ResourceManager::ReleasePixelShader(pixelShader);
		ResourceManager::ReleaseVertexShader(vertexShader, inputLayout);

	}


	void Primitive::rect(ID3D11DeviceContext* context,
		const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size,
		const DirectX::XMFLOAT2& center, float angle,
		const DirectX::XMFLOAT4& color) const
	{
		if (size.x * size.y == 0.0f) return;

		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		context->RSGetViewports(&numViewports, &viewport);

		vertex vertices[] = {
			{ DirectX::XMFLOAT3(-0.0f, +1.0f, 0), color },
			{ DirectX::XMFLOAT3(+1.0f, +1.0f, 0), color },
			{ DirectX::XMFLOAT3(-0.0f, -0.0f, 0), color },
			{ DirectX::XMFLOAT3(+1.0f, -0.0f, 0), color },
		};

		float sinValue = sinf(angle);
		float cosValue = cosf(angle);
		for (int i = 0; i < 4; i++) {
			vertices[i].position.x *= size.x;
			vertices[i].position.y *= size.y;
			vertices[i].position.x -= center.x;
			vertices[i].position.y -= center.y;

			float rx = vertices[i].position.x;
			float ry = vertices[i].position.y;
			vertices[i].position.x = rx * cosValue - ry * sinValue;
			vertices[i].position.y = rx * sinValue + ry * cosValue;
			vertices[i].position.x += pos.x;
			vertices[i].position.y += pos.y;

			vertices[i].position.x = vertices[i].position.x * 2 / viewport.Width - 1.0f;
			vertices[i].position.y = 1.0f - vertices[i].position.y * 2 / viewport.Height;
		}

		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, vertices, sizeof(vertices));
		context->Unmap(buffer, 0);

		UINT stride = sizeof(vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		context->IASetInputLayout(inputLayout);
		context->RSSetState(rasterizerState);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);
		context->OMSetDepthStencilState(depthStencilState, 1);

		context->Draw(4, 0);
	}


	void Primitive::rect(ID3D11DeviceContext* context,
		float x, float y, float w, float h,
		float cx, float cy, float angle,
		float r, float g, float b, float a) const
	{
		rect(context, DirectX::XMFLOAT2(x, y), DirectX::XMFLOAT2(w, h), DirectX::XMFLOAT2(cx, cy), angle, DirectX::XMFLOAT4(r, g, b, a));
	}

	void Primitive::line(ID3D11DeviceContext *context,
		const DirectX::XMFLOAT2 &start, const DirectX::XMFLOAT2 &fin, 
		const DirectX::XMFLOAT4 &color, float width) const
	{
		if (width <= 0.0f) return;

		DirectX::XMFLOAT2 v1(start.x, start.y);
		DirectX::XMFLOAT2 v2(fin.x, fin.y);
		DirectX::XMFLOAT2 vec(v2.x - v1.x,v2.y-v1.y);
		float w = sqrtf(vec.x*vec.x+vec.y*vec.y);
		float h = width;
		float cx = w * 0.5f;
		float cy = h * 0.5f;
		float x = (v1.x + v2.x) * 0.5f;
		float y = (v1.y + v2.y) * 0.5f;
		float angle = atan2(vec.y, vec.x);

		rect(context, { x, y }, { w, h }, { cx, cy }, angle, color);

	}

	void Primitive::line(ID3D11DeviceContext *context,
		float x1, float y1, float x2, float y2,
		float r, float g, float b, float a, float width) const
	{
		line(context, { x1,y1 }, { x2,y2 }, { r,g,b,a }, width);
	}

	void Primitive::circle(ID3D11DeviceContext *context,
		const DirectX::XMFLOAT2 &center,float radius,
		const DirectX::XMFLOAT4 &color, int n) const
	{
		if (n < 3 || radius <= 0.0f) return;
		if (n > 64) n = 64;//最大64角形

		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		context->RSGetViewports(&numViewports, &viewport);

		vertex vertices[130] = { DirectX::XMFLOAT3(0,0,0) };
		float arc = DirectX::XM_PI * 2 / n;
		vertex *p = &vertices[0];
		for (int i = 0; i <= n; i++)
		{
			
			p->position.x = center.x + cosf(-DirectX::XM_PI*0.5f + arc * i) * radius;
			p->position.y = center.y + sinf(-DirectX::XM_PI*0.5f + arc * i) * radius;
			p->position.x = p->position.x * 2 / viewport.Width - 1.0f;
			p->position.y = 1.0f - p->position.y * 2 / viewport.Height;
			p->color = color;
			p++;

			p->position.x = center.x; p->position.y = center.y;
			p->position.x = p->position.x * 2 / viewport.Width - 1.0f;
			p->position.y = 1.0f - p->position.y * 2 / viewport.Height;
			p->color = color;
			p++;
		}

		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, vertices, sizeof(vertex) * (n + 1) * 2);
		context->Unmap(buffer, 0);

		UINT stride = sizeof(vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		context->IASetInputLayout(inputLayout);
		context->RSSetState(rasterizerState);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);
		context->OMSetDepthStencilState(depthStencilState, 1);

		context->Draw((n + 1) * 2 - 1, 0);
		context->RSGetViewports(&numViewports, &viewport);
	}

	void Primitive::circle(ID3D11DeviceContext *context,
		float x, float y, float radius,
		float r, float g, float b, float a, int n) const
	{
		circle(context, { x,y }, radius, { r,g,b,a }, n);
	}

	void Primitive::quad(ID3D11DeviceContext *context, const DirectX::XMFLOAT2(&pos)[4], const DirectX::XMFLOAT4 &color) const
	{
		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		context->RSGetViewports(&numViewports, &viewport);

		vertex vertices[] = {
			{ DirectX::XMFLOAT3(pos[3].x, pos[3].y, 0), color },
			{ DirectX::XMFLOAT3(pos[2].x, pos[2].y, 0), color },
			{ DirectX::XMFLOAT3(pos[0].x, pos[0].y, 0), color },
			{ DirectX::XMFLOAT3(pos[1].x, pos[1].y, 0), color },
		};

		for (int i = 0; i < 4; i++)
		{
			vertices[i].position.x = vertices[i].position.x * 2 / viewport.Width - 1.0f;
			vertices[i].position.y = 1.0f - vertices[i].position.y * 2 / viewport.Height;
		}

		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, vertices, sizeof(vertices));
		context->Unmap(buffer, 0);

		UINT stride = sizeof(vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		context->IASetInputLayout(inputLayout);
		context->RSSetState(rasterizerState);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);
		context->OMSetDepthStencilState(depthStencilState, 1);

		context->Draw(4, 0);
	}

	void Primitive::quad(ID3D11DeviceContext *context, const DirectX::XMFLOAT2(&pos)[4], float r, float g, float b, float a) const
	{
		quad(context, pos, { r, g, b, a });
	}

}