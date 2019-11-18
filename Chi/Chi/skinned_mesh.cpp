#include "skinned_mesh.h"

#include <iostream>

#include "Donya/Useful.h"

//#undef max
//#undef min

void skinned_mesh::fbxInit(ID3D11Device* _device, const std::string& _fbxFileName)
{

	// Create the FBX SDK manager 
	FbxManager* manager = FbxManager::Create();


	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h. 
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));


	// Create an importer. 
	FbxImporter* importer = FbxImporter::Create(manager, "");


	// Initialize the importer. 
	bool import_status = false;
	import_status = importer->Initialize(_fbxFileName.c_str(), -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());


	// Create a new scene so it can be populated by the imported file. 
	FbxScene* scene = FbxScene::Create(manager, "");


	// Import the contents of the file into the scene. 
	import_status = importer->Import(scene);
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh 
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);


	// Fetch node attributes and materials under this node recursively. Currently only mesh.  
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
		if (node) {
			FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute) {
				switch (fbx_node_attribute->GetAttributeType()) {
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
				traverse(node->GetChild(i));
		}
	};
	traverse(scene->GetRootNode());

	meshes.resize(fetched_meshes.size());
	int index_mesh = 0;
	for (size_t i = 0; i < fetched_meshes.size(); i++, index_mesh++)
	{
		if (i == 12)
			int a = 0;
		FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
		mesh& mesh = meshes.at(i);
		mesh.node_name = fetched_meshes.at(i)->GetNameOnly();
		mesh.index = index_mesh;
		const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();

		mesh.subsets.resize(number_of_materials);


		for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
		{
			have_material = true;
			subset& subset = mesh.subsets.at(index_of_material);
			const FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);

			std::function<void(property&, const char*, const char*)> fetch_material_property = [&](property& material, const char* property_name, const char* factor_name)
			{
				const FbxProperty property = surface_material->FindProperty(property_name);
				const FbxProperty factor = surface_material->FindProperty(factor_name);
				if (property.IsValid() && factor.IsValid())
				{
					FbxDouble3 colour = property.Get<FbxDouble3>();
					double f = factor.Get<FbxDouble>();
					material.color.x = static_cast<float>(colour[0]);
					material.color.y = static_cast<float>(colour[1]);
					material.color.z = static_cast<float>(colour[2]);
					material.color.w = 1.0f;
				}

				if (property.IsValid())
				{
					const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
					if (number_of_textures > 0)
					{
						const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
						if (file_texture)
						{
							//material.texture_filename = file_texture->GetFileName();
							std::string FAKE2 = _fbxFileName.substr(0, _fbxFileName.find_last_of("/") + 1);
							material.texture_filename = FAKE2 + file_texture->GetRelativeFileName();
							std::string FAKE = material.texture_filename;
							std::wstring NAME;
							NAME += std::wstring(FAKE.begin(), FAKE.end());

							// Create "diffuse.shader_resource_view" from "filename".
							if (!ResourceManager::LoadShaderResourceView(_device, NAME.c_str(), &material.shader_resource_view, &tex2dDesc))
							{
								assert(0 && "テクスチャ画像読み込み失敗");
								return;
							}
							have_uv = true;

						}
					}
				}
			};

			fetch_material_property(subset.diffuse, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
			fetch_material_property(subset.ambient, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
			fetch_material_property(subset.specular, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
			fetch_material_property(subset.transparent, FbxSurfaceMaterial::sTransparentColor, FbxSurfaceMaterial::sTransparencyFactor);

		}


		// Fetch mesh data 
		u_int vertex_count = 0;


		const FbxVector4* array_of_control_points = fbx_mesh->GetControlPoints();
		const int number_of_polygons = fbx_mesh->GetPolygonCount();
		mesh.indices.resize(number_of_polygons * 3);

		if (number_of_materials > 0)
		{
			const int number_of_polygons = fbx_mesh->GetPolygonCount();
			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
			{
				const u_int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
				mesh.subsets.at(material_index).index_count += 3;
			}

			int offset = 0;
			for (subset& subset : mesh.subsets)
			{
				subset.index_start = offset;
				offset += subset.index_count;
				subset.index_count = 0;
			}
		}



		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);
		FbxVector2 uv;
		bool unmapped_uv;

		std::vector<bone_influences_per_control_point> bone_influences;
		//ボーン情報の読み込み
		fetch_bone_influences(fbx_mesh, bone_influences);

		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
		{
			int index_of_material = 0;
			if (number_of_materials > 0)
			{
				index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
			}
			subset& subset = mesh.subsets.at(index_of_material);

			const int index_offset = subset.index_start + subset.index_count;

			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++)
			{
				vertex vertex;
				const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
				vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
				vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);


				FbxVector4 normal;
				fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
				vertex.normal.x = static_cast<float>(normal[0]);
				vertex.normal.y = static_cast<float>(normal[1]);
				vertex.normal.z = static_cast<float>(normal[2]);

				if (have_material)
				{
					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				bone_influences_per_control_point bone_point = bone_influences[index_of_control_point];
				float strong_weight = 0;
				int index = 0;
				int indexies[4];

				//上位4つを保存
				for (size_t index_of_influence = 0; index_of_influence < bone_point.size(); index_of_influence++)
				{

					if (index_of_influence < MAX_BONE_INFLUENCES)
					{
						vertex.bone_weights[index_of_influence] = bone_point.at(index_of_influence).weight;
						vertex.bone_indices[index_of_influence] = bone_point.at(index_of_influence).index;
						indexies[index_of_influence] = vertex.bone_indices[index_of_influence];
					}
					else
					{
						float weight = vertex.bone_weights[0];
						int _index = 0;
						for (int i = 1; i < 4; i++)
						{
							if (vertex.bone_weights[i] > weight)
								continue;
							_index = i;
						}
						if (vertex.bone_weights[_index] < bone_point.at(index_of_influence).weight)
						{
							vertex.bone_weights[_index] = bone_point.at(index_of_influence).weight;
							vertex.bone_indices[_index] = bone_point.at(index_of_influence).index;
							indexies[_index] = vertex.bone_indices[_index];
						}
					}

				}

				//4つの中から一番強いウェイトを見つける
				for (int i = 0; i < 4; i++)
				{
					if (strong_weight < vertex.bone_weights[i])
					{
						strong_weight = vertex.bone_weights[i];
						index = i;
					}
				}

				//余りを1番強いウェイトに加算
				for (size_t index_of_influence = 0; index_of_influence < bone_point.size(); index_of_influence++)
				{
					int _index = bone_point.at(index_of_influence).index;
					if (_index == indexies[0] || _index == indexies[1] || _index == indexies[2] || _index == indexies[3])
						continue;
					else
					{
						vertex.bone_weights[index] += bone_point.at(index_of_influence).weight;
					}
				}
				for (int i = 0; i < 4; i++)
				{
					mesh.bone_weights[i] = vertex.bone_weights[i];
					mesh.bone_indices[i] = vertex.bone_indices[i];
					mesh.pos = { vertex.position.x,vertex.position.y,vertex.position.z,1.0f };
				}
				mesh.vertices.push_back(vertex);
				mesh.indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count += 1;
			}
			subset.index_count += 3;
		}

		FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
		for (int row = 0; row < 4; row++)
		{
			for (int column = 0; column < 4; column++)
			{
				mesh.global_transform(row, column) = static_cast<float>(global_transform[row][column]);
			}
		}

		fetch_animations(fbx_mesh, mesh.anim);
	}



	manager->Destroy();




}



