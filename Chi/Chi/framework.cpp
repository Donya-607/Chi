#include "framework.h"

bool framework::initialize()
{
	//ハンドルリザルト : これがあるとエラー発生時にメッセージがもらえる
	HRESULT hr = S_OK;
	UINT createDeviceFlags = 0;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	//スワップチェインの引数を構造体でまとめたもの
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;


	//スワップチェインとクリエイトデバイスを同時に行う : for
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_swapChain, &m_device, &m_featureLevel, &m_deviceContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return false;

	//レンダービューターゲットの作成

	ID3D11Texture2D* back_buffer = nullptr;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
	if (FAILED(hr))
		return false;

	hr = m_device->CreateRenderTargetView(back_buffer, NULL, &m_renderTargetView);
	if (FAILED(hr))
		return false;

	back_buffer->Release();


	D3D11_TEXTURE2D_DESC txDesc;
	txDesc.Width = SCREEN_WIDTH;
	txDesc.Height = SCREEN_HEIGHT;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;

	ID3D11Texture2D* depth_stencil_buffer;
	hr = m_device->CreateTexture2D(&txDesc, NULL, &depth_stencil_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
	depth_stencil_view_desc.Format = txDesc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags = 0;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	hr = m_device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &m_depthStenciView);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_buffer->Release();


	//ビューポートの生成
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_deviceContext->RSSetViewports(1, &vp);

	//for (int i = 0; i < 1024; i++)
	//{
	//	sprites[i] = new Sprite(m_device, L"particle-smoke.png");
	//}
	//sprites[1] = new Sprite(m_device, L"n64.png");

	//particle.RoadSprite(m_device, L"particle-smoke.png");
	font.RoadSprite(m_device, L"./fonts/font0.png");
	//box = new geometric_primitve(m_device);
	//particle_b = new sprite_batch(m_device, L"particle-smoke.png");
	//sprites_b[0] = new sprite_batch(m_device, L"logos.jpg");
	//sprites_b[1] = new sprite_batch(m_device, L"n64.png");

	//sample_particle = new DirectX::SpriteBatch(m_deviceContext);

	UINT m4xMsaaQuality;
	m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);

	return true;
}
void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{

}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr = S_OK;

	//ビューポートの設定

	float ClearColor[4] = { 0.5f, .0f, .0f, 1.0f }; //red,green,blue,alpha

	m_deviceContext->ClearRenderTargetView(m_renderTargetView, ClearColor);

	m_deviceContext->ClearDepthStencilView(m_depthStenciView, D3D11_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStenciView);

	static float angle = 0;
	angle += 6.0f * elapsed_time; // 1 round per 60 seconds
	float x = 0;
	float y = 0;
	static DirectX::XMFLOAT2 sprite_position[1024] = {};
	static float timer = 0;
	timer += elapsed_time;

	static blender Blender(m_device);
	//m_deviceContext->OMSetBlendState(Blender.states[blender::BS_NONE], nullptr, 0xFFFFFFFF);
	//sprites_b[0]->begin(m_deviceContext);
	//sprites_b[0]->render(m_deviceContext, 0, 0, 1280, 720, 0, 0, 1920, 1080, 0, 1, 1, 1, 1);
	//sprites_b[0]->end(m_deviceContext);
	m_deviceContext->OMSetBlendState(Blender.states[blender::BS_ADD], nullptr, 0xFFFFFFFF);
	//sprites_b[1]->begin(m_deviceContext);
	//sprites_b[1]->render(m_deviceContext, 8, 8, 1280, 720, 0, 0, 900, 877, 0, 1, 1, 1, 0.5);
	//sprites_b[1]->end(m_deviceContext);

	static TimeMeasure measure;
	static std::deque<double> benchmark(10);


	measure.Start();
	if (timer > 4.0f)
	{
		for (auto &p : sprite_position)
		{
			// update positions once in four seconds  
			float a = static_cast<float>(rand()) / RAND_MAX * 360.0f;
			// angle(degree)    
			float r = static_cast<float>(rand()) / RAND_MAX * 256.0f;
			// radius(screen space)   
			p.x = cosf(a*0.01745f)*r;
			p.y = sinf(a*0.01745f)*r;
		}
		timer = 0;
	}
	//particle_b->begin(m_deviceContext);

	//for (int i = 0; i < 10; i++)
	//{
	//	for (auto &p : sprite_position)
	//	{
	//		particle.Render
	//		(
	//			m_deviceContext, p.x + 256*(i%5), p.y + 256*(i/5),
	//			128, 128, 0, 0, 420, 420,
	//			angle * 4, 0.2f, 0.05f*timer, 0.01f*timer,
	//			fabsf(sinf(3.141592f*timer*0.5f*0.5f))
	//		);
	//		//particle_b->render
	//		//(
	//		//	m_deviceContext, p.x + 256  *(i % 5), p.y + 256 * (i / 5),
	//		//	128, 128, 0, 0, 420, 420,
	//		//	angle * 4, 0.2f, 0.05f*timer, 0.01f*timer,
	//		//	fabsf(sinf(3.141592f*timer*0.5f*0.5f))
	//		//);

	//	}
	//}
	//particle_b->end(m_deviceContext);
	DirectX::XMFLOAT4 pos = { 0,0,-10,1 };
	DirectX::XMFLOAT4 target = { 0,0,0,1 };
	DirectX::XMVECTOR	p = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR	t = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	DirectX::XMVECTOR	up = DirectX::XMVectorSet(0.0f, -10.0f, 0.0f, 1.0f);

	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(p, t, up);
	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(30 * (3.141592f / 180), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);
	DirectX::XMMATRIX world;

	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	初期化
	world = DirectX::XMMatrixIdentity();

	//	拡大・縮小
	S = DirectX::XMMatrixScaling(1.0, 1.0, 1.0);

	//	回転
	Rx = DirectX::XMMatrixRotationX(30);				//	X軸を基準とした回転行列
	Ry = DirectX::XMMatrixRotationY(0);				//	Y軸を基準とした回転行列
	Rz = DirectX::XMMatrixRotationZ(0);				//	Z軸を基準とした回転行列
	R = Rz * Ry * Rx;

	//	平行移動
	T = DirectX::XMMatrixTranslation(0, 0.5, 0);

	//	ワールド変換行列
	world = S * R * T;


	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMFLOAT4X4 World;
	DirectX::XMStoreFloat4x4(&world_view_projection, world * view * projection);

	//box->render(m_deviceContext,world_view_projection,World, {1,1,0,1}, { 1,1,0,1 }, false);

	benchmark.push_back(measure.End());
	benchmark.pop_front();

	double result = 0;
	for (int i = 0; i < 10; i++)
	{
		result += benchmark[i];
	}
	result /= 10;

	font.textout(m_deviceContext, "benchmark t = " + std::to_string(result), 0, 0, 16, 16, 1, 1, 1, 1);

	//レンダリングしたイメージの描画(引数を1にすると60fps固定にできる)
	m_swapChain->Present(0, 0);

}

