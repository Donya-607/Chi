#ifndef STATIC_MESH_H_
#define STATIC_MESH_H_


#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <fstream>
#include <d3d11.h>
#include <DirectXMath.h>
#include "misc.h"
#include <vector>
#include "resourceManager.h"
#include "light.h"
#include "system.h"
#include "Donya/Quaternion.h"
using namespace Donya;

class static_mesh
{
public:
	enum shaderNumber
	{
		Phong,
		Rambart,
		END,
	};

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};


	struct Material
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 camPos;
		LineLight lineLight;
		PointLight pntLight[5];
		Material b_material;
		DirectX::XMFLOAT4 judge_color;
		DirectX::XMFLOAT4 screenSize;
	};

	struct shadow_cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 lightDirection;

	};

	struct subset
	{
		std::wstring usemtl;//マテリアル名
		std::vector<vertex> m_vertexes;
		u_int index_start = 0;//index bufferの開始位置
		u_int index_count = 0;//vertexの個数
	};
	struct material
	{
		std::wstring newmtl;
		DirectX::XMFLOAT3 Ka = { 0.2f,0.2f,0.2f };
		DirectX::XMFLOAT3 Kd = { 0.8f,0.8f,0.8f };
		DirectX::XMFLOAT3 Ks = { 1.0f,1.0f,1.0f };
		u_int illum = 1;
		std::wstring map_Kd;//マテリアルファイル名
		ID3D11ShaderResourceView* Resource;
		D3D11_TEXTURE2D_DESC tex2dDesc;

	};

	struct primitive_material
	{
		DirectX::XMFLOAT4 ambient = { 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 diffuse = { 0.8f,0.8f,0.8f,1.0f };
	};
protected:
	ID3D11VertexShader* vertexShader;		//VertexShader
	ID3D11PixelShader* pixelShader;		//PixelShader
	ID3D11InputLayout* layout;				//inputLayout
	ID3D11Buffer* vertex_buffer;		//頂点バッファ
	ID3D11Buffer* index_buffer;		//インデックスバッファ
	ID3D11Buffer* constant_buffer;	//定数バッファ
	ID3D11RasterizerState* rasterizeLine;		//線描画
	ID3D11RasterizerState* rasterizeFillOut;	//塗りつぶし描画
	ID3D11DepthStencilState* depthStencilState;	//depthStencilState

	ID3D11PixelShader* z_PS;		//PixelShader
	ID3D11PixelShader* bloom_PS;		//PixelShader

	ID3D11PixelShader* noTex_z_PS;		//PixelShader
	ID3D11PixelShader* noTex_bloom_PS;		//PixelShader

	Quaternion orientation;

	int							numIndices;

	ID3D11VertexShader* noTexVS;		//VertexShader
	ID3D11PixelShader* noTexPS;		//PixelShader
	ID3D11InputLayout* noTexLayout;	//inputLayout

	std::vector<material> materials;//マテリアル情報のコンテナ
	primitive_material primitiveMaterial;
	ID3D11SamplerState* sampleState;

	void objInit(ID3D11Device* _device, const wchar_t* _objFileName, const std::wstring& _mtlFileNmae);
	void TextureInit(ID3D11Device* _device);
public:
	std::vector<subset> subsets;//メッシュ情報のコンテナ
	void init(ID3D11Device* device,
		std::string vsName, D3D11_INPUT_ELEMENT_DESC* inputElementDescs, int numElement,
		std::string psName);
	bool createBuffer(ID3D11Device* device,
		vertex* vertices, int numV,
		unsigned int* indices, int numI);

	static_mesh() : constant_buffer(nullptr), depthStencilState(nullptr), index_buffer(nullptr), layout(nullptr), noTexLayout(nullptr), noTexPS(nullptr), noTexVS(nullptr), numIndices(0), pixelShader(nullptr), vertexShader(nullptr), rasterizeFillOut(nullptr), rasterizeLine(nullptr), vertex_buffer(nullptr), sampleState(nullptr) {}
	~static_mesh();
	void loadStaticMesh(ID3D11Device* _device, const wchar_t* _objFileName);
	void loadStaticMeshMTL(ID3D11Device* _device, const wchar_t* _objFileName, const std::wstring& _mtlFileNmae);
	void createPrimitive(ID3D11Device* _device);
	void createCube(ID3D11Device* _device);
	void createPlane(ID3D11Device* _device, u_int _vertical = 1, u_int _side = 1);
	void createSphere(ID3D11Device* _device, u_int slices, u_int stacks);
	void createBillboard(ID3D11Device* _device, const wchar_t* _textureName);
	primitive_material& getPrimitiveMaterial()
	{
		return primitiveMaterial;
	}

	void render(
		ID3D11DeviceContext*,		//デバイスコンテキスト
		const DirectX::XMFLOAT4X4&,	//ワールドビュープロジェクション合成行列
		const DirectX::XMFLOAT4X4&,	//ワールド変換行列
		const DirectX::XMFLOAT4&,	//カメラ座標
		line_light& _line_light,	//平行光
		std::vector<point_light>& _point_light,	//ポイントライト
		const DirectX::XMFLOAT4&,	//材質色
		bool						//線・塗りつぶし描画フラグ
	);

	void z_render(
		ID3D11DeviceContext*,		//デバイスコンテキスト
		const DirectX::XMFLOAT4X4&,	//ワールドビュープロジェクション合成行列
		const DirectX::XMFLOAT4X4&,	//ワールド変換行列
		const DirectX::XMFLOAT4&
	);

	void bloom_SRV_render(
		ID3D11DeviceContext*,		//デバイスコンテキスト
		const DirectX::XMFLOAT4X4&,	//ワールドビュープロジェクション合成行列
		const DirectX::XMFLOAT4X4&,	//ワールド変換行列
		const DirectX::XMFLOAT4&,	//カメラ座標
		line_light& _line_light,	//平行光
		std::vector<point_light>& _point_light,	//ポイントライト
		const DirectX::XMFLOAT4&,	//材質色
		const DirectX::XMFLOAT3& judge_color,
		ID3D11ShaderResourceView* z_SRV
	);

	void billboardRender(
		ID3D11DeviceContext*,		//デバイスコンテキスト
		const DirectX::XMFLOAT4X4&,	//ビュープロジェクション合成行列
		const DirectX::XMFLOAT4&,	//ポジション
		const DirectX::XMFLOAT2,	//スケール
		const float,	//アングル
		const DirectX::XMFLOAT4&,	//カメラ座標
		const DirectX::XMFLOAT2& texpos,
		const DirectX::XMFLOAT2& texsize,
		const float alpha,
		const DirectX::XMFLOAT3& color
	);

	void billboard_z_render(
		ID3D11DeviceContext*,		//デバイスコンテキスト
		const DirectX::XMFLOAT4X4&,	//ビュープロジェクション合成行列
		const DirectX::XMFLOAT4&,	//ポジション
		const DirectX::XMFLOAT2,	//スケール
		const float,	//アングル
		const DirectX::XMFLOAT4&,	//カメラ座標
		const DirectX::XMFLOAT2& texpos,
		const DirectX::XMFLOAT2& texsize,
		const float alpha,
		const DirectX::XMFLOAT3& color
	);

	void billboard_bloom_SRV_render(
		ID3D11DeviceContext*,		//デバイスコンテキスト
		const DirectX::XMFLOAT4X4&,	//ビュープロジェクション合成行列
		const DirectX::XMFLOAT4&,	//ポジション
		const DirectX::XMFLOAT2,	//スケール
		const float,	//アングル
		const DirectX::XMFLOAT4&,	//カメラ座標
		const DirectX::XMFLOAT2& texpos,
		const DirectX::XMFLOAT2& texsize,
		const float alpha,
		const DirectX::XMFLOAT3& color,
		const DirectX::XMFLOAT4& judge_color,
		ID3D11ShaderResourceView* z_SRV
	);

	void renderFirst(
		ID3D11DeviceContext*,		//デバイスコンテキスト
		const DirectX::XMFLOAT4X4&,	//ワールドビュープロジェクション合成行列
		const DirectX::XMFLOAT4X4&	//ワールド変換行列
	);

	void renderSecond(
		ID3D11DeviceContext*,		//デバイスコンテキスト
		const DirectX::XMFLOAT4X4&,	//ワールドビュープロジェクション合成行列
		const DirectX::XMFLOAT4X4&,	//ワールド変換行列
		const DirectX::XMFLOAT4&,	//線光源
		const DirectX::XMFLOAT4&,	//材質色
		ID3D11ShaderResourceView*,	//シャドウマップ
		bool						//線・塗りつぶし描画フラグ
	);


};

#endif // !STATIC_MESH_H_
