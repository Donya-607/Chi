#ifndef SPRITE_H_
#define SPRITE_H_
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
#include "vector.h"
class Sprite
{
private:
	ID3D11VertexShader*		m_vertex;
	ID3D11PixelShader*		m_pixel;
	ID3D11InputLayout*		m_input;
	ID3D11Buffer*			m_buffer;
	ID3D11RasterizerState*  m_rasterize;
	ID3D11ShaderResourceView *m_shader;
	D3D11_TEXTURE2D_DESC m_tex2dDesc;
	ID3D11SamplerState *m_sampleState;
	ID3D11DepthStencilState *m_depth;
	vector2 size;
	struct vertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

public:
	Sprite() {}
	~Sprite() {}
	vector2 getSize() { return size; }
	void RoadSprite(ID3D11Device* _device, const wchar_t* _textureName);
	void Render(ID3D11DeviceContext* _dContext,
		float _dx, float _dy, float _dw, float _dh,
		float _sx, float _sy, float _sw, float _sh,
		float _angle, float _r, float _g, float _b, float _a,
		float _cx = 0,float _cy  =0,
		bool _flipX = false,bool _flipY = false) const;
	void textout(ID3D11DeviceContext* _dContext,
		std::string _string,
		float _x, float _y, float _w, float _h,
		float _r = 1, float _g = 1, float _b = 1, float _a = 1) const;
	ID3D11ShaderResourceView* getSRV()
	{
		return m_shader;
	}
};

class sprite_batch
{
private:
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;

	ID3D11Buffer* vertex_buffer;
	ID3D11RasterizerState* rasterizer_state;

	ID3D11ShaderResourceView* shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	ID3D11SamplerState* sampler_state;

	ID3D11DepthStencilState* depth_stencil_state;

	const size_t MAX_INSTANCES = 10240;
	struct instance
	{
		DirectX::XMFLOAT4X4 ndc_transform;
		DirectX::XMFLOAT4 texcoord_transform;
		DirectX::XMFLOAT4 color;
	};
	ID3D11Buffer* instance_buffer;

public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	sprite_batch(ID3D11Device *device, const wchar_t *file_name, size_t max_instance = 10240);

	void begin(ID3D11DeviceContext *immediate_context);
	void render
	(
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		float angle, float r, float g, float b, float a);
	float textout(std::string s,
		float x, float y, float sx, float sy,
		float r, float g, float b, float a);

	void end(ID3D11DeviceContext *immediate_context);

private:
	D3D11_VIEWPORT viewport;

	UINT count_instance = 0;
	instance *instances = nullptr;
};





//==========================================================================
class SpriteBatch
{
private:
	ID3D11VertexShader*         vertexShader;
	ID3D11PixelShader*          pixelShader;
	ID3D11InputLayout*          inputLayout;
	ID3D11Buffer*               buffer;
	ID3D11RasterizerState*      rasterizerState;

	ID3D11ShaderResourceView*   shaderResourceView;
	D3D11_TEXTURE2D_DESC        tex2dDesc;
	ID3D11SamplerState*         samplerState;
	ID3D11DepthStencilState*    depthStencilState;

	size_t MAX_INSTANCES;

	//--------------------------------
	//  構造体定義
	//--------------------------------
	struct instance
	{
		DirectX::XMFLOAT4X4 ndcTransform;
		DirectX::XMFLOAT4 texcoordTransform;
		DirectX::XMFLOAT4 color;
	};
	ID3D11Buffer* instanceBuffer;

public:

	//--------------------------------
	//  構造体定義
	//--------------------------------
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	//------------------------------------------------------
	//  コンストラクタ
	//------------------------------------------------------
	SpriteBatch(ID3D11Device*, const wchar_t*, size_t);

	//------------------------------------------------------
	//  デストラクタ
	//------------------------------------------------------
	~SpriteBatch();

	//------------------------------------------------------
	//  前処理
	//------------------------------------------------------
	void begin(ID3D11DeviceContext*);

	//------------------------------------------------------
	//  スプライトバッチ描画
	//------------------------------------------------------
	void render(
		const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
		const DirectX::XMFLOAT2&, const DirectX::XMFLOAT2&,
		const DirectX::XMFLOAT2&, float angle,
		const DirectX::XMFLOAT4&);

	//------------------------------------------------------
	//  スプライトバッチ描画
	//------------------------------------------------------
	void render(
		float, float, float, float,
		float, float, float, float,
		float, float, float,
		float, float, float, float);

	//------------------------------------------------------
	//  テキスト描画
	//------------------------------------------------------
	float textout(
		std::wstring, const DirectX::XMFLOAT2&,
		const DirectX::XMFLOAT2&, const DirectX::XMFLOAT4&);

	//------------------------------------------------------
	//  テキスト描画
	//------------------------------------------------------
	float textout(
		std::wstring, float, float,
		float, float, float, float, float, float);

	//------------------------------------------------------
	//  後処理
	//------------------------------------------------------
	void end(ID3D11DeviceContext*);

private:

	D3D11_VIEWPORT viewport;

	UINT instanceCount = 0;
	instance* instances = nullptr;
};





#endif // !SPRITE_H_
