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
#include "system.h"
#include "Donya/Serializer.h"
#include "Donya/Template.h"
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>

struct fbx_shader
{
	ID3D11VertexShader* vertexShader = nullptr;		//VertexShader
	ID3D11PixelShader* pixelShader = nullptr;		//PixelShader
	ID3D11PixelShader* noTexPS = nullptr;		//PixelShader
	ID3D11VertexShader* noBoneVS = nullptr;		//VertexShader
	ID3D11InputLayout* layout = nullptr;				//inputLayout
	ID3D11InputLayout* dummylayout = nullptr;				//inputLayout
	~fbx_shader()
	{
		ResourceManager::ReleaseVertexShader(vertexShader, layout);
		ResourceManager::ReleaseVertexShader(noBoneVS, dummylayout);
		ResourceManager::ReleasePixelShader(pixelShader);
		ResourceManager::ReleasePixelShader(noTexPS);
	}
};

struct bone
{
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				CEREAL_NVP(transform._11),
				CEREAL_NVP(transform._12),
				CEREAL_NVP(transform._13),
				CEREAL_NVP(transform._14),
				CEREAL_NVP(transform._21),
				CEREAL_NVP(transform._22),
				CEREAL_NVP(transform._23),
				CEREAL_NVP(transform._24),
				CEREAL_NVP(transform._31),
				CEREAL_NVP(transform._32),
				CEREAL_NVP(transform._33),
				CEREAL_NVP(transform._34),
				CEREAL_NVP(transform._41),
				CEREAL_NVP(transform._42),
				CEREAL_NVP(transform._43),
				CEREAL_NVP(transform._44)
			);
		}
	}

	DirectX::XMFLOAT4X4 transform;
};
typedef std::vector<bone> skeletal;
struct skeletal_animation : public std::vector<skeletal>
{
	float sampling_time = 1 / 24.0f;
	float animation_tick = 0.0f;
};
struct mesh_animation
{
	std::string mesh_name;
	skeletal_animation animation;
};


class bone_animation
{
private:
	std::vector<mesh_animation> anim_container;

	bool	stop_animation;
	float	stop_time;
	int	animation_flame;

public:
	//constructor destructor
	bone_animation() {}
	~bone_animation()
	{
	}
	//読み込み周り
	void init(ID3D11Device* _device, std::string& file_name);
	bool loadBinary(std::string& file_name);
	bool find_file(std::string& file_name);
	void loadAnimation(ID3D11Device* _device, std::string& _filename);
	void fetch_animations(FbxMesh* fbx_mesh, skeletal_animation& skeletal_animation, u_int sampling_rate = 0);
	void fetch_bone_matrices(FbxMesh* fbx_mesh, std::vector<bone>& skeletal, FbxTime time);
	void fbxamatrix_to_xmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4);

	//アニメーション
	void playAnimation(float _elapsed_time, float magnification, bool _is_loop);

	//getter
	int getAnimationFlame()
	{
		return animation_flame;
	}
	std::vector<mesh_animation> getAnimContainer()
	{
		return anim_container;
	}

	//setter
	void setAnimFlame(const float _animFlame)
	{
		for (auto& it : anim_container)
		{
			it.animation.animation_tick = _animFlame * it.animation.sampling_time;
		}
	}
	void setAnimStopTimer(float _stop_time)
	{
		stop_time = _stop_time;
	}
	void setAnimationStopFlg(bool _flg)
	{
		stop_animation = _flg;
	}
};


class skinned_mesh
{
public:
	static const int MAX_BONE_INFLUENCES = 4;
	static const int MAX_BONES = 64;