void skinned_mesh::setInfo(ID3D11Device* _device, const std::string& _fbxFileName, bool load_cerealize, bool is_Tpose)
{
	size_t count = _fbxFileName.find_last_of("/") + 1;
	size_t count2 = _fbxFileName.find_last_of(".") + 1;
	std::string filename = "./Data/model/serialized/" + _fbxFileName.substr(count, count2 - count);
	std::string bin_name = filename + "bin";
	std::string json_name = filename + "json";

	if (load_cerealize)
	{
		if (loadBinary(bin_name))
		{
			for (auto& p : meshes)
			{
				for (auto& it : p.subsets)
				{
					// std::wstring dummy(it.diffuse.texture_filename.begin(), it.diffuse.texture_filename.end());
					std::wstring dummy = Donya::MultiToWide( it.diffuse.texture_filename );
					if (dummy.empty())
						continue;	ResourceManager::LoadShaderResourceView(_device, dummy, &it.diffuse.shader_resource_view, &tex2dDesc);

				}
			}
			init(_device);

		}
		else if (loadBinary(json_name))
		{
			for (auto& p : meshes)
			{
				for (auto& it : p.subsets)
				{
					std::wstring dummy(it.diffuse.texture_filename.begin(), it.diffuse.texture_filename.end());
					ResourceManager::LoadShaderResourceView(_device, dummy, &it.diffuse.shader_resource_view, &tex2dDesc);

				}
			}
			init(_device);

		}

		else
		{

			have_uv = false;
			have_material = 0;
			have_born = false;

			if (is_Tpose)
				setInfo_T_pose(_device, _fbxFileName);
			else
				fbxInit(_device, _fbxFileName);

			init(_device);

			saveBinary(bin_name);
			saveBinary(json_name);

		}
	}
	else
	{

		have_uv = false;
		have_material = 0;
		have_born = false;

		if (is_Tpose)
			setInfo_T_pose(_device, _fbxFileName);
		else
			fbxInit(_device, _fbxFileName);

		init(_device);

		saveBinary(bin_name);
		saveBinary(json_name);

	}

	for (auto& p : meshes)
	{
		createBuffer(p.index, _device, p.vertices.data(), p.vertices.size(), p.indices.data(), p.indices.size());
	}
}

