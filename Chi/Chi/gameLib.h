#ifndef GAMELIB_H_
#define GAMELIB_H_

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <Keyboard.h>
#include <xaudio2.h>
#include <mmsystem.h>
#include <ctime>
#include "util.h"
#include "misc.h"
#include "dragDrop.h"
#include "blend.h"
#include "font.h"
#include "primitive.h"
#include "light.h"
#include "static_mesh.h"
#include "skinned_mesh.h"
#include "high_resolution_timer.h"
#include "frame_rate.h"
#include "system.h"
#include "Camera.h"
#include "bloom.h"
#include "XinputPad.h"
#include "keyInput.h"
namespace GameLib
{

	HWND  getHandle();
	ID3D11Device*	getDevice();
	ID3D11DeviceContext*  getContext();
	float getDeltaTime();

	//ライブラリ
	//初期化
	void init(LPCTSTR, HINSTANCE, int width = (640), int height = (480),
		int iconNum = -1, bool isFullscreen = (false), double frameRate = (0.0));
	//終了
	void uninit();

	//ゲームループ
	bool gameLoop(bool isShowFrameRate = (false));

	//バックバッファの内容を表示
	HRESULT present(UINT SyncInterval, UINT Flags);

	//レンダーターゲットの設定
	bool createRenderTarget(ID3D11RenderTargetView* _renderTarget);
	bool setRenderTarget(ID3D11RenderTargetView** _renderTarget);
	bool resetRendertarget();

	//シャドウマップ周り　
	bool createRT(ID3D11RenderTargetView** _renderTarget);
	bool createSRV(ID3D11ShaderResourceView** _shaderResource,ID3D11RenderTargetView** RT);


	//画面を指定色にクリア
	void clear(const DirectX::XMFLOAT4&);
	void clearRT(ID3D11RenderTargetView* RT, const DirectX::XMFLOAT4& color);


	//ドラッグ＆ドロップ関連
	void loadFile(LPWSTR _filename);


	void postEffect_Bloom_SRV(ID3D11ShaderResourceView** _shaderResource, DirectX::XMFLOAT4 _judge_color);
	void postEffect_Bloom(ID3D11ShaderResourceView** _shaderResource,float _blur_value,DirectX::XMFLOAT4 _judge_color);
	
	//other
	std::wstring getLoadedFileName();
	int getLoadedFileCount();
	bool getLoadFlg();

	namespace blend
	{
		void setBlendMode(int _blendNum, const float alpha);

		enum blendNumber
		{
			NONE = 0,
			ALPHA,
			ADD,
			SUBTRACT,
			REPLACE,
			MULTIPLY,
			LIGHTEN,
			DARKEN,
			SCREEN,
			END
		};

	}

	//ウインドウの設定
	namespace window
	{
		//初期化
		HWND init(LPCTSTR, HINSTANCE, int, int, int);
		//解放
		void uninit();

	}

	//DirectX11関連
	namespace DirectX11
	{

		//初期化
		HRESULT init(HWND, int, int, bool);
		//解放
		void uninit();

	}


	DirectX::XMINT2 getWindowSize();

	//-------------------------------------------------------------------//
	//
	//
	//							2D
	//
	//
	//-------------------------------------------------------------------//
	//テクスチャー
	namespace texture
	{
		void SpriteRender(Sprite*,
			float _dx, float _dy, float _dw, float _dh,
			float _sx, float _sy, float _sw, float _sh,
			float _angle, float _r, float _g, float _b,
			float _cx, float _cy, bool _flipX, bool _flipY
		);

		void SpriteRoad(Sprite*, std::wstring texName);
	}

	//プリミティブ
	namespace primitive
	{
		//------------------------------------------------------
		//  矩形描画
		//------------------------------------------------------
		//  float x         描画位置x     float y        描画位置y
		//  float w         幅            float h        高さ
		//  float centerX   基準点x       float centerY  基準点y
		//  float angle     角度 (radian)
		//  float r         色 r (0.0f ~ 1.0f)
		//  float g         色 g (0.0f ~ 1.0f)
		//  float b         色 b (0.0f ~ 1.0f)
		//  float a         色 a (0.0f ~ 1.0f)
		//------------------------------------------------------
		void rect(float, float, float, float,
			float cx = (0), float cy = (0), float angle = (0),
			float r = (1), float g = (1), float b = (1));

		//------------------------------------------------------
		//  矩形描画
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2& pos      描画位置 (x, y)
		//  const DirectX::XMFLOAT2& size     幅高さ   (w, h)
		//  const DirectX::XMFLOAT2& center   基準点   (x, y)
		//  float angle             角度     (radian)
		//  const DirectX::XMFLOAT4& color    色       (r, g, b, a) (0.0f ~ 1.0f)
		//------------------------------------------------------
		void rect(const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
			const DirectX::XMFLOAT2& center = (DirectX::XMFLOAT2(0, 0)), float angle = (0),
			const DirectX::XMFLOAT3& color = (DirectX::XMFLOAT3(1, 1, 1)));