	struct vertex
	{
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t version)
		{
			if (version >= 0)
			{
				archive
				(
					CEREAL_NVP(position.x),
					CEREAL_NVP(position.y),
					CEREAL_NVP(position.z),
					CEREAL_NVP(normal.x),
					CEREAL_NVP(normal.y),
					CEREAL_NVP(normal.z),
					CEREAL_NVP(texcoord.x),
					CEREAL_NVP(texcoord.y),
					CEREAL_NVP(bone_weights),
					CEREAL_NVP(bone_indices)
				);
			}
		}
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
		DirectX::XMFLOAT4 judge_color;
		DirectX::XMFLOAT4 screenSize;
	};

	struct property
	{
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t version)
		{
			if (version >= 0)
			{
				archive
				(
					CEREAL_NVP(factor),
					CEREAL_NVP(color.x),
					CEREAL_NVP(color.y),
					CEREAL_NVP(color.z),
					CEREAL_NVP(color.w),
					CEREAL_NVP(texture_filename),
					CEREAL_NVP(scale_u),
					CEREAL_NVP(scale_v)
				);
			}
		}
		float factor = 1;
		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f }; // w channel is used as shininess by only specular.
		std::string texture_filename;
		float scale_u = 1.0;
		float scale_v = 1.0;

		DirectX::TexMetadata metadata = {};
		ID3D11ShaderResourceView* shader_resource_view;

	};

	struct subset
	{
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t version)
		{
			if (version >= 0)
			{
				archive
				(
					CEREAL_NVP(ambient),
					CEREAL_NVP(diffuse),
					CEREAL_NVP(specular),
					CEREAL_NVP(transparent),
					CEREAL_NVP(index_start),
					CEREAL_NVP(index_count)
				);
			}
		}
		property ambient;
		property diffuse;
		property specular;
		property transparent;
		u_int index_start = 0;//index bufferの開始位置
		u_int index_count = 0;//vertexの個数
	};

	struct mesh
	{
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t version)
		{
			if (version >= 0)
			{
				archive
				(
					CEREAL_NVP(vertices),
					CEREAL_NVP(indices),
					CEREAL_NVP(index),
					CEREAL_NVP(subsets),
					CEREAL_NVP(node_name),
					CEREAL_NVP(anim),
					CEREAL_NVP(global_transform._11),
					CEREAL_NVP(global_transform._12),
					CEREAL_NVP(global_transform._13),
					CEREAL_NVP(global_transform._14),
					CEREAL_NVP(global_transform._21),
					CEREAL_NVP(global_transform._22),
					CEREAL_NVP(global_transform._23),
					CEREAL_NVP(global_transform._24),
					CEREAL_NVP(global_transform._31),
					CEREAL_NVP(global_transform._32),
					CEREAL_NVP(global_transform._33),
					CEREAL_NVP(global_transform._34),
					CEREAL_NVP(global_transform._41),
					CEREAL_NVP(global_transform._42),
					CEREAL_NVP(global_transform._43),
					CEREAL_NVP(global_transform._44),
					CEREAL_NVP(bone_weights),
					CEREAL_NVP(bone_indices),

					CEREAL_NVP(pos.x),
					CEREAL_NVP(pos.y),
					CEREAL_NVP(pos.z),
					CEREAL_NVP(pos.w)

				);
			}
		}

		std::vector<vertex> vertices; // Vertex buffer 
		std::vector<u_int> indices;  // Index buffer 
		int index;
		ID3D11Buffer* vertex_buffer;
		ID3D11Buffer* index_buffer;
		std::vector<subset> subsets;
		std::string node_name;
		skeletal_animation anim;
		DirectX::XMFLOAT4X4 global_transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 1,0,0,0 };
		INT bone_indices[MAX_BONE_INFLUENCES] = {};
		DirectX::XMFLOAT4 pos;
	};

	struct bone_influence
	{
		int index;
		float weight;
	};
	DirectX::XMFLOAT4X4 coordinate_conversion = {
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

protected:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				CEREAL_NVP(meshes),
				CEREAL_NVP(have_uv),
				CEREAL_NVP(have_born),
				CEREAL_NVP(have_material),
				CEREAL_NVP(numIndices)

			);
		}
	}
	typedef std::vector<bone_influence> bone_influences_per_control_point;
	std::vector<mesh>meshes;
	D3D11_TEXTURE2D_DESC tex2dDesc;
	bool have_uv;
	bool have_born;
	bool have_material;
	ID3D11SamplerState* sampleState = nullptr;

	ID3D11Buffer* constant_buffer = nullptr;	//定数バッファ
	ID3D11RasterizerState* rasterizeLine = nullptr;		//線描画
	ID3D11RasterizerState* rasterizeFillOut = nullptr;	//塗りつぶし描画
	ID3D11DepthStencilState* depthStencilState = nullptr;	//depthStencilState

	bool	anim_fin;
	bool	is_loop;
	bool	stop_animation;
	float	stop_time;
	int	animation_flame;


	int							numIndices;

	void fbxInit(ID3D11Device* _device, const std::string& _objFileName);

