#include "floor.h"


void Floor::initialize(const float r, const float g, const float b, const float a)
{
	position = { .0f,.0f,.0f };
	loadData();
	angle = { .0f,.0f,.0f };
	wireFlg = false;
	collision = false;
	createPlane(&OBJ);

	world = DirectX::XMMatrixIdentity();


}

void Floor::initialize(const DirectX::XMFLOAT4 & _color)
{
	position = { .0f,.0f,.0f };
	loadData();
	angle = { .0f,.0f,.0f };
	wireFlg = false;
	collision = false;
	createPlane(&OBJ);

	world = DirectX::XMMatrixIdentity();


}

void Floor::update()
{
	//	ägëÂÅEèkè¨
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	âÒì]
	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	ïΩçsà⁄ìÆ
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//	ÉèÅ[ÉãÉhïœä∑çsóÒ
	world = S * R * T;

	DirectX::XMStoreFloat4x4(&world_view_projection, world * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World, world);

}

void Floor::render()
{
	OBJRender(&OBJ, world_view_projection, World,color, wireFlg);
}

void Floor::imGui()
{
	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 1000, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::Begin("FLOOR", NULL, ImGuiWindowFlags_MenuBar);
	
	if (ImGui::Button("Save##floor"))
	{
		saveData();
	}

	ImGui::Text("Scale");
	ImGui::DragFloat("x##scale", &scale.x);
	ImGui::DragFloat("z##scale", &scale.z);

	ImGui::Text("ambient");
	ImGui::DragFloat("x##ambient", &getPrimitiveMaterial(&OBJ).ambient.x, 0.1f);
	ImGui::DragFloat("y##ambient", &getPrimitiveMaterial(&OBJ).ambient.y, 0.1f);
	ImGui::DragFloat("z##ambient", &getPrimitiveMaterial(&OBJ).ambient.z, 0.1f);
	ImGui::DragFloat("w##ambient", &getPrimitiveMaterial(&OBJ).ambient.w, 0.1f);

	ImGui::Text("diffuse");
	ImGui::DragFloat("x##diffuse", &getPrimitiveMaterial(&OBJ).diffuse.x, 0.1f);
	ImGui::DragFloat("y##diffuse", &getPrimitiveMaterial(&OBJ).diffuse.y, 0.1f);
	ImGui::DragFloat("z##diffuse", &getPrimitiveMaterial(&OBJ).diffuse.z, 0.1f);
	ImGui::DragFloat("w##diffuse", &getPrimitiveMaterial(&OBJ).diffuse.w, 0.1f);

	ImGui::Text("specular");
	ImGui::DragFloat("r##specular", &color.x, 0.1f);
	ImGui::DragFloat("g##specular", &color.y, 0.1f);
	ImGui::DragFloat("b##specular", &color.z, 0.1f);
	ImGui::DragFloat("w##specular", &color.w, 0.1f);

	ImGui::NewLine();

	if (ImGui::Button("wirer"))
	{
		wireFlg ^= 1;
	}
	if (ImGui::Button("collision"))
	{
		collision ^= 1;
	}
	ImGui::End();
}

void Floor::saveData()
{
	FILE* f;

	fopen_s(&f, "./Data/material_info/floor.txt", "w");

	fprintf_s(f, "%f\n", scale.x);
	fprintf_s(f, "%f\n", scale.y);
	fprintf_s(f, "%f\n", scale.z);


	fprintf_s(f, "%f\n", getPrimitiveMaterial(&OBJ).ambient.x);
	fprintf_s(f, "%f\n", getPrimitiveMaterial(&OBJ).ambient.y);
	fprintf_s(f, "%f\n", getPrimitiveMaterial(&OBJ).ambient.z);

	fprintf_s(f, "%f\n", getPrimitiveMaterial(&OBJ).diffuse.x);
	fprintf_s(f, "%f\n", getPrimitiveMaterial(&OBJ).diffuse.y);
	fprintf_s(f, "%f\n", getPrimitiveMaterial(&OBJ).diffuse.z);

	fprintf_s(f, "%f\n", color.x);
	fprintf_s(f, "%f\n", color.y);
	fprintf_s(f, "%f\n", color.z);
	fprintf_s(f, "%f\n", color.w);

	fclose(f);

}

