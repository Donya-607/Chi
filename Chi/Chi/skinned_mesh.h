#ifndef SKINNED_MESH_H_
#define SKINNED_MESH_H_

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <fstream>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fbxsdk.h>
#include <functional>
#include "misc.h"
#include <vector>
#include "./liblary/DirectXTex-master/DirectXTex/DirectXTex.h"
#include "resourceManager.h"
#include "light.h"

#define MAX_BONE_INFLUENCES 4
#define MAX_BONES 32

class skinned_mesh
{
public:
	struct bone
	{
		DirectX::XMFLOAT4X4 transform;
	};
	typedef std::vector<bone> skeletal;
	struct skeletal_animation : public std::vector<skeletal>
	{
		float sampling_time = 1 / 24.0f;
		float animation_tick = 0.0f;
	};
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
		FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 1,0,0,0 };
		INT bone_indices[MAX_BONE_INFLUENCES] = {};
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
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	};

	struct property
	{
		float factor = 1;
		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f }; // w channel is used as shininess by only specular.
		std::string texture_filename;
		float scale_u = 1.0;
		float scale_v = 1.0;

		DirectX::TexMetadata metadata = {};
		ID3D11ShaderResourceView* shader_resource_view;

		template<class Archive>
		void serialize(Archive &archive)
		{
			archive
			(
				factor,
				color.w, color.x, color.y, color.z,
				texture_filename,
				scale_u, scale_v
			);
		}
	};

	struct subset
	{
		property ambient;
		property diffuse;
		property specular;
		u_int index_start = 0;//index bufferの開始位置
		u_int index_count = 0;//vertexの個数
	};

	struct mesh
	{
		ID3D11Buffer* vertex_buffer;
		ID3D11Buffer* index_buffer;
		std::vector<subset> subsets;

		DirectX::XMFLOAT4X4 global_transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		skeletal_animation skeletal_animation;
	};

	struct bone_influence
	{
		int index;
		float weight;
	};
	DirectX::XMFLOAT4X4 coordinate_conversion = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
	};

protected:
	typedef std::vector<bone_influence> bone_influences_per_control_point;
	std::vector<mesh>meshes;
	D3D11_TEXTURE2D_DESC tex2dDesc;
	std::string vsName;
	std::string psName;
	bool have_uv;
	bool have_born;
	bool have_material;
	ID3D11SamplerState* sampleState;

	ID3D11VertexShader*			vertexShader;		//VertexShader
	ID3D11PixelShader*			pixelShader;		//PixelShader
	ID3D11InputLayout*			layout;				//inputLayout
	ID3D11VertexShader*			noTexVS;		//VertexShader
	ID3D11PixelShader*			noTexPS;		//PixelShader
	ID3D11InputLayout*			noTexLayout;				//inputLayout
	ID3D11Buffer*				constant_buffer;	//定数バッファ
	ID3D11RasterizerState*		rasterizeLine;		//線描画
	ID3D11RasterizerState*		rasterizeFillOut;	//塗りつぶし描画
	ID3D11DepthStencilState*	depthStencilState;	//depthStencilState

	int							numIndices;

	void fbxInit(ID3D11Device* _device, const std::string& _objFileName);

public:
	skinned_mesh() {}
	~skinned_mesh() {}
	void setInfo(ID3D11Device* _device, const std::string& _objFileName);

	void init(ID3D11Device* device,
		std::string vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
		std::string psName);
	bool createBuffer(int index_mesh, ID3D11Device* device,
		vertex* vertices, int numV,
		unsigned int* indices, int numI);

	void render(
		ID3D11DeviceContext *context,
		const DirectX::XMFLOAT4X4&SynthesisMatrix,
		const DirectX::XMFLOAT4X4&worldMatrix,
		const DirectX::XMFLOAT4&camPos,
		line_light& _lineLight,
		std::vector<point_light>& _point_light,
		const DirectX::XMFLOAT4&materialColor,
		bool wireFlg,
		float elapsed_time
	);

	void release();

	void fetch_bone_influences(const FbxMesh *fbx_mesh, std::vector<bone_influences_per_control_point>&influences);
	void fetch_bone_matrices(FbxMesh *fbx_mesh, std::vector<skinned_mesh::bone> &skeletal, FbxTime time);
	void fbxamatrix_to_xmfloat4x4(const FbxAMatrix &fbxamatrix, DirectX::XMFLOAT4X4 &xmfloat4x4);
	void fetch_animations(FbxMesh *fbx_mesh, skinned_mesh::skeletal_animation &skeletal_animation, u_int sampling_rate = 0);

private:
	//template <class Archive>
	//void serialize(Archive& _ar)
	//{
	//	_ar(CEREAL_NVP(emit_dset));
	//}


};

#endif // !SKINNED_MESH_H_