public:
	int getMeshCount()
	{
		return meshes.size();
	}
	mesh& getMesh(int index)
	{
		return meshes[index];
	}

	skinned_mesh() : sampleState(nullptr), constant_buffer(nullptr), rasterizeFillOut(nullptr), rasterizeLine(nullptr), depthStencilState(nullptr), have_born(false), have_material(false), have_uv(false), numIndices(0), tex2dDesc(), is_loop(true), stop_animation(false), stop_time(0), animation_flame(0), anim_fin(false)
	{
		int a = 0;
	}
	~skinned_mesh() {}
	void setInfo(ID3D11Device* _device, const std::string& _objFileName, bool load_cerealize, bool is_Tpose);
	void setInfo_T_pose(ID3D11Device* _device, const std::string& _objFileName);

	void init(ID3D11Device* device);
	bool createBuffer(int index_mesh, ID3D11Device* device,
		vertex* vertices, int numV,
		unsigned int* indices, int numI);

	void saveBinary(std::string _fille_name);
	bool loadBinary(std::string _fille_name);

	void render(
		ID3D11DeviceContext* context,
		fbx_shader& hlsl,
		const DirectX::XMFLOAT4X4& SynthesisMatrix,
		const DirectX::XMFLOAT4X4& worldMatrix,
		const DirectX::XMFLOAT4& camPos,
		line_light& _lineLight,
		std::vector<point_light>& _point_light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireFlg,
		float elapsed_time,
		float magnification,
		bool animation_flg
	);

	void z_render(
		ID3D11DeviceContext* context,
		fbx_shader& hlsl,
		const DirectX::XMFLOAT4X4& SynthesisMatrix,
		const DirectX::XMFLOAT4X4& worldMatrix,
		const DirectX::XMFLOAT4& camPos,
		ID3D11PixelShader* z_PS
	);

	void bloom_SRVrender(
		ID3D11DeviceContext* context,
		fbx_shader& hlsl,
		const DirectX::XMFLOAT4X4& SynthesisMatrix,
		const DirectX::XMFLOAT4X4& worldMatrix,
		const DirectX::XMFLOAT4& camPos,
		line_light& _lineLight,
		std::vector<point_light>& _point_light,
		const DirectX::XMFLOAT4& materialColor,
		ID3D11PixelShader* bloom_SRV,
		const DirectX::XMFLOAT4& judge_color,
		ID3D11ShaderResourceView* z_SRV
	);


	void render(
		ID3D11DeviceContext* context,
		fbx_shader& hlsl,
		bone_animation* animation,
		const DirectX::XMFLOAT4X4& SynthesisMatrix,
		const DirectX::XMFLOAT4X4& worldMatrix,
		const DirectX::XMFLOAT4& camPos,
		line_light& _lineLight,
		std::vector<point_light>& _point_light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireFlg,
		float elapsed_time
	);


	void release();

	void fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& influences);
	void fetch_animations(FbxMesh* fbx_mesh, skeletal_animation& skeletal_animation, u_int sampling_rate = 0);
	void fetch_bone_matrices(FbxMesh* fbx_mesh, std::vector<bone>& skeletal, FbxTime time);
	void fbxamatrix_to_xmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4);

	bool calcTransformedPosBySpecifyMesh(DirectX::XMFLOAT3& _local_pos, std::string _mesh_name, bone_animation* anim);
	bool calcTransformedPosBySpecifyMesh(DirectX::XMFLOAT3& _local_pos, std::string _mesh_name);

	float getAnimationTime()
	{
		return meshes.at(0).anim.sampling_time * meshes.at(0).anim.size();
	}
	int getAnimFlame()
	{
		return animation_flame;
	}
	bool getAnimationStopFlg()
	{
		return stop_animation;
	}
	bool getAnimFinFlg()
	{
		return anim_fin;
	}

	void setAnimFlame(const int _animFlame)
	{
		for (auto& it : meshes)
		{
			it.anim.animation_tick = _animFlame * it.anim.sampling_time;
		}
	}
	void setAnimStopTimer(float _stop_time)
	{
		stop_time = _stop_time;
	}
	void setAnimationStopFlg(bool _flg)
	{
		stop_animation = _flg;
	}
	void setLoopFlg(const bool flg)
	{
		is_loop = flg;
	}


};
CEREAL_CLASS_VERSION(skinned_mesh, 0)


#endif // !SKINNED_MESH_H_