void skinned_mesh::setInfo_T_pose(ID3D11Device* _device, const std::string& _fbxFileName)
{

	// Create the FBX SDK manager 
	FbxManager* manager = FbxManager::Create();


	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h. 
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));


	// Create an importer. 
	FbxImporter* importer = FbxImporter::Create(manager, "");


	// Initialize the importer. 
	bool import_status = false;
	import_status = importer->Initialize(_fbxFileName.c_str(), -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());


	// Create a new scene so it can be populated by the imported file. 
	FbxScene* scene = FbxScene::Create(manager, "");


	// Import the contents of the file into the scene. 
	import_status = importer->Import(scene);
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh 
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);


	// Fetch node attributes and materials under this node recursively. Currently only mesh.  
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
		if (node) {
			FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute) {
				switch (fbx_node_attribute->GetAttributeType()) {
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
				traverse(node->GetChild(i));
		}
	};
	traverse(scene->GetRootNode());

	meshes.resize(fetched_meshes.size());
	int index_mesh = 0;
	for (size_t i = 0; i < fetched_meshes.size(); i++, index_mesh++)
	{
		if (i == 12)
			int a = 0;
		FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
		mesh& mesh = meshes.at(i);
		mesh.node_name = fetched_meshes.at(i)->GetNameOnly();

		const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();

		mesh.subsets.resize(number_of_materials);


		for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
		{
			have_material = true;
			subset& subset = mesh.subsets.at(index_of_material);
			const FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);

			std::function<void(property&, const char*, const char*)> fetch_material_property = [&](property& material, const char* property_name, const char* factor_name)
			{
				const FbxProperty property = surface_material->FindProperty(property_name);
				const FbxProperty factor = surface_material->FindProperty(factor_name);
				if (property.IsValid() && factor.IsValid())
				{
					FbxDouble3 colour = property.Get<FbxDouble3>();
					double f = factor.Get<FbxDouble>();
					material.color.x = static_cast<float>(colour[0]);
					material.color.y = static_cast<float>(colour[1]);
					material.color.z = static_cast<float>(colour[2]);
					material.color.w = 1.0f;
				}

				if (property.IsValid())
				{
					const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
					if (number_of_textures > 0)
					{
						const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
						if (file_texture)
						{
							//material.texture_filename = file_texture->GetFileName();
							material.texture_filename = file_texture->GetRelativeFileName();
							std::string FAKE = material.texture_filename;
							std::string FAKE2 = _fbxFileName.substr(0, _fbxFileName.find_last_of("/") + 1);
							std::wstring NAME;
							NAME += std::wstring(FAKE2.begin(), FAKE2.end()) + std::wstring(FAKE.begin(), FAKE.end());

							// Create "diffuse.shader_resource_view" from "filename".
							if (!ResourceManager::LoadShaderResourceView(_device, NAME.c_str(), &material.shader_resource_view, &tex2dDesc))
							{
								assert(0 && "テクスチャ画像読み込み失敗");
								return;
							}
							have_uv = true;

						}
					}
				}
			};

			fetch_material_property(subset.diffuse, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
			fetch_material_property(subset.ambient, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
			fetch_material_property(subset.specular, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);

		}


		// Fetch mesh data 
		std::vector<vertex> vertices; // Vertex buffer 
		std::vector<u_int> indices;  // Index buffer 
		u_int vertex_count = 0;


		const FbxVector4* array_of_control_points = fbx_mesh->GetControlPoints();
		const int number_of_polygons = fbx_mesh->GetPolygonCount();
		indices.resize(number_of_polygons * 3);

		if (number_of_materials > 0)
		{
			const int number_of_polygons = fbx_mesh->GetPolygonCount();
			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
			{
				const u_int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
				mesh.subsets.at(material_index).index_count += 3;
			}

			int offset = 0;
			for (subset& subset : mesh.subsets)
			{
				subset.index_start = offset;
				offset += subset.index_count;
				subset.index_count = 0;
			}
		}



		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);
		FbxVector2 uv;
		bool unmapped_uv;

		std::vector<bone_influences_per_control_point> bone_influences;
		//ボーン情報の読み込み
		fetch_bone_influences(fbx_mesh, bone_influences);

		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
		{
			int index_of_material = 0;
			if (number_of_materials > 0)
			{
				index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
			}
			subset& subset = mesh.subsets.at(index_of_material);

			const int index_offset = subset.index_start + subset.index_count;

			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++)
			{
				vertex vertex;
				const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
				vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
				vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);


				FbxVector4 normal;
				fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
				vertex.normal.x = static_cast<float>(normal[0]);
				vertex.normal.y = static_cast<float>(normal[1]);
				vertex.normal.z = static_cast<float>(normal[2]);

				if (have_material)
				{
					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				bone_influences_per_control_point bone_point = bone_influences[index_of_control_point];
				float strong_weight = 0;
				int index = 0;
				int indexies[4];

				//上位4つを保存
				for (size_t index_of_influence = 0; index_of_influence < bone_point.size(); index_of_influence++)
				{

					if (index_of_influence < MAX_BONE_INFLUENCES)
					{
						vertex.bone_weights[index_of_influence] = bone_point.at(index_of_influence).weight;
						vertex.bone_indices[index_of_influence] = bone_point.at(index_of_influence).index;
						indexies[index_of_influence] = vertex.bone_indices[index_of_influence];
					}
					else
					{
						float weight = vertex.bone_weights[0];
						int _index = 0;
						for (int i = 1; i < 4; i++)
						{
							if (vertex.bone_weights[i] > weight)
								continue;
							_index = i;
						}
						if (vertex.bone_weights[_index] < bone_point.at(index_of_influence).weight)
						{
							vertex.bone_weights[_index] = bone_point.at(index_of_influence).weight;
							vertex.bone_indices[_index] = bone_point.at(index_of_influence).index;
							indexies[_index] = vertex.bone_indices[_index];
						}
					}

				}

				//4つの中から一番強いウェイトを見つける
				for (int i = 0; i < 4; i++)
				{
					if (strong_weight < vertex.bone_weights[i])
					{
						strong_weight = vertex.bone_weights[i];
						index = i;
					}
				}

				//余りを1番強いウェイトに加算
				for (size_t index_of_influence = 0; index_of_influence < bone_point.size(); index_of_influence++)
				{
					int _index = bone_point.at(index_of_influence).index;
					if (_index == indexies[0] || _index == indexies[1] || _index == indexies[2] || _index == indexies[3])
						continue;
					else
					{
						vertex.bone_weights[index] += bone_point.at(index_of_influence).weight;
					}
				}
				for (int i = 0; i < 4; i++)
				{
					mesh.bone_weights[i] = vertex.bone_weights[i];
					mesh.bone_indices[i] = vertex.bone_indices[i];
					mesh.pos = { vertex.position.x,vertex.position.y,vertex.position.z,1.0f };
				}
				vertices.push_back(vertex);
				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count += 1;
			}
			subset.index_count += 3;
		}

		FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
		for (int row = 0; row < 4; row++)
		{
			for (int column = 0; column < 4; column++)
			{
				mesh.global_transform(row, column) = static_cast<float>(global_transform[row][column]);
			}
		}

		createBuffer(index_mesh, _device, vertices.data(), vertices.size(), indices.data(), indices.size());
	}



	manager->Destroy();




}
void skinned_mesh::init(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	//sampler_state
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC; //UNIT.06
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	DirectX::XMFLOAT4 dummy(0.0f, 0.0f, 0.0f, 0.0f);
	memcpy(sampler_desc.BorderColor, &dummy, sizeof(DirectX::XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, &sampleState);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//rasterizerLine
	D3D11_RASTERIZER_DESC line_desc;
	line_desc.FillMode = D3D11_FILL_WIREFRAME;	//塗りつぶし設定
	line_desc.CullMode = D3D11_CULL_BACK;		//カリング設定
	line_desc.FrontCounterClockwise = TRUE;	//頂点がどっち周りですか？
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
	fillOut_desc.FrontCounterClockwise = TRUE;	//頂点がどっち周りですか？
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


	hr = device->CreateDepthStencilState(&depthStencil_desc, &depthStencilState);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}


bool skinned_mesh::createBuffer(int index_mesh, ID3D11Device* device, vertex* vertices, int numV, unsigned int* indices, int numI)
{
	HRESULT hr;
	D3D11_BUFFER_DESC vertexBuffer_desc;
	vertexBuffer_desc.ByteWidth = numV * sizeof(vertex);
	vertexBuffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//バインド時の識別
	vertexBuffer_desc.CPUAccessFlags = 0;
	vertexBuffer_desc.MiscFlags = 0;
	vertexBuffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBuffer_data;
	vertexBuffer_data.pSysMem = vertices;
	vertexBuffer_data.SysMemPitch = 0;
	vertexBuffer_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vertexBuffer_desc, &vertexBuffer_data, &meshes[index_mesh].vertex_buffer);
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

		hr = device->CreateBuffer(&indexBuffer_desc, &indexBuffer_data, &meshes[index_mesh].index_buffer);
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

void skinned_mesh::saveBinary(std::string _file_name)
{
	std::string name = _file_name.substr(_file_name.find_last_of("/") + 1, _file_name.find_last_of(".") - 1);

	std::string extend = _file_name.substr(_file_name.find_last_of(".") + 1, _file_name.size() - _file_name.find_last_of(".") + 1);

	Donya::Serializer::Extension ext;
	std::string filePath;
	Donya::Serializer seria;
	if (extend == "bin")
	{
		ext = Donya::Serializer::Extension::BINARY;
		seria.Save(ext, _file_name.c_str(), name.c_str(), *this);
	}
	else
	{
		ext = Donya::Serializer::Extension::JSON;
		seria.Save(ext, _file_name.c_str(), name.c_str(), *this);
	}


}

bool skinned_mesh::loadBinary(std::string _file_name)
{
	std::string name = _file_name.substr(_file_name.find_last_of("/") + 1, _file_name.find_last_of(".") - 1);

	std::string extend = _file_name.substr(_file_name.find_last_of(".") + 1, _file_name.size() - _file_name.find_last_of(".") + 1);

	Donya::Serializer::Extension ext;
	std::string filePath;
	if (extend == "bin")
	{
		ext = Donya::Serializer::Extension::BINARY;
		filePath = "./Data/model/serialized/" + name;
	}
	else
	{
		ext = Donya::Serializer::Extension::JSON;
		filePath = "./Data/model/serialized/" + name;
	}


	Donya::Serializer seria;
	return seria.Load(ext, filePath.c_str(), name.c_str(), *this);

}

void skinned_mesh::render(ID3D11DeviceContext* context, fbx_shader& hlsl, const DirectX::XMFLOAT4X4& SynthesisMatrix, const DirectX::XMFLOAT4X4& worldMatrix, const DirectX::XMFLOAT4& camPos, line_light& _lineLight, std::vector<point_light>& _point_light, const DirectX::XMFLOAT4& materialColor, bool wireFlg, float elapsed_time, float magnification, bool animation_flg
)
{
	if (animation_flg)
		if (stop_time)
			stop_time -= elapsed_time;

	if (have_material)
	{
		for (auto& it : meshes)
		{
			if (animation_flg)
				if (!it.anim.empty())
				{
					animation_flame = static_cast<int>(it.anim.animation_tick / it.anim.sampling_time);

					//ループ用
					if (is_loop)
					{
						if (static_cast<size_t>(animation_flame) > it.anim.size() - 1)
						{
							animation_flame = 0;
							it.anim.animation_tick = 0;
							anim_fin = true;
						}
						//アニメーションタイマーのインクリメント
						if (!stop_animation && stop_time <= 0)
						{
							it.anim.animation_tick += elapsed_time * magnification;
							anim_fin = false;
						}
					}
					//ループ無し
					else
					{
						if (static_cast<size_t>(animation_flame) > it.anim.size() - 1)
						{

							animation_flame = it.anim.size() - 1;
							anim_fin = true;
						}
						else
						{
							if (!stop_animation && stop_time <= 0)
							{
								anim_fin = false;
								it.anim.animation_tick += elapsed_time * magnification;
							}
						}
					}
				}

			for (auto& p : it.subsets)
			{
				//	定数バッファの作成
				cbuffer cb;
				DirectX::XMStoreFloat4x4
				(&cb.world_view_projection,
					DirectX::XMLoadFloat4x4(&coordinate_conversion) * DirectX::XMLoadFloat4x4(&it.global_transform) *
					DirectX::XMLoadFloat4x4(&SynthesisMatrix)
				);

				DirectX::XMStoreFloat4x4
				(&cb.world,
					DirectX::XMLoadFloat4x4(&coordinate_conversion) * DirectX::XMLoadFloat4x4(&it.global_transform) *
					DirectX::XMLoadFloat4x4(&worldMatrix)
				);


				//アニメーション行列の取得
				if (!it.anim.empty())
				{
					//現在のフレームでの変換行列の算出
					std::vector<bone>& skeletal = it.anim.at(animation_flame);
					size_t number_of_bones = skeletal.size();
					_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
					for (size_t i = 0; i < number_of_bones; i++)
					{
						DirectX::XMStoreFloat4x4(&cb.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
					}
					//TODO skinme

				}


				cb.camPos = camPos;
				cb.lineLight = _lineLight.getInfo();
				int size = _point_light.size();
				for (int i = 0; i < 5; i++)
				{
					if (size <= i)
					{
						cb.pntLight[i].pos.w = 0;
						continue;
					}
					cb.pntLight[i] = _point_light[i].getInfo();
				}
				cb.ambient = p.ambient.color;
				cb.ambient.w = p.transparent.color.w;
				{
					cb.ambient.x *= materialColor.x;
					cb.ambient.y *= materialColor.y;
					cb.ambient.z *= materialColor.z;
					cb.ambient.w *= materialColor.w;
				}
				cb.diffuse = p.diffuse.color;
				cb.specular = p.specular.color;

				context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
				context->VSSetConstantBuffers(0, 1, &constant_buffer);
				context->PSSetConstantBuffers(0, 1, &constant_buffer);


				// 頂点バッファのバインド
				UINT stride = sizeof(vertex);
				UINT offset = 0;
				context->IASetVertexBuffers(0, 1, &it.vertex_buffer, &stride, &offset);

				//	インデックスバッファのバインド
				context->IASetIndexBuffer(it.index_buffer, DXGI_FORMAT_R32_UINT, 0);

				//	プリミティブモードの設定
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


				//	ラスタライザーの設定
				if (!wireFlg)	context->RSSetState(rasterizeFillOut);
				else		context->RSSetState(rasterizeLine);

				//	入力レイアウトのバインド
				context->IASetInputLayout(hlsl.layout);
				//	シェーダー(2種)の設定
				context->VSSetShader(hlsl.vertexShader, nullptr, 0);

				if (p.diffuse.shader_resource_view)
				{
					context->PSSetShader(hlsl.pixelShader, nullptr, 0);
				}
				else
				{
					context->PSSetShader(hlsl.noTexPS, nullptr, 0);
				}

				//	深度テストの設定
				context->OMSetDepthStencilState(depthStencilState, 0);

				if (p.diffuse.shader_resource_view)
				{
					context->PSSetShaderResources(0, 1, &p.diffuse.shader_resource_view);
					context->PSSetSamplers(0, 1, &sampleState);
				}


				//	プリミティブの描画
				context->DrawIndexed(p.index_count, p.index_start, 0);
			}
		}
	}
	else
	{
		for (auto& it : meshes)
		{
			//	定数バッファの作成
			cbuffer cb;
			DirectX::XMStoreFloat4x4
			(&cb.world_view_projection,
				DirectX::XMLoadFloat4x4(&it.global_transform) *
				DirectX::XMLoadFloat4x4(&SynthesisMatrix)
			);

			DirectX::XMStoreFloat4x4
			(&cb.world,
				DirectX::XMLoadFloat4x4(&it.global_transform) *
				DirectX::XMLoadFloat4x4(&worldMatrix)
			);
			cb.camPos = camPos;
			cb.lineLight = _lineLight.getInfo();
			int size = _point_light.size();
			for (int i = 0; i < 5; i++)
			{
				if (size <= i)
				{
					cb.pntLight[i].pos.w = 0;
					continue;
				}
				cb.pntLight[i] = _point_light[i].getInfo();
			}
			cb.ambient = materialColor;
			cb.diffuse = { 1,1,1,1 };
			cb.specular = { 0.5f,0.5f,0.5f,0.5f };

			context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer);

			// 頂点バッファのバインド
			UINT stride = sizeof(vertex);
			UINT offset = 0;
			context->IASetVertexBuffers(0, 1, &it.vertex_buffer, &stride, &offset);

			//	インデックスバッファのバインド
			context->IASetIndexBuffer(it.index_buffer, DXGI_FORMAT_R32_UINT, 0);

			//	プリミティブモードの設定
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//	入力レイアウトのバインド
			context->IASetInputLayout(hlsl.layout);

			//	ラスタライザーの設定
			if (!wireFlg)	context->RSSetState(rasterizeFillOut);
			else		context->RSSetState(rasterizeLine);

			//	シェーダー(2種)の設定
			context->VSSetShader(hlsl.noBoneVS, nullptr, 0);
			context->PSSetShader(hlsl.pixelShader, nullptr, 0);


			//	深度テストの設定
			context->OMSetDepthStencilState(depthStencilState, 0);



			//	プリミティブの描画
			context->DrawIndexed(numIndices, 0, 0);

		}
	}
}

void skinned_mesh::z_render(ID3D11DeviceContext* context, fbx_shader& hlsl, const DirectX::XMFLOAT4X4& SynthesisMatrix, const DirectX::XMFLOAT4X4& worldMatrix, const DirectX::XMFLOAT4& camPos, ID3D11PixelShader* z_PS)

{

	if (have_material)
	{
		for (auto& it : meshes)
		{

			for (auto& p : it.subsets)
			{
				//	定数バッファの作成
				cbuffer cb;
				DirectX::XMStoreFloat4x4
				(&cb.world_view_projection,
					DirectX::XMLoadFloat4x4(&coordinate_conversion) * DirectX::XMLoadFloat4x4(&it.global_transform) *
					DirectX::XMLoadFloat4x4(&SynthesisMatrix)
				);

				DirectX::XMStoreFloat4x4
				(&cb.world,
					DirectX::XMLoadFloat4x4(&coordinate_conversion) * DirectX::XMLoadFloat4x4(&it.global_transform) *
					DirectX::XMLoadFloat4x4(&worldMatrix)
				);


				//アニメーション行列の取得
				if (!it.anim.empty())
				{
					//現在のフレームでの変換行列の算出
					std::vector<bone>& skeletal = it.anim.at(animation_flame);
					size_t number_of_bones = skeletal.size();
					_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
					for (size_t i = 0; i < number_of_bones; i++)
					{
						DirectX::XMStoreFloat4x4(&cb.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
					}
					//TODO skinme

				}


				cb.camPos = camPos;
				cb.ambient = p.ambient.color;
				cb.ambient.w = p.transparent.color.w;
				cb.diffuse = p.diffuse.color;
				cb.specular = p.specular.color;
				context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
				context->VSSetConstantBuffers(0, 1, &constant_buffer);
				context->PSSetConstantBuffers(0, 1, &constant_buffer);


				// 頂点バッファのバインド
				UINT stride = sizeof(vertex);
				UINT offset = 0;
				context->IASetVertexBuffers(0, 1, &it.vertex_buffer, &stride, &offset);

				//	インデックスバッファのバインド
				context->IASetIndexBuffer(it.index_buffer, DXGI_FORMAT_R32_UINT, 0);

				//	プリミティブモードの設定
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


				//	ラスタライザーの設定
				context->RSSetState(rasterizeFillOut);

				//	入力レイアウトのバインド
				context->IASetInputLayout(hlsl.layout);
				//	シェーダー(2種)の設定
				context->VSSetShader(hlsl.vertexShader, nullptr, 0);

				context->PSSetShader(z_PS, nullptr, 0);

				//	深度テストの設定
				context->OMSetDepthStencilState(depthStencilState, 0);

				if (p.diffuse.shader_resource_view)
				{
					context->PSSetShaderResources(0, 1, &p.diffuse.shader_resource_view);
					context->PSSetSamplers(0, 1, &sampleState);
				}


				//	プリミティブの描画
				context->DrawIndexed(p.index_count, p.index_start, 0);
			}
		}
	}
	else
	{
		for (auto& it : meshes)
		{
			//	定数バッファの作成
			cbuffer cb;
			DirectX::XMStoreFloat4x4
			(&cb.world_view_projection,
				DirectX::XMLoadFloat4x4(&it.global_transform) *
				DirectX::XMLoadFloat4x4(&SynthesisMatrix)
			);

			DirectX::XMStoreFloat4x4
			(&cb.world,
				DirectX::XMLoadFloat4x4(&it.global_transform) *
				DirectX::XMLoadFloat4x4(&worldMatrix)
			);
			cb.camPos = camPos;
			cb.ambient = { 1,1,1,1 };
			cb.diffuse = { 1,1,1,1 };
			cb.specular = { 0.5f,0.5f,0.5f,0.5f };

			context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer);

			// 頂点バッファのバインド
			UINT stride = sizeof(vertex);
			UINT offset = 0;
			context->IASetVertexBuffers(0, 1, &it.vertex_buffer, &stride, &offset);

			//	インデックスバッファのバインド
			context->IASetIndexBuffer(it.index_buffer, DXGI_FORMAT_R32_UINT, 0);

			//	プリミティブモードの設定
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//	入力レイアウトのバインド
			context->IASetInputLayout(hlsl.layout);

			//	ラスタライザーの設定
			context->RSSetState(rasterizeFillOut);

			//	シェーダー(2種)の設定
			context->VSSetShader(hlsl.noBoneVS, nullptr, 0);
			context->PSSetShader(z_PS, nullptr, 0);


			//	深度テストの設定
			context->OMSetDepthStencilState(depthStencilState, 0);



			//	プリミティブの描画
			context->DrawIndexed(numIndices, 0, 0);

		}
	}
}

void skinned_mesh::bloom_SRVrender(ID3D11DeviceContext* context, fbx_shader& hlsl, const DirectX::XMFLOAT4X4& SynthesisMatrix, const DirectX::XMFLOAT4X4& worldMatrix, const DirectX::XMFLOAT4& camPos, line_light& _lineLight, std::vector<point_light>& _point_light, const DirectX::XMFLOAT4& materialColor, ID3D11PixelShader* bloom_PS, const DirectX::XMFLOAT4& _judge_color, ID3D11ShaderResourceView* z_SRV)

{

	if (have_material)
	{
		for (auto& it : meshes)
		{

			for (auto& p : it.subsets)
			{
				//	定数バッファの作成
				cbuffer cb;
				DirectX::XMStoreFloat4x4
				(&cb.world_view_projection,
					DirectX::XMLoadFloat4x4(&coordinate_conversion) * DirectX::XMLoadFloat4x4(&it.global_transform) *
					DirectX::XMLoadFloat4x4(&SynthesisMatrix)
				);

				DirectX::XMStoreFloat4x4
				(&cb.world,
					DirectX::XMLoadFloat4x4(&coordinate_conversion) * DirectX::XMLoadFloat4x4(&it.global_transform) *
					DirectX::XMLoadFloat4x4(&worldMatrix)
				);


				//アニメーション行列の取得
				if (!it.anim.empty())
				{
					//現在のフレームでの変換行列の算出
					std::vector<bone>& skeletal = it.anim.at(animation_flame);
					size_t number_of_bones = skeletal.size();
					_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
					for (size_t i = 0; i < number_of_bones; i++)
					{
						DirectX::XMStoreFloat4x4(&cb.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
					}
					//TODO skinme

				}


				cb.camPos = camPos;
				cb.lineLight = _lineLight.getInfo();
				int size = _point_light.size();
				for (int i = 0; i < 5; i++)
				{
					if (size <= i)
					{
						cb.pntLight[i].pos.w = 0;
						continue;
					}
					cb.pntLight[i] = _point_light[i].getInfo();
				}
				cb.ambient = p.ambient.color;
				cb.ambient.w = p.transparent.color.w;
				{
					cb.ambient.x *= materialColor.x;
					cb.ambient.y *= materialColor.y;
					cb.ambient.z *= materialColor.z;
					cb.ambient.w *= materialColor.w;
				}
				cb.diffuse = p.diffuse.color;
				cb.specular = p.specular.color;
				cb.judge_color = _judge_color;
				cb.screenSize = { (float)pSystem->SCREEN_WIDTH,(float)pSystem->SCREEN_HEIGHT,.0f,.0f };
				context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
				context->VSSetConstantBuffers(0, 1, &constant_buffer);
				context->PSSetConstantBuffers(0, 1, &constant_buffer);


				// 頂点バッファのバインド
				UINT stride = sizeof(vertex);
				UINT offset = 0;
				context->IASetVertexBuffers(0, 1, &it.vertex_buffer, &stride, &offset);

				//	インデックスバッファのバインド
				context->IASetIndexBuffer(it.index_buffer, DXGI_FORMAT_R32_UINT, 0);

				//	プリミティブモードの設定
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


				//	ラスタライザーの設定
				context->RSSetState(rasterizeFillOut);

				//	入力レイアウトのバインド
				context->IASetInputLayout(hlsl.layout);
				//	シェーダー(2種)の設定
				context->VSSetShader(hlsl.vertexShader, nullptr, 0);

				if (p.diffuse.shader_resource_view)
				{
					context->PSSetShader(bloom_PS, nullptr, 0);
				}
				else
				{
					context->PSSetShader(hlsl.noTexPS, nullptr, 0);
				}

				//	深度テストの設定
				context->OMSetDepthStencilState(depthStencilState, 0);

				if (p.diffuse.shader_resource_view)
				{
					context->PSSetShaderResources(0, 1, &p.diffuse.shader_resource_view);
					context->PSSetSamplers(0, 1, &sampleState);
				}
				context->PSSetShaderResources(1, 1, &z_SRV);
				context->PSSetSamplers(1, 1, &sampleState);


				//	プリミティブの描画
				context->DrawIndexed(p.index_count, p.index_start, 0);
			}
		}
	}
	else
	{
		for (auto& it : meshes)
		{
			//	定数バッファの作成
			cbuffer cb;
			DirectX::XMStoreFloat4x4
			(&cb.world_view_projection,
				DirectX::XMLoadFloat4x4(&it.global_transform) *
				DirectX::XMLoadFloat4x4(&SynthesisMatrix)
			);

			DirectX::XMStoreFloat4x4
			(&cb.world,
				DirectX::XMLoadFloat4x4(&it.global_transform) *
				DirectX::XMLoadFloat4x4(&worldMatrix)
			);
			cb.camPos = camPos;
			cb.lineLight = _lineLight.getInfo();
			int size = _point_light.size();
			for (int i = 0; i < 5; i++)
			{
				if (size <= i)
				{
					cb.pntLight[i].pos.w = 0;
					continue;
				}
				cb.pntLight[i] = _point_light[i].getInfo();
			}
			cb.ambient = materialColor;
			cb.diffuse = { 1,1,1,1 };
			cb.specular = { 0.5f,0.5f,0.5f,0.5f };
			cb.judge_color = { _judge_color.x,_judge_color.y,_judge_color.z,1.0f };
			cb.screenSize = { (float)pSystem->SCREEN_WIDTH,(float)pSystem->SCREEN_HEIGHT,.0f,.0f };
			context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer);

			// 頂点バッファのバインド
			UINT stride = sizeof(vertex);
			UINT offset = 0;
			context->IASetVertexBuffers(0, 1, &it.vertex_buffer, &stride, &offset);

			//	インデックスバッファのバインド
			context->IASetIndexBuffer(it.index_buffer, DXGI_FORMAT_R32_UINT, 0);

			//	プリミティブモードの設定
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//	入力レイアウトのバインド
			context->IASetInputLayout(hlsl.layout);

			//	ラスタライザーの設定
			context->RSSetState(rasterizeFillOut);

			//	シェーダー(2種)の設定
			context->VSSetShader(hlsl.noBoneVS, nullptr, 0);
			context->PSSetShader(bloom_PS, nullptr, 0);


			//	深度テストの設定
			context->OMSetDepthStencilState(depthStencilState, 0);

			context->PSSetShaderResources(1, 1, &z_SRV);
			context->PSSetSamplers(1, 1, &sampleState);

			//	プリミティブの描画
			context->DrawIndexed(numIndices, 0, 0);

		}
	}
}

void skinned_mesh::render(
	ID3D11DeviceContext* context,
	fbx_shader& hlsl,
	bone_animation* animation,
	const DirectX::XMFLOAT4X4& SynthesisMatrix,
	const DirectX::XMFLOAT4X4& worldMatrix,
	const DirectX::XMFLOAT4& camPos,
	line_light& _lightAmbient,
	std::vector<point_light>& _point_light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireFlg,
	float elapsed_time
)

{
	if (have_material)
	{
		for (auto& it : meshes)
		{
			for (auto& p : it.subsets)
			{
				//	定数バッファの作成
				cbuffer cb;
				DirectX::XMStoreFloat4x4
				(&cb.world_view_projection,
					DirectX::XMLoadFloat4x4(&it.global_transform) *
					DirectX::XMLoadFloat4x4(&SynthesisMatrix)
				);

				DirectX::XMStoreFloat4x4
				(&cb.world,
					DirectX::XMLoadFloat4x4(&it.global_transform) *
					DirectX::XMLoadFloat4x4(&worldMatrix)
				);


				//アニメーション行列の取得
				if (animation)
				{
					for (auto& anim : animation->getAnimContainer())
					{
						if (anim.mesh_name != it.node_name)
							continue;
						//現在のフレームでの変換行列の算出
						std::vector<bone>& skeletal = anim.animation.at(animation->getAnimationFlame());
						size_t number_of_bones = skeletal.size();
						_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
						for (size_t i = 0; i < number_of_bones; i++)
						{
							DirectX::XMStoreFloat4x4(&cb.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
						}
						//TODO skinme
					}
				}


				cb.camPos = camPos;
				cb.lineLight = _lightAmbient.getInfo();
				int size = _point_light.size();
				for (int i = 0; i < 5; i++)
				{
					if (size <= i)
					{
						cb.pntLight[i].pos.w = 0;
						continue;
					}
					cb.pntLight[i] = _point_light[i].getInfo();
				}
				cb.ambient = p.ambient.color;
				cb.ambient.x *= materialColor.x;
				cb.ambient.y *= materialColor.y;
				cb.ambient.z *= materialColor.z;
				cb.ambient.w *= materialColor.w;
				cb.diffuse = p.diffuse.color;
				cb.specular = p.specular.color;

				context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
				context->VSSetConstantBuffers(0, 1, &constant_buffer);
				context->PSSetConstantBuffers(0, 1, &constant_buffer);


				// 頂点バッファのバインド
				UINT stride = sizeof(vertex);
				UINT offset = 0;
				context->IASetVertexBuffers(0, 1, &it.vertex_buffer, &stride, &offset);

				//	インデックスバッファのバインド
				context->IASetIndexBuffer(it.index_buffer, DXGI_FORMAT_R32_UINT, 0);

				//	プリミティブモードの設定
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


				//	ラスタライザーの設定
				if (!wireFlg)	context->RSSetState(rasterizeFillOut);
				else		context->RSSetState(rasterizeLine);

				//	入力レイアウトのバインド
				context->IASetInputLayout(hlsl.layout);
				//	シェーダー(2種)の設定
				context->VSSetShader(hlsl.vertexShader, nullptr, 0);

				if (p.diffuse.shader_resource_view)
				{
					context->PSSetShader(hlsl.pixelShader, nullptr, 0);
				}
				else
				{
					context->PSSetShader(hlsl.noTexPS, nullptr, 0);
				}

				//	深度テストの設定
				context->OMSetDepthStencilState(depthStencilState, 0);

				if (p.diffuse.shader_resource_view)
				{
					context->PSSetShaderResources(0, 1, &p.diffuse.shader_resource_view);
					context->PSSetSamplers(0, 1, &sampleState);
				}


				//	プリミティブの描画
				context->DrawIndexed(p.index_count, p.index_start, 0);
			}
		}
	}
	else
	{
		for (auto& it : meshes)
		{
			//	定数バッファの作成
			cbuffer cb;
			DirectX::XMStoreFloat4x4
			(&cb.world_view_projection,
				DirectX::XMLoadFloat4x4(&it.global_transform) *
				DirectX::XMLoadFloat4x4(&SynthesisMatrix)
			);

			DirectX::XMStoreFloat4x4
			(&cb.world,
				DirectX::XMLoadFloat4x4(&it.global_transform) *
				DirectX::XMLoadFloat4x4(&worldMatrix)
			);
			cb.camPos = camPos;
			cb.lineLight = _lightAmbient.getInfo();
			int size = _point_light.size();
			for (int i = 0; i < 5; i++)
			{
				if (size <= i)
				{
					cb.pntLight[i].pos.w = 0;
					continue;
				}
				cb.pntLight[i] = _point_light[i].getInfo();
			}
			cb.ambient = materialColor;
			cb.diffuse = { 1,1,1,1 };
			cb.specular = { 0.5f,0.5f,0.5f,0.5f };

			context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer);

			// 頂点バッファのバインド
			UINT stride = sizeof(vertex);
			UINT offset = 0;
			context->IASetVertexBuffers(0, 1, &it.vertex_buffer, &stride, &offset);

			//	インデックスバッファのバインド
			context->IASetIndexBuffer(it.index_buffer, DXGI_FORMAT_R32_UINT, 0);

			//	プリミティブモードの設定
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//	入力レイアウトのバインド
			context->IASetInputLayout(hlsl.layout);

			//	ラスタライザーの設定
			if (!wireFlg)	context->RSSetState(rasterizeFillOut);
			else		context->RSSetState(rasterizeLine);

			//	シェーダー(2種)の設定
			context->VSSetShader(hlsl.noBoneVS, nullptr, 0);
			context->PSSetShader(hlsl.pixelShader, nullptr, 0);


			//	深度テストの設定
			context->OMSetDepthStencilState(depthStencilState, 0);



			//	プリミティブの描画
			context->DrawIndexed(numIndices, 0, 0);

		}
	}
}

void skinned_mesh::release()
{
	constant_buffer->Release();
	rasterizeLine->Release();
	rasterizeFillOut->Release();
	depthStencilState->Release();

}

void skinned_mesh::fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& influences)
{
	const int number_of_control_points = fbx_mesh->GetControlPointsCount();
	influences.resize(number_of_control_points);

	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));

		const int number_of_clusters = skin->GetClusterCount();
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
		{
			FbxCluster* cluster = skin->GetCluster(index_of_cluster);

			const int number_of_control_point_indices = cluster->GetControlPointIndicesCount();
			const int* array_of_control_point_indices = cluster->GetControlPointIndices();
			const double* array_of_control_point_weights = cluster->GetControlPointWeights();

			for (int i = 0; i < number_of_control_point_indices; ++i)
			{
				bone_influences_per_control_point& influences_per_control_point = influences.at(array_of_control_point_indices[i]);
				bone_influence influence;
				influence.index = index_of_cluster;
				influence.weight = static_cast<float>(array_of_control_point_weights[i]);
				influences_per_control_point.push_back(influence);
				have_born = true;
			}
		}
	}
}
void skinned_mesh::fetch_animations(FbxMesh* fbx_mesh, skeletal_animation& skeletal_animation, u_int sampling_rate)
{
	// Get the list of all the animation stack.   
	FbxArray<FbxString*> array_of_animation_stack_names;
	fbx_mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);

	// Get the number of animations.   
	int number_of_animations = array_of_animation_stack_names.Size();

	if (number_of_animations > 0)
	{
		// Get the FbxTime per animation's frame.
		FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
		FbxTime frame_time;    frame_time.SetTime(0, 0, 0, 1, 0, time_mode);

		sampling_rate = sampling_rate > 0 ? sampling_rate : (u_int)frame_time.GetFrameRate(time_mode);
		float sampling_time = 1.0f / sampling_rate;
		skeletal_animation.sampling_time = sampling_time;
		skeletal_animation.animation_tick = 0.0f;

		FbxString* animation_stack_name = array_of_animation_stack_names.GetAt(0);
		FbxAnimStack* current_animation_stack = fbx_mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
		fbx_mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);

		FbxTakeInfo* take_info = fbx_mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
		FbxTime start_time = take_info->mLocalTimeSpan.GetStart();
		FbxTime end_time = take_info->mLocalTimeSpan.GetStop();

		FbxTime sampling_step;    sampling_step.SetTime(0, 0, 1, 0, 0, time_mode);
		sampling_step = static_cast<FbxLongLong>(sampling_step.Get() * sampling_time);
		for (FbxTime current_time = start_time; current_time < end_time; current_time += sampling_step)
		{
			skeletal skeletal;
			fetch_bone_matrices(fbx_mesh, skeletal, current_time);
			skeletal_animation.push_back(skeletal);
		}
	}
	for (int i = 0; i < number_of_animations; i++)
	{
		delete array_of_animation_stack_names[i];
	}

}
void skinned_mesh::fetch_bone_matrices(FbxMesh* fbx_mesh, std::vector<bone>& skeletal, FbxTime time)
{
	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));

		const int number_of_clusters = skin->GetClusterCount();
		skeletal.resize(number_of_clusters);
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
		{
			bone& bone = skeletal.at(index_of_cluster);

			FbxCluster* cluster = skin->GetCluster(index_of_cluster);

			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);

			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);

			FbxAMatrix cluster_global_current_position;
			cluster_global_current_position = cluster->GetLink()->EvaluateGlobalTransform(time);

			FbxAMatrix reference_global_current_position;
			reference_global_current_position = fbx_mesh->GetNode()->EvaluateGlobalTransform(time);

			FbxAMatrix transform = reference_global_current_position.Inverse() * cluster_global_current_position
				* cluster_global_init_position.Inverse() * reference_global_init_position;

			fbxamatrix_to_xmfloat4x4(transform, bone.transform);
		}
	}
}
void skinned_mesh::fbxamatrix_to_xmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}
bool skinned_mesh::calcTransformedPosBySpecifyMesh(DirectX::XMFLOAT3& _local_pos, std::string _mesh_name, bone_animation* anim)
{
	mesh* _mesh = nullptr;
	for (mesh* p = meshes.data(); p < meshes.data() + meshes.size(); p++)
	{
		if (p->node_name != _mesh_name)
			continue;
		_mesh = p;
		break;
	}
	if (_mesh == nullptr)
		return false;
	for (auto& p : anim->getAnimContainer())
	{
		if (p.mesh_name != _mesh_name)
			continue;
		std::vector<bone>& skeletal = p.animation.at(anim->getAnimationFlame());

		size_t number_of_bones = skeletal.size();
		_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");

		DirectX::XMFLOAT4 pos = { _mesh->pos.x + _local_pos.x,_mesh->pos.y + _local_pos.y,_mesh->pos.z + _local_pos.z,_mesh->pos.w };
		DirectX::XMFLOAT3 _p = { 0,0,0 };

		for (size_t i = 0; i < 4; i++)
		{

			DirectX::XMFLOAT4X4 transform = skeletal.at(_mesh->bone_indices[i]).transform;
			DirectX::XMStoreFloat4x4(&transform, DirectX::XMLoadFloat4x4(&transform) * DirectX::XMLoadFloat4x4(&coordinate_conversion));

			float w = pos.x * transform._14 + pos.y * transform._24 + pos.z * transform._34 + pos.w * transform._44;

			_p.x += (pos.x * transform._11 + pos.y * transform._21 + pos.z * transform._31 + pos.w * transform._41) / w * _mesh->bone_weights[i];
			_p.y += (pos.x * transform._12 + pos.y * transform._22 + pos.z * transform._32 + pos.w * transform._42) / w * _mesh->bone_weights[i];
			_p.z += (pos.x * transform._13 + pos.y * transform._23 + pos.z * transform._33 + pos.w * transform._43) / w * _mesh->bone_weights[i];

		}

		_local_pos = { _p.x,_p.y,_p.z };
	}
	return true;
}

