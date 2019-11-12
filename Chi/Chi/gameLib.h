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

	//���C�u����
	//������
	void init(LPCTSTR, HINSTANCE, int width = (640), int height = (480),
		int iconNum = -1, bool isFullscreen = (false), double frameRate = (0.0));
	//�I��
	void uninit();

	//�Q�[�����[�v
	bool gameLoop(bool isShowFrameRate = (false));

	//�o�b�N�o�b�t�@�̓��e��\��
	HRESULT present(UINT SyncInterval, UINT Flags);

	//�����_�[�^�[�Q�b�g�̐ݒ�
	bool createRenderTarget(ID3D11RenderTargetView* _renderTarget);
	bool setRenderTarget(ID3D11RenderTargetView** _renderTarget);
	bool resetRendertarget();

	//�V���h�E�}�b�v����@
	bool createRT(ID3D11RenderTargetView** _renderTarget);
	bool createSRV(ID3D11ShaderResourceView** _shaderResource,ID3D11RenderTargetView** RT);


	//��ʂ��w��F�ɃN���A
	void clear(const DirectX::XMFLOAT4&);
	void clearRT(ID3D11RenderTargetView* RT, const DirectX::XMFLOAT4& color);


	//�h���b�O���h���b�v�֘A
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

	//�E�C���h�E�̐ݒ�
	namespace window
	{
		//������
		HWND init(LPCTSTR, HINSTANCE, int, int, int);
		//���
		void uninit();

	}

	//DirectX11�֘A
	namespace DirectX11
	{

		//������
		HRESULT init(HWND, int, int, bool);
		//���
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
	//�e�N�X�`���[
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

	//�v���~�e�B�u
	namespace primitive
	{
		//------------------------------------------------------
		//  ��`�`��
		//------------------------------------------------------
		//  float x         �`��ʒux     float y        �`��ʒuy
		//  float w         ��            float h        ����
		//  float centerX   ��_x       float centerY  ��_y
		//  float angle     �p�x (radian)
		//  float r         �F r (0.0f ~ 1.0f)
		//  float g         �F g (0.0f ~ 1.0f)
		//  float b         �F b (0.0f ~ 1.0f)
		//  float a         �F a (0.0f ~ 1.0f)
		//------------------------------------------------------
		void rect(float, float, float, float,
			float cx = (0), float cy = (0), float angle = (0),
			float r = (1), float g = (1), float b = (1));

		//------------------------------------------------------
		//  ��`�`��
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2& pos      �`��ʒu (x, y)
		//  const DirectX::XMFLOAT2& size     ������   (w, h)
		//  const DirectX::XMFLOAT2& center   ��_   (x, y)
		//  float angle             �p�x     (radian)
		//  const DirectX::XMFLOAT4& color    �F       (r, g, b, a) (0.0f ~ 1.0f)
		//------------------------------------------------------
		void rect(const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
			const DirectX::XMFLOAT2& center = (DirectX::XMFLOAT2(0, 0)), float angle = (0),
			const DirectX::XMFLOAT3& color = (DirectX::XMFLOAT3(1, 1, 1)));

		//------------------------------------------------------
		//  ���`��
		//------------------------------------------------------
		//  float x1        �n�_ x    float y1    // �n�_ y
		//  float x2        �I�_ x    float y2    // �I�_ y
		//  float r         �F r (0.0f ~ 1.0f)
		//  float g         �F g (0.0f ~ 1.0f)
		//  float b         �F b (0.0f ~ 1.0f)
		//  float a         �F a (0.0f ~ 1.0f)
		//  float width     ��
		//------------------------------------------------------
		void line(float, float, float, float,
			float r = (1), float g = (1), float b = (1),
			float width = (1));

		//------------------------------------------------------
		//  ���`��
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2& from     �n�_ (x, y)
		//  const DirectX::XMFLOAT2& to       �I�_ (x, y)
		//  const DirectX::XMFLOAT4& color    �F   (r, g, b, a)
		//  float width             ��
		//------------------------------------------------------
		void line(const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
			const DirectX::XMFLOAT3& color = (DirectX::XMFLOAT3(1, 1, 1)), float width = (1));

		//------------------------------------------------------
		//  �~�`��
		//------------------------------------------------------
		//  float x         ���S�ʒu x    float y      // ���S�ʒu y
		//  float radius    ���a
		//  float r         �F r (0.0f ~ 1.0f)
		//  float g         �F g (0.0f ~ 1.0f)
		//  float b         �F b (0.0f ~ 1.0f)
		//  float a         �F a (0.0f ~ 1.0f)
		//  int n           ���p�`��
		//------------------------------------------------------
		void circle(float, float, float,
			float r = (1), float g = (1), float b = (1),
			int n = (32));

		//------------------------------------------------------
		//  �~�`��
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2& pos      ���S�ʒu (x, y)
		//  float radius            ���a
		//  const DirectX::XMFLOAT4& color    �F   (r, g, b, a)
		//  int n                   ���p�`��
		//------------------------------------------------------
		void circle(const DirectX::XMFLOAT2&, float,
			const DirectX::XMFLOAT3& color = (DirectX::XMFLOAT3(1, 1, 1)), int n = (32));

		//------------------------------------------------------
		//  �l�p�|���S���`��i���_�w��j
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2(&v)[4]    ���_�̔z��i�傫��4�j
		//  float r     �F r (0.0f ~ 1.0f)
		//  float g     �F g (0.0f ~ 1.0f)
		//  float b     �F b (0.0f ~ 1.0f)
		//  float a     �F a (0.0f ~ 1.0f)
		//------------------------------------------------------
		void quad(const DirectX::XMFLOAT2(&)[4],
			float r = (1), float g = (1), float b = (1));

		//------------------------------------------------------
		//  �l�p�|���S���`��i���_�w��j
		//------------------------------------------------------
		//  const DirectX::XMFLOAT2(&v)[4]    ���_�̔z��i�傫��4�j
		//  const DirectX::XMFLOAT4& color    �F (r, g, b, a)
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
	//���y��
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

	//�J��������
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

	//�f�o�b�N
	namespace debug
	{
		//--------------------------------
		//  �f�o�b�O��������Z�b�g����(��ʂɕ\������)
		//--------------------------------
		void drawDebug(DirectX::XMINT2 _pos, std::wstring str);

	}

	//�e�L�X�g����
	namespace font
	{
		float textOut(int, std::wstring,
			float, float, float scaleX = (1), float scaleY = (1),
			float r = (1), float g = (1), float b = (1), float a = (1));

		LPCSTR FormatString(LPCSTR format, ...);
	}

	//�C���v�b�g����
	namespace input
	{
		namespace xInput
		{
			int getState();
			///<summary>
			///0 : �����ĂȂ� 1 : �������u�� 2 : �������ςȂ� -1 : �������u��
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