		//------------------------------------------------------
		//  線描画
		//------------------------------------------------------
		//  float x1        始点 x    float y1    // 始点 y
		//  float x2        終点 x    float y2    // 終点 y
		//  float r         色 r (0.0f ~ 1.0f)
		//  float g         色 g (0.0f ~ 1.0f)
		//  float b         色 b (0.0f ~ 1.0f)
		//  float a         色 a (0.0f ~ 1.0f)
		//  float width     幅
		//------------------------------------------------------
		void line(float, float, float, float,
			float r = (1), float g = (1), float b = (1),
			float width = (1));

		//------------------------------------------------------
		//  線描画
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2& from     始点 (x, y)
		//  const DirectX::XMFLOAT2& to       終点 (x, y)
		//  const DirectX::XMFLOAT4& color    色   (r, g, b, a)
		//  float width             幅
		//------------------------------------------------------
		void line(const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
			const DirectX::XMFLOAT3& color = (DirectX::XMFLOAT3(1, 1, 1)), float width = (1));

		//------------------------------------------------------
		//  円描画
		//------------------------------------------------------
		//  float x         中心位置 x    float y      // 中心位置 y
		//  float radius    半径
		//  float r         色 r (0.0f ~ 1.0f)
		//  float g         色 g (0.0f ~ 1.0f)
		//  float b         色 b (0.0f ~ 1.0f)
		//  float a         色 a (0.0f ~ 1.0f)
		//  int n           何角形か
		//------------------------------------------------------
		void circle(float, float, float,
			float r = (1), float g = (1), float b = (1),
			int n = (32));

		//------------------------------------------------------
		//  円描画
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2& pos      中心位置 (x, y)
		//  float radius            半径
		//  const DirectX::XMFLOAT4& color    色   (r, g, b, a)
		//  int n                   何角形か
		//------------------------------------------------------
		void circle(const DirectX::XMFLOAT2&, float,
			const DirectX::XMFLOAT3& color = (DirectX::XMFLOAT3(1, 1, 1)), int n = (32));

		//------------------------------------------------------
		//  四角ポリゴン描画（頂点指定）
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2(&v)[4]    頂点の配列（大きさ4）
		//  float r     色 r (0.0f ~ 1.0f)
		//  float g     色 g (0.0f ~ 1.0f)
		//  float b     色 b (0.0f ~ 1.0f)
		//  float a     色 a (0.0f ~ 1.0f)
		//------------------------------------------------------
		void quad(const DirectX::XMFLOAT2(&)[4],
			float r = (1), float g = (1), float b = (1));

		//------------------------------------------------------
		//  四角ポリゴン描画（頂点指定）
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2(&v)[4]    頂点の配列（大きさ4）
		//  const DirectX::XMFLOAT4& color    色 (r, g, b, a)
		//------------------------------------------------------
		void quad(const DirectX::XMFLOAT2(&)[4],
			const DirectX::XMFLOAT3& color = (DirectX::XMFLOAT3(1, 1, 1)));
	}

	//-------------------------------------------------------------------//
	//
	//
	//							3D
	//
	//
	//-------------------------------------------------------------------//

	//light
	namespace light
	{
		void setLineLight(const DirectX::XMFLOAT4& _lightDirection,const DirectX::XMFLOAT4& _lightColor);
		void setLineLight(const float x, const float y, const float z, const float w,const float r,const float g,const float b,const float cw);

		line_light &getLineLight();

		void setPointLight(const PointLight& _info);
		void setPointlightInfo(const int index, const PointLight& _info);
		void removePointLight(const int i);
		void resetPointLight();
		std::vector<point_light>& getPointLight();
	}

	//static_mesh
	namespace staticMesh
	{
		void createCube(static_mesh*);
		void createSphere(static_mesh*, u_int, u_int);
		void createPlane(static_mesh*, u_int, u_int);
		void createBillboard(static_mesh*, const wchar_t* _textureName);
		void loadMesh(static_mesh* _mesh, const wchar_t* objName);
		void loadMeshMTL(static_mesh* _mesh, const wchar_t* objName, const wchar_t* mtlName);
		static_mesh::primitive_material& getPrimitiveMaterial(static_mesh* _mesh);
		void staticMeshRender(static_mesh* _mesh, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4&, line_light& _line_light, std::vector<point_light>& _point_light, const DirectX::XMFLOAT4&, bool);
		void builboradRender(static_mesh* _mesh,const DirectX::XMFLOAT4X4&,const DirectX::XMFLOAT4&,const DirectX::XMFLOAT2,const float,const DirectX::XMFLOAT4&,const DirectX::XMFLOAT2& texpos,const DirectX::XMFLOAT2& texsize,const float alpha,const DirectX::XMFLOAT3& color);
		void staticMeshShadowRender1(static_mesh* _mesh, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&);
		void staticMeshShadowRender2(static_mesh* _mesh, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4X4&, const DirectX::XMFLOAT4&,const DirectX::XMFLOAT4&,ID3D11ShaderResourceView* ,bool);
	}