bool skinned_mesh::calcTransformedPosBySpecifyMesh(DirectX::XMFLOAT3& _local_pos, std::string _mesh_name)

{
	mesh* _mesh = nullptr;
	for (mesh* p = meshes.data(); p < meshes.data() + meshes.size(); p++)
	{
		if (p->node_name != _mesh_name)
			continue;
		_mesh = p;

		if (_mesh == nullptr)
			return false;

		std::vector<bone>& skeletal = p->anim.at(animation_flame);

		size_t number_of_bones = skeletal.size();
		_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");

		DirectX::XMFLOAT4 pos = { _mesh->pos.x + _local_pos.x,_mesh->pos.y + _local_pos.y,_mesh->pos.z + _local_pos.z,_mesh->pos.w };
		DirectX::XMFLOAT3 _p = { 0,0,0 };

		for (size_t i = 0; i < 4; i++)
		{

			DirectX::XMFLOAT4X4 transform = skeletal.at(_mesh->bone_indices[i]).transform;
			DirectX::XMStoreFloat4x4(&transform, DirectX::XMLoadFloat4x4(&transform) * DirectX::XMLoadFloat4x4(&coordinate_conversion));
			float w = pos.x * transform._14 + pos.y * transform._24 + pos.z * transform._34 + pos.w * transform._44;

			_p.x += (pos.x * transform._11 + pos.y * transform._21 + pos.z * transform._31 + pos.w * transform._41) / w * _mesh->bone_weights[i];
			_p.y += (pos.x * transform._12 + pos.y * transform._22 + pos.z * transform._32 + pos.w * transform._42) / w * _mesh->bone_weights[i];
			_p.z += (pos.x * transform._13 + pos.y * transform._23 + pos.z * transform._33 + pos.w * transform._43) / w * _mesh->bone_weights[i];

		}

		_local_pos = { _p.x,_p.y,_p.z };
		break;
	}
	return true;
}




