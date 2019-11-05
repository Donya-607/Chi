#include "gameLib.h"
#include <crtdbg.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);


namespace GameLib
{
	struct Members
	{
		// ウインドウ関連
		HWND hwnd;  // ウインドウハンドル

					// DirectX11関連
		ID3D11Device*           device = nullptr;
		ID3D11DeviceContext*    context = nullptr;
		IDXGISwapChain*         swapChain = nullptr;
		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;
		ID3D11BlendState*       blendState = nullptr;

		std::wstring projectName;

		//// その他
		blender*                 Blender = nullptr;
		Primitive*               primitive = nullptr;
		//PrimitiveBatch*        primitiveBatch;
		high_resolution_timer    hrTimer;
		Camera					 cam;
		line_light				 LineLight;
		std::vector<point_light> pointLights;
		XboxPad					 pad[4];
		XboxPad					 prevPad[4];
		keyInput				 keyboard;
		dragDrop				drag_drop;
	};

	static Members m;


	HWND getHandle()
	{
		return m.hwnd;
	}

	ID3D11Device * getDevice()
	{
		return m.device;
	}

	ID3D11DeviceContext * getContext()
	{
		return m.context;
	}

	bool createRenderTarget(ID3D11RenderTargetView* _renderTarget)
	{
		HRESULT hr = S_OK;
		ID3D11Texture2D* back_buffer = nullptr;
		hr = m.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
		if (FAILED(hr))
			return false;

		hr = m.device->CreateRenderTargetView(back_buffer, NULL, &_renderTarget);
		if (FAILED(hr))
			return false;

		back_buffer->Release();
		return true;

	}
	bool setRenderTarget(ID3D11RenderTargetView** _renderTarget)
	{
		m.context->OMSetRenderTargets(1, _renderTarget, m.depthStencilView);
		return true;
	}
	bool resetRendertarget()
	{
		m.context->OMSetRenderTargets(1, &m.renderTargetView, m.depthStencilView);
		return true;
	}


	bool createShadowRT(ID3D11RenderTargetView** _renderTarget)
	{

		HRESULT hr = S_OK;
		ID3D11Texture2D* back_buffer = nullptr;
		D3D11_TEXTURE2D_DESC renderTextureDesc;
		ZeroMemory(&renderTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		renderTextureDesc.Width = (UINT)pSystem->SCREEN_WIDTH;
		renderTextureDesc.Height = (UINT)pSystem->SCREEN_HEIGHT;
		renderTextureDesc.MipLevels = 1;
		renderTextureDesc.ArraySize = 1;
		renderTextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		renderTextureDesc.SampleDesc.Count = 1;
		renderTextureDesc.SampleDesc.Quality = 0;
		renderTextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		renderTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
		renderTextureDesc.CPUAccessFlags = 0;
		hr = m.device->CreateTexture2D(&renderTextureDesc, NULL, &back_buffer);
		if (FAILED(hr))
			return false;

		//hr = m.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
		//if (FAILED(hr))
		//	return false;

		hr = m.device->CreateRenderTargetView(back_buffer, NULL, _renderTarget);
		if (FAILED(hr))
			return false;


		//back_buffer->Release();
		return true;
	}
	bool createShadowSRV(ID3D11ShaderResourceView* _shaderResource)
	{
		HRESULT hr = S_OK;
		ID3D11Texture2D* back_buffer = nullptr;
		D3D11_TEXTURE2D_DESC renderTextureDesc;
		ZeroMemory(&renderTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		renderTextureDesc.Width = (UINT)pSystem->SCREEN_WIDTH;
		renderTextureDesc.Height = (UINT)pSystem->SCREEN_HEIGHT;
		renderTextureDesc.MipLevels = 1;
		renderTextureDesc.ArraySize = 1;
		renderTextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		renderTextureDesc.SampleDesc.Count = 1;
		renderTextureDesc.SampleDesc.Quality = 0;
		renderTextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		renderTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
		renderTextureDesc.CPUAccessFlags = 0;
		hr = m.device->CreateTexture2D(&renderTextureDesc, nullptr, &back_buffer);
		if (FAILED(hr))
			return false;

		hr = m.device->CreateShaderResourceView(back_buffer, nullptr, &_shaderResource);
		if (FAILED(hr))
			return false;
		return true;


	}


	float getDeltaTime()
	{
		return m.hrTimer.time_interval();
	}

	DirectX::XMINT2 getWindowSize()
	{
		return{ pSystem->SCREEN_WIDTH ,pSystem->SCREEN_HEIGHT };
	}

	void init(LPCTSTR caption, HINSTANCE instance, int width, int height, int iconNum, bool isFullscreen, double frameRate)
	{

		pSystem->SCREEN_WIDTH = width;
		pSystem->SCREEN_HEIGHT = height;
		m.projectName = caption;
		//乱数の設定
		srand((unsigned int)time(NULL));

		m.hwnd = window::init(caption, instance, pSystem->SCREEN_WIDTH, pSystem->SCREEN_HEIGHT, iconNum);

		DirectX11::init(m.hwnd, pSystem->SCREEN_WIDTH, pSystem->SCREEN_HEIGHT, isFullscreen);

		if (frameRate != 0)
			m.hrTimer.setFrameRate(frameRate);

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(m.hwnd);
		ImGui_ImplDX11_Init(m.device, m.context);

	}

	void uninit()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		// DirectX11の終了処理
		DirectX11::uninit();

		// ウインドウの終了処理
		window::uninit();
	}

	bool gameLoop(bool isShowFrameRate)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//フレームレート用
		while (!m.hrTimer.tick());
		if (isShowFrameRate)
		{
			calculateFrameStats(m.hwnd, &m.hrTimer, m.projectName);//フレームレート計算・タイトルバーに表示
		}
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		m.cam.update();
		input::xInput::getState();
		input::keyboard::update();
		float ClearColor[4] = { 0.5f, .0f, .0f, 1.0f }; //red,green,blue,alpha

		m.context->ClearRenderTargetView(m.renderTargetView, ClearColor);

		m.context->ClearDepthStencilView(m.depthStencilView, D3D11_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

		m.context->OMSetRenderTargets(1, &m.renderTargetView, m.depthStencilView);

		return true;
	}


	HRESULT present(UINT SyncInterval, UINT Flags)
	{
		return m.swapChain->Present(SyncInterval, Flags);
	}



	/*
	/
	/
	/				window
	/
	/
	*/


	static const LPCWSTR CLASS_NAME = L"2dgp";
	//プロトタイプ宣言
	LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//初期化
	HWND window::init(LPCTSTR caption, HINSTANCE instance, int width, int height, int iconNum)
	{
#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = fnWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = GetModuleHandle(NULL);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = CLASS_NAME;
		if (iconNum != -1)
		{
			wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(102));
			wcex.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(102));
		}
		else
		{
			wcex.hIcon = NULL;//ここよ
			wcex.hIconSm = 0;//ここよ
		}
		RegisterClassEx(&wcex);