	//skinned_mesh
	namespace skinnedMesh
	{
		void loadFBX(skinned_mesh*, const std::string& _fbxName,bool is_Tpose);

		void loadShader(fbx_shader& shader, std::string vertex, std::string pixel, std::string noBoneVertex, std::string notexPS);

		void setLoopanimation(skinned_mesh* _mesh, const bool _loop_flg);
		void setStopAnimation(skinned_mesh* _mesh, const bool _is_stop);
		void setStopTime(skinned_mesh* _mesh, const float _stop_time);
		void setAnimFlame(skinned_mesh* _mesh, const int _anim_flame);
		const int getAnimFlame(skinned_mesh*);

		bool calcTransformedPosBySpecifyMesh(skinned_mesh* _mesh, DirectX::XMFLOAT3& _pos, std::string _mesh_name);
		bool calcTransformedPosBySpecifyMesh(skinned_mesh* _mesh, DirectX::XMFLOAT3& _pos, std::string _mesh_name, bone_animation* anim);

		void skinnedMeshRender(
			skinned_mesh* _mesh,
			fbx_shader& hlsl,
			float magnification,
			const DirectX::XMFLOAT4X4& SynthesisMatrix,
			const DirectX::XMFLOAT4X4& worldMatrix,
			const DirectX::XMFLOAT4& camPos,
			line_light& lineLight,
			std::vector<point_light>& _point_light,
			const DirectX::XMFLOAT4& materialColor,
			bool wireFlg
		);


		void skinnedMeshRender(
			skinned_mesh* _mesh,
			fbx_shader& hlsl,
			bone_animation* anim,
			const DirectX::XMFLOAT4X4&SynthesisMatrix,
			const DirectX::XMFLOAT4X4&worldMatrix,
			const DirectX::XMFLOAT4&camPos,
			line_light& lineLight,
			std::vector<point_light>& _point_light,
			const DirectX::XMFLOAT4&materialColor,
			bool wireFlg
		);

		void loadAnimation(bone_animation*, std::string _anim_name);
		void playAnimation(bone_animation* anim, float magnification = 1.0f, bool _is_loop = true);

		void setStopAnimation(bone_animation* _mesh, const bool _is_stop);
		void setStopTime(bone_animation* _mesh, const float _stop_time);
		void setAnimFlame(bone_animation* _mesh, const int _anim_flame);
		const int getAnimFlame(bone_animation*);

	}

	//--------------------------------------------------------------//
	//
	//
	//音楽類
	//
	//
	//--------------------------------------------------------------//
	//BGM
	namespace music
	{

	}
	//SE
	namespace sound
	{

	}

	//カメラ周り
	namespace camera
	{
		DirectX::XMMATRIX GetProjectionMatrix();
		DirectX::XMMATRIX GetViewMatrix();
		DirectX::XMMATRIX GetLightProjectionMatrix();
		DirectX::XMMATRIX GetLightViewMatrix(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _direct);
		void setPos(const DirectX::XMFLOAT3& _pos);
		void setTarget(const DirectX::XMFLOAT3& _target);
		DirectX::XMFLOAT4 getPos();
		DirectX::XMFLOAT4 getTarget();
	}

	//デバック
	namespace debug
	{
		//--------------------------------
		//  デバッグ文字列をセットする(画面に表示する)
		//--------------------------------
		void drawDebug(DirectX::XMINT2 _pos, std::wstring str);

	}

	//テキスト周り
	namespace font
	{
		float textOut(int, std::wstring,
			float, float, float scaleX = (1), float scaleY = (1),
			float r = (1), float g = (1), float b = (1), float a = (1));

		LPCSTR FormatString(LPCSTR format, ...);
	}

	//インプット周り
	namespace input
	{
		namespace xInput
		{
			int getState();
			///<summary>
			///0 : 押してない 1 : 押した瞬間 2 : 押しっぱなし -1 : 離した瞬間
			///</summary>
			int pressedButtons(int _padNum, int _button);

			DirectX::XMINT2 getThumbL(int _padNum);
			DirectX::XMINT2 getThumbR(int _padNum);

		}

		namespace keyboard
		{
			int getState(int _keyNum);

			void update();
		}
	}

}

#endif // !GAMELIB_H_