void bone_animation::init(ID3D11Device* _device, std::string& file_name)
{
	if (loadBinary(file_name))
		return;
	else
	{
		loadAnimation(_device, file_name);
	}
}

bool bone_animation::loadBinary(std::string& file_name)
{
	size_t start = file_name.find_last_of("/") + 1;

	std::string binary_name = file_name.substr(start, file_name.find_last_of(".") + 1) + "bin";

	if (!find_file(binary_name))
		return false;
	return false;
}

bool bone_animation::find_file(std::string& file_name)
{
	SetCurrentDirectory(L"./Data/model/binary");
	WIN32_FIND_DATA data;
	HANDLE hnd;

	hnd = FindFirstFile(L"*.bin", &data);
	{
		std::wstring name = data.cFileName;
		size_t start = name.find_last_of(L"\\") + 1;
		std::wstring _name = name.substr(start, name.size() - start);
		// std::string dummy(_name.begin(), _name.end());
		std::string dummy = Donya::WideToMulti( _name );
		if (dummy == file_name)
		{
			FindClose(hnd);
			SetCurrentDirectory(L"..");
			SetCurrentDirectory(L"..");
			SetCurrentDirectory(L"..");
			return true;
		}}


	while (1)
	{
		if (FindNextFile(hnd, &data))
		{
			std::wstring name = data.cFileName;
			size_t start = name.find_last_of(L"\\") + 1;
			std::wstring _name = name.substr(start, name.size() - start);
			// std::string dummy(_name.begin(), _name.end());
			std::string dummy = Donya::WideToMulti( _name );
			if (dummy == file_name)
			{
				FindClose(hnd);
				SetCurrentDirectory(L"..");
				SetCurrentDirectory(L"..");
				SetCurrentDirectory(L"..");
				return true;
			}
		}
		else
		{
			FindClose(hnd);
			SetCurrentDirectory(L"..");
			SetCurrentDirectory(L"..");
			SetCurrentDirectory(L"..");
			break;
		}
	}
	return false;
}