		RECT rc = { 0, 0, width, height };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		HWND hwnd = CreateWindow(CLASS_NAME, caption, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, GetModuleHandle(NULL), NULL);
		ShowWindow(hwnd, SW_SHOWDEFAULT);

		return hwnd;

	}

	//開放
	void window::uninit()
	{
		UnregisterClass(CLASS_NAME, GetModuleHandle(NULL));
	}


	//ウインドウプロシージャ

	LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		using DirectX::Keyboard;
		HDROP hDrop;
		UINT uFileNo;
		wchar_t szFileName[256] = { 0 };
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			return 1;
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_ACTIVATEAPP:
			Keyboard::ProcessMessage(msg, wParam, lParam);
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
				break;
			}
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Keyboard::ProcessMessage(msg, wParam, lParam);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			m.hrTimer.stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			m.hrTimer.start();
			break;
		case WM_DROPFILES:
				hDrop = (HDROP)wParam;
				uFileNo = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
				for (int i = 0; i < (int)uFileNo; i++) 
				{
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					m.drag_drop.signUpFileName(szFileName);
					for (int i = 0; i < 256; i++)
					{
						szFileName[i] = 0;
					}
				}
				DragFinish(hDrop);
				m.drag_drop.moveFile(hwnd);
				break;
			default:
				return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}


	/*
	/
	/
	/	directX11
	/
	/
	*/
	namespace DirectX11
	{
		//初期化
		HRESULT init(HWND hwnd, int width, int height, bool isFullscreen)
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
			sd.BufferDesc.Width = width;
			sd.BufferDesc.Height = height;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = hwnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = !isFullscreen;


			//スワップチェインとクリエイトデバイスを同時に行う : for
			for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
			{
				D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
				D3D_FEATURE_LEVEL featureLevel;
				hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
					D3D11_SDK_VERSION, &sd, &m.swapChain, &m.device, &featureLevel, &m.context);
				if (SUCCEEDED(hr))
					break;
			}
			if (FAILED(hr))
				return false;

			//レンダービューターゲットの作成

			ID3D11Texture2D* back_buffer = nullptr;
			hr = m.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
			if (FAILED(hr))
				return false;

			hr = m.device->CreateRenderTargetView(back_buffer, NULL, &m.renderTargetView);
			if (FAILED(hr))
				return false;

			back_buffer->Release();


			D3D11_TEXTURE2D_DESC txDesc;
			txDesc.Width = width;
			txDesc.Height = height;
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
			hr = m.device->CreateTexture2D(&txDesc, NULL, &depth_stencil_buffer);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
			ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
			depth_stencil_view_desc.Format = txDesc.Format;
			depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depth_stencil_view_desc.Flags = 0;
			depth_stencil_view_desc.Texture2D.MipSlice = 0;
			hr = m.device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &m.depthStencilView);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			depth_stencil_buffer->Release();


			//ビューポートの生成
			D3D11_VIEWPORT vp;
			vp.Width = (FLOAT)width;
			vp.Height = (FLOAT)height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			m.context->RSSetViewports(1, &vp);


			UINT m4xMsaaQuality;
			m.device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
			m.context->OMSetRenderTargets(1, &m.renderTargetView, m.depthStencilView);
			m.primitive = new Primitive(m.device);
			m.Blender = new blender(m.device);
			m.context->OMSetBlendState(m.Blender->states[m.Blender->BS_NONE], nullptr, 0xFFFFFFFF);
			FontManager::getInstance()->init(m.device);
			return true;
		}


		//解放
		void uninit()
		{
			m.swapChain->SetFullscreenState(false, nullptr);

			//safe_delete(m.primitiveBatch);
			safe_delete(m.primitive);
			safe_delete(m.Blender);

			safe_release(m.depthStencilView);
			safe_release(m.renderTargetView);
			safe_release(m.swapChain);
			safe_release(m.context);
			safe_release(m.device);
		}

	}

	void clear(const DirectX::XMFLOAT4& color)
	{
		m.context->ClearRenderTargetView(m.renderTargetView, (const float *)&color);
		m.context->ClearDepthStencilView(m.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}

	namespace blend
	{
		void setBlendMode(int _blendNum, const float alpha)
		{
			m.context->OMSetBlendState(m.Blender->states[_blendNum], nullptr, 0xFFFFFFFF);
			pSystem->alpha = alpha;

		}
	}

	namespace debug
	{
		void drawDebug(DirectX::XMINT2 _pos, std::wstring str)
		{
			HDC hdc = GetDC(m.hwnd);
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, _pos.x, _pos.y, str.c_str(), str.size());
			ReleaseDC(m.hwnd, hdc);
			RECT rect;
			GetClientRect(m.hwnd, &rect);
			ValidateRect(m.hwnd, &rect);
		}

	}

	namespace texture
	{
		void SpriteRender(Sprite* _spr,
			float _dx, float _dy, float _dw, float _dh,
			float _sx, float _sy, float _sw, float _sh,
			float _angle, float _r, float _g, float _b,
			float _cx, float _cy, bool _flipX, bool _flipY
		)
		{
			_spr->Render(m.context, _dx, _dy, _dw, _dh, _sx, _sy, _sw, _sh, _angle, _r, _g, _b, pSystem->alpha, _cx, _cy, _flipX, _flipY);
		}

		void SpriteRoad(Sprite* spr, std::wstring texName)
		{
			spr->RoadSprite(m.device, texName.c_str());
		}
	}

	namespace primitive
	{
		//--------------------------------
		//  矩形描画
		//--------------------------------
		void rect(
			float x, float y, float w, float h,
			float cx, float cy, float angle,
			float r, float g, float b)
		{
			rect({ x, y }, { w, h }, { cx, cy }, angle, { r, g, b });
		}

		void rect(const DirectX::XMFLOAT2 &pos, const DirectX::XMFLOAT2 &size, const DirectX::XMFLOAT2 & center, float angle, const DirectX::XMFLOAT3 & color)
		{
			m.primitive->rect(m.context, pos, size, center, angle, { color.x,color.y,color.z,pSystem->alpha });
		}

		//------------------------------------------------------
		//  線描画
		//------------------------------------------------------
		void line(
			float x1, float y1, float x2, float y2,
			float r, float g, float b,
			float width)
		{
			line({ x1, y1 }, { x2, y2 }, { r, g, b }, width);
		}

		//--------------------------------
		//  線描画
		//--------------------------------
		void line(
			const DirectX::XMFLOAT2& from, const DirectX::XMFLOAT2& to,
			const DirectX::XMFLOAT3& color,
			float width)
		{
			m.primitive->line(m.context, from, to, { color.x,color.y,color.z,pSystem->alpha }, width);
		}

		//------------------------------------------------------
		//  円描画
		//------------------------------------------------------
		void circle(
			float x, float y, float radius,
			float r, float g, float b,
			int n)
		{
			circle({ x, y }, radius, { r, g, b }, n);
		}

		//------------------------------------------------------
		//  円描画
		//------------------------------------------------------
		void circle(
			const DirectX::XMFLOAT2& pos, float radius,
			const DirectX::XMFLOAT3& color,
			int n)
		{
			m.primitive->circle(m.context, pos, radius, { color.x,color.y,color.z,pSystem->alpha }, n);
		}

		//------------------------------------------------------
		//  四角ポリゴン描画（頂点指定）
		//------------------------------------------------------
		void quad(const DirectX::XMFLOAT2(&v)[4], float r, float g, float b)
		{
			quad(v, { r, g, b });
		}

		//------------------------------------------------------
		//  四角ポリゴン描画（頂点指定）
		//------------------------------------------------------
		void quad(const DirectX::XMFLOAT2(&v)[4], const DirectX::XMFLOAT3& color)
		{
			m.primitive->quad(m.context, v, { color.x,color.y,color.z,pSystem->alpha });
		}

	}

	namespace light
	{
		void setLineLight(const DirectX::XMFLOAT4& _lightDirection,const DirectX::XMFLOAT4& _lightColor)
		{
			m.LineLight.setLineLight(_lightDirection,_lightColor);
		}
		void setLineLight(const float x, const float y, const float z, const float w, const float r, const float g, const float b, const float cw)
		{
			m.LineLight.setLineLight({ x, y, z, w }, {r,g,b,cw});
		}

		line_light& getLineLight()
		{
			return m.LineLight;
		}

		void setPointLight(const PointLight& _info)
		{
			int number = m.pointLights.size();
			m.pointLights.emplace_back();
			m.pointLights.back().setPointLight(_info);
			m.pointLights.back().setIndex(number);
		}

		void setPointlightInfo(const int index, const PointLight& _info)
		{
			m.pointLights[index].setPointLight(_info);
		}

		void removePointLight(const int i)
		{
			m.pointLights.erase(m.pointLights.begin() + i);
		}

		void resetPointLight()
		{
			m.pointLights.erase(m.pointLights.begin(), m.pointLights.end());

		}

		std::vector<point_light>& getPointLight()
		{
			return m.pointLights;
		}

	}

	namespace staticMesh
	{
		void createCube(static_mesh* _primitive)
		{
			_primitive->createCube(m.device);
		}

		void createSphere(static_mesh* _sphere, u_int slices, u_int stacks)
		{
			_sphere->createSphere(m.device, slices, stacks);
		}

		void createPlane(static_mesh* _plane, u_int _vertical, u_int _side)
		{
			_plane->createPlane(m.device, _vertical, _side);
		}

		void loadMesh(static_mesh* _mesh, const wchar_t* objName)
		{
			_mesh->loadStaticMesh(m.device, objName);
		}
		void loadMeshMTL(static_mesh* _mesh, const wchar_t* objName, const wchar_t* mtlName)
		{
			_mesh->loadStaticMeshMTL(m.device, objName, mtlName);
		}

		static_mesh::primitive_material& getPrimitiveMaterial(static_mesh* _mesh)
		{
			return _mesh->getPrimitiveMaterial();
		}

		void staticMeshRender(
			static_mesh* _mesh,
			const DirectX::XMFLOAT4X4&SynthesisMatrix,
			const DirectX::XMFLOAT4X4&worldMatrix,
			const DirectX::XMFLOAT4&camPos,
			line_light& _line_light,
			std::vector<point_light>& _point_light,
			const DirectX::XMFLOAT4&materialColor,
			bool wireFlg
		)
		{
			_mesh->render(m.context, SynthesisMatrix, worldMatrix, camPos, _line_light, _point_light, materialColor, wireFlg);
		}

		void staticMeshShadowRender1(static_mesh* _mesh,
			const DirectX::XMFLOAT4X4&SynthesisMatrix,
			const DirectX::XMFLOAT4X4&worldMatrix
		)
		{
			_mesh->renderFirst(m.context, SynthesisMatrix, worldMatrix);
		}
		void staticMeshShadowRender2(
			static_mesh* _mesh,
			const DirectX::XMFLOAT4X4&SynthesisMatrix,
			const DirectX::XMFLOAT4X4&worldMatrix,
			const DirectX::XMFLOAT4&lightDirection,
			const DirectX::XMFLOAT4&color,
			ID3D11ShaderResourceView* shadowMap,
			bool wireFlg
		)
		{
			_mesh->renderSecond(m.context, SynthesisMatrix, worldMatrix, lightDirection, color, shadowMap, wireFlg);
		}

	}

	namespace skinnedMesh
	{
		void loadFBX(skinned_mesh* _mesh, const std::string& _fbxName)
		{
			_mesh->setInfo(m.device, _fbxName);
		}

		void setLoopFlg(skinned_mesh* _mesh, const bool _is_loop)
		{
			_mesh->setLoopFlg(_is_loop);
		}
		void setStopAnimation(skinned_mesh* _mesh, const bool _is_stop)
		{
			_mesh->setStopAnimation(_is_stop);
		}

		void setStopTime(skinned_mesh* _mesh, const float _stop_time)
		{
			_mesh->setStoptimer(_stop_time);
		}

		void setAnimFlame(skinned_mesh* _mesh, const int _anim_flame)
		{
			_mesh->setAnimFlame(_anim_flame);
		}

		bool calcTransformedPosBySpecifyMesh(skinned_mesh* _mesh, DirectX::XMFLOAT3& _pos, std::string _mesh_name)
		{
			return _mesh->calcTransformedPosBySpecifyMesh(_pos, _mesh_name);
		}

		const int getAnimFlame(skinned_mesh* _mesh)
		{
			return _mesh->getAnimFlame();
		}


		void skinnedMeshRender(
			skinned_mesh* _mesh,
			const DirectX::XMFLOAT4X4&SynthesisMatrix,
			const DirectX::XMFLOAT4X4&worldMatrix,
			const DirectX::XMFLOAT4&camPos,
			line_light& lineLight,
			std::vector<point_light>& _point_light,
			const DirectX::XMFLOAT4&materialColor,
			bool wireFlg
		)
		{
			_mesh->render(m.context, SynthesisMatrix, worldMatrix,camPos, lineLight,_point_light, materialColor, wireFlg, m.hrTimer.time_interval());
		}
	}

	namespace camera
	{
		DirectX::XMMATRIX GetProjectionMatrix()
		{
			return m.cam.GetProjectionMatrix();
		}

		DirectX::XMMATRIX GetViewMatrix()
		{
			return m.cam.GetViewMatrix();
		}

		DirectX::XMMATRIX GetLightProjectionMatrix()
		{
			return m.cam.GetLightProjectionMatrix();
		}
		DirectX::XMMATRIX GetLightViewMatrix(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _direct)
		{
			return m.cam.GetLightViewMatrix(_pos, _direct);
		}

		void setPos(const DirectX::XMFLOAT3& _pos)
		{
			m.cam.setPosition(_pos);
		}

		void setTarget(const DirectX::XMFLOAT3& _target)
		{
			m.cam.setTarget(_target);
		}

		DirectX::XMFLOAT4 getPos()
		{
			return m.cam.getCameraPos();
		}
	}


	namespace font
	{
		//  文字の描画
		float textOut(
			int fontNo, std::wstring str,
			float x, float y, float scaleX, float scaleY,
			float r, float g, float b, float a)
		{
			return FontManager::getInstance()->textOut(m.context,
				fontNo, str,
				x, y, scaleX, scaleY,
				r, g, b, a);
		}

		//  文字列のフォーマット
		LPCSTR FormatString(LPCSTR format, ...)
		{
			static CHAR buf[1024];
			va_list args;
			va_start(args, format);
			vsnprintf_s(buf, sizeof(buf), 1024, format, args);
			va_end(args);
			return buf;
		}
	}


	namespace input
	{
		namespace xInput
		{
			int getState()
			{
				int index = 0;
				for (int i = 0; i < 4; i++)
				{
					m.prevPad[i].pad = m.pad[i].pad;

					if (XInputGetState(i, &m.pad[index].pad) == ERROR_SUCCESS)
						index++;
				}

				return index + 1;//padの個数
			}

			int pressedButtons(int _padNum, int _button)
			{
				if (!m.prevPad[_padNum].pressedButton(_button))
				{
					if (!m.pad[_padNum].pressedButton(_button))
						return 0;	//押してない
					else return 1;	//押した瞬間
				}
				else
				{
					if (!m.pad[_padNum].pressedButton(_button))
						return -1;	//離した瞬間
					else return 2;	//押しっぱなし
				}

			}

			DirectX::XMINT2 getThumbL(int _padNum)
			{
				return m.pad[_padNum].getThumbL();
			}

			DirectX::XMINT2 getThumbR(int _padNum)
			{
				return m.pad[_padNum].getThumbR();
			}

		}
	}
}

int GameLib::input::keyboard::getState(int _keyNum)
{
	return m.keyboard.getState(_keyNum);
}

void GameLib::input::keyboard::update()
{
	m.keyboard.update();
}