void Floor::loadData()
{
	FILE* f;

	fopen_s(&f, "./Data/material_info/floor.txt", "r");

	fscanf_s(f, "%f", &scale.x);
	fscanf_s(f, "%f", &scale.y);
	fscanf_s(f, "%f", &scale.z);


	fscanf_s(f, "%f", &getPrimitiveMaterial(&OBJ).ambient.x);
	fscanf_s(f, "%f", &getPrimitiveMaterial(&OBJ).ambient.y);
	fscanf_s(f, "%f", &getPrimitiveMaterial(&OBJ).ambient.z);

	fscanf_s(f, "%f", &getPrimitiveMaterial(&OBJ).diffuse.x);
	fscanf_s(f, "%f", &getPrimitiveMaterial(&OBJ).diffuse.y);
	fscanf_s(f, "%f", &getPrimitiveMaterial(&OBJ).diffuse.z);

	fscanf_s(f, "%f", &color.x);
	fscanf_s(f, "%f", &color.y);
	fscanf_s(f, "%f", &color.z);
	fscanf_s(f, "%f", &color.w);

	fclose(f);

}




void Floor2::initialize(const float r, const float g, const float b, const float a)
{
	position = { .0f,.0f,.0f };
	scale = { 5.0f,1.0f,5.0f };
	angle = { .0f,.0f,.0f };
	color = {r,g,b,a};
	wireFlg = false;
	collision = false;
	createPlane(&OBJ);

	world = DirectX::XMMatrixIdentity();


}

void Floor2::initialize(const DirectX::XMFLOAT4 & _color)
{
	position = { .0f,.0f,.0f };
	scale = { 5.0f,1.0f,5.0f };
	angle = { .0f,.0f,.0f };
	color = _color;
	wireFlg = false;
	collision = false;
	createPlane(&OBJ);

	world = DirectX::XMMatrixIdentity();


}

void Floor2::update()
{
	//	ägëÂÅEèkè¨
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	âÒì]
	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	ïΩçsà⁄ìÆ
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//	ÉèÅ[ÉãÉhïœä∑çsóÒ
	world = S * R * T;

}

void Floor2::render1(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _direct)
{
	DirectX::XMStoreFloat4x4(&world_view_projection, world * GameLib::camera::GetLightViewMatrix(_pos,_direct) * GameLib::camera::GetLightProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World, world);

	GameLib::staticMesh::staticMeshShadowRender1(&OBJ, world_view_projection, World);


}

void Floor2::render2(ID3D11ShaderResourceView * _resouce, DirectX::XMFLOAT4 _direct)
{
	DirectX::XMStoreFloat4x4(&world_view_projection, world * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World, world);
	GameLib::staticMesh::staticMeshShadowRender2(&OBJ, world_view_projection, World,_direct, color, _resouce, wireFlg);

}

void Floor2::imGui()
{
	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 1000, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::Begin("FLOOR", NULL, ImGuiWindowFlags_MenuBar);
	
	if (ImGui::Button("Save##floor"))
	{
		saveData();
	}

	ImGui::Text("Scale");
	ImGui::DragFloat("x##scale", &scale.x);
	ImGui::DragFloat("z##scale", &scale.z);

	ImGui::NewLine();

	if (ImGui::Button("wirer"))
	{
		wireFlg ^= 1;
	}
	ImGui::End();
}

void Floor2::saveData()
{
	FILE* f;

	fopen_s(&f, "./Data/material_info/floor2.txt", "w");

	fprintf_s(f, "%f\n", scale.x);
	fprintf_s(f, "%f\n", scale.y);
	fprintf_s(f, "%f\n", scale.z);



	fclose(f);

}

void Floor2::loadData()
{
	FILE* f;

	fopen_s(&f, "./Data/materialInfo/floor2.txt", "r");

	fscanf_s(f, "%f", &scale.x);
	fscanf_s(f, "%f", &scale.y);
	fscanf_s(f, "%f", &scale.z);


	fclose(f);

}