void bone_animation::loadAnimation(ID3D11Device* _device, std::string& _filename)
{
	// Create the FBX SDK manager 
	FbxManager* manager = FbxManager::Create();


	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h. 
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));


	// Create an importer. 
	FbxImporter* importer = FbxImporter::Create(manager, "");


	// Initialize the importer. 
	bool import_status = false;
	import_status = importer->Initialize(_filename.c_str(), -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());


	// Create a new scene so it can be populated by the imported file. 
	FbxScene* scene = FbxScene::Create(manager, "");


	// Import the contents of the file into the scene. 
	import_status = importer->Import(scene);
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh 
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);


	// Fetch node attributes and materials under this node recursively. Currently only mesh.  
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
		if (node) {
			FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute) {
				switch (fbx_node_attribute->GetAttributeType()) {
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
				traverse(node->GetChild(i));
		}
	};
	traverse(scene->GetRootNode());

	anim_container.resize(fetched_meshes.size());
	int index_mesh = 0;
	for (size_t i = 0; i < fetched_meshes.size(); i++, index_mesh++)
	{
		if (i == 12)
			int a = 0;
		FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
		mesh_animation& anim_dummy = anim_container.at(i);
		anim_dummy.mesh_name = fetched_meshes.at(i)->GetNameOnly();
		fetch_animations(fbx_mesh, anim_dummy.animation);
	}
	manager->Destroy();

}

void bone_animation::fetch_animations(FbxMesh* fbx_mesh, skeletal_animation& skeletal_animation, u_int sampling_rate)
{
	// Get the list of all the animation stack.   
	FbxArray<FbxString*> array_of_animation_stack_names;
	fbx_mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);

	// Get the number of animations.   
	int number_of_animations = array_of_animation_stack_names.Size();

	if (number_of_animations > 0)
	{
		// Get the FbxTime per animation's frame.
		FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
		FbxTime frame_time;    frame_time.SetTime(0, 0, 0, 1, 0, time_mode);

		sampling_rate = sampling_rate > 0 ? sampling_rate : (u_int)frame_time.GetFrameRate(time_mode);
		float sampling_time = 1.0f / sampling_rate;
		skeletal_animation.sampling_time = sampling_time;
		skeletal_animation.animation_tick = 0.0f;

		FbxString* animation_stack_name = array_of_animation_stack_names.GetAt(0);
		FbxAnimStack* current_animation_stack = fbx_mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
		fbx_mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);

		FbxTakeInfo* take_info = fbx_mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
		FbxTime start_time = take_info->mLocalTimeSpan.GetStart();
		FbxTime end_time = take_info->mLocalTimeSpan.GetStop();

		FbxTime sampling_step;    sampling_step.SetTime(0, 0, 1, 0, 0, time_mode);
		sampling_step = static_cast<FbxLongLong>(sampling_step.Get() * sampling_time);
		for (FbxTime current_time = start_time; current_time < end_time; current_time += sampling_step)
		{
			skeletal skeletal;
			fetch_bone_matrices(fbx_mesh, skeletal, current_time);
			skeletal_animation.push_back(skeletal);
		}
	}
	for (int i = 0; i < number_of_animations; i++)
	{
		delete array_of_animation_stack_names[i];
	}

}

void bone_animation::fetch_bone_matrices(FbxMesh* fbx_mesh, std::vector<bone>& skeletal, FbxTime time)
{
	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));

		const int number_of_clusters = skin->GetClusterCount();
		skeletal.resize(number_of_clusters);
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
		{
			bone& bone = skeletal.at(index_of_cluster);

			FbxCluster* cluster = skin->GetCluster(index_of_cluster);

			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);

			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);

			FbxAMatrix cluster_global_current_position;
			cluster_global_current_position = cluster->GetLink()->EvaluateGlobalTransform(time);

			FbxAMatrix reference_global_current_position;
			reference_global_current_position = fbx_mesh->GetNode()->EvaluateGlobalTransform(time);

			FbxAMatrix transform = reference_global_current_position.Inverse() * cluster_global_current_position
				* cluster_global_init_position.Inverse() * reference_global_init_position;

			fbxamatrix_to_xmfloat4x4(transform, bone.transform);
		}
	}
}

void bone_animation::fbxamatrix_to_xmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}

void bone_animation::playAnimation(float _elapsed_time, float magnification, bool _is_loop)
{
	if (stop_animation)
		return;
	if (stop_time)
	{
		stop_time -= _elapsed_time;
		return;
	}

	for (auto& p : anim_container)
	{
		animation_flame = static_cast<int>(p.animation.animation_tick / p.animation.sampling_time);

		//ループ用
		if (_is_loop)
		{
			if (static_cast<size_t>(animation_flame) > p.animation.size() - 1)
			{
				animation_flame = 0;
				p.animation.animation_tick = 0;
			}
			//アニメーションタイマーのインクリメント
			if (!stop_animation && stop_time <= 0)
				p.animation.animation_tick += _elapsed_time * magnification;
		}
		//ループ無し
		else
		{
			if (static_cast<size_t>(animation_flame) > p.animation.size() - 1)
			{
				animation_flame = p.animation.size() - 1;
				p.animation.animation_tick = 0;
			}

			if (static_cast<size_t>(animation_flame) < p.animation.size() - 1)
				if (!stop_animation && stop_time <= 0)
					p.animation.animation_tick += _elapsed_time * magnification;
		}

	}

}
