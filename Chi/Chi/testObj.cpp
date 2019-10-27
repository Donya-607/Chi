#include "testObj.h"

void testOBJ::initialize(DirectX::XMFLOAT4 _color)
{
	loadData();
	wireFlg = false;
	loadOBJ(&model, L"./Data/model/cube.obj");
	color = _color;
	collision = false;
	world = DirectX::XMMatrixIdentity();
	useShaderNumber = 0;
	//	ägëÂÅEèkè¨
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	âÒì]
	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	ïΩçsà⁄ìÆ
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//	ÉèÅ[ÉãÉhïœä∑çsóÒ
	world = S * R * T;
	DirectX::XMStoreFloat4x4(&world_view_projection, world * getViewMatrix() * getProjectionMatrix());
}

void testOBJ::initialize(float r, float g, float b, float a)
{
	loadData();
	collision = false;

	world = DirectX::XMMatrixIdentity();

	//	ägëÂÅEèkè¨
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	âÒì]
	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	ïΩçsà⁄ìÆ
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//	ÉèÅ[ÉãÉhïœä∑çsóÒ
}

void testOBJ::update()
{


	angle = { angle.x *DirectX::XM_PI / 180.0f,angle.y*DirectX::XM_PI / 180.0f,angle.z*DirectX::XM_PI / 180.0f };
	//	ägëÂÅEèkè¨
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	âÒì]
	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	ïΩçsà⁄ìÆ
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	world = S * R * T;
	DirectX::XMStoreFloat4x4(&world_view_projection, world * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World, world);
	angle = { angle.x / DirectX::XM_PI * 180.0f,angle.y / DirectX::XM_PI * 180.0f,angle.z / DirectX::XM_PI * 180.0f };

}

void testOBJ::render()
{
	OBJRender(&model, world_view_projection, World,color,wireFlg);
}

void testOBJ::imGui()
{
	static const char* kind_PS = { "phong\0rambart\0\0" };
	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::Begin("OBJ", NULL, ImGuiWindowFlags_MenuBar);

	if (ImGui::Button("Save##testObj"))
	{
		saveData();
	}

	ImGui::Text("Position");
	ImGui::DragFloat("x##position_obj", &position.x, 0.1f);
	ImGui::DragFloat("y##position_obj", &position.y, 0.1f);
	ImGui::DragFloat("z##position_obj", &position.z, 0.1f);

	ImGui::Text("ambient");
	ImGui::DragFloat("x##ambient_obj", &getPrimitiveMaterial(&model).ambient.x, 0.1f);
	ImGui::DragFloat("y##ambient_obj", &getPrimitiveMaterial(&model).ambient.y, 0.1f);
	ImGui::DragFloat("z##ambient_obj", &getPrimitiveMaterial(&model).ambient.z, 0.1f);

	ImGui::Text("diffuse");
	ImGui::DragFloat("x##diffuse_obj", &getPrimitiveMaterial(&model).diffuse.x, 0.1f);
	ImGui::DragFloat("y##diffuse_obj", &getPrimitiveMaterial(&model).diffuse.y, 0.1f);
	ImGui::DragFloat("z##diffuse_obj", &getPrimitiveMaterial(&model).diffuse.z, 0.1f);

	ImGui::Text("specular");
	ImGui::DragFloat("r##specular_obj", &color.x, 0.1f);
	ImGui::DragFloat("g##specular_obj", &color.y, 0.1f);
	ImGui::DragFloat("b##specular_obj", &color.z, 0.1f);
	ImGui::DragFloat("w##specular_obj", &color.w, 0.1f);

	ImGui::NewLine();

	ImGui::Text("Angle");
	ImGui::DragFloat("x##angle_obj", &angle.x);
	ImGui::DragFloat("y##angle_obj", &angle.y);
	ImGui::DragFloat("z##angle_obj", &angle.z);

	ImGui::NewLine();

	ImGui::Text("Scale");
	ImGui::DragFloat("x##scale_obj", &scale.x);
	ImGui::DragFloat("y##scale_obj", &scale.y);
	ImGui::DragFloat("z##scale_obj", &scale.z);

	//if (ImGui::Combo("select PS", &useShaderNumber, kind_PS))
	//{
	//	model.setPsNumber(useShaderNumber);
	//}

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

void testOBJ::saveData()
{
	FILE* f;

	fopen_s(&f, "./Data/material_info/testOBJ.txt", "w");

	fprintf_s(f, "%f\n", position.x);
	fprintf_s(f, "%f\n", position.y);
	fprintf_s(f, "%f\n", position.z);


	fprintf_s(f, "%f\n", scale.x);
	fprintf_s(f, "%f\n", scale.y);
	fprintf_s(f, "%f\n", scale.z);


	//fprintf_s(f, "%f\n", getPrimitiveMaterial(&model).ambient.x);
	//fprintf_s(f, "%f\n", getPrimitiveMaterial(&model).ambient.y);
	//fprintf_s(f, "%f\n", getPrimitiveMaterial(&model).ambient.z);

	//fprintf_s(f, "%f\n", getPrimitiveMaterial(&model).diffuse.x);
	//fprintf_s(f, "%f\n", getPrimitiveMaterial(&model).diffuse.y);
	//fprintf_s(f, "%f\n", getPrimitiveMaterial(&model).diffuse.z);

	fprintf_s(f, "%f\n", color.x);
	fprintf_s(f, "%f\n", color.y);
	fprintf_s(f, "%f\n", color.z);
	fprintf_s(f, "%f\n", color.w);

	fclose(f);

}

void testOBJ::loadData()
{
	FILE* f;

	fopen_s(&f, "./Data/material_info/testOBJ.txt", "r");

	fscanf_s(f, "%f", &position.x);
	fscanf_s(f, "%f", &position.y);
	fscanf_s(f, "%f", &position.z);


	fscanf_s(f, "%f", &scale.x);
	fscanf_s(f, "%f", &scale.y);
	fscanf_s(f, "%f", &scale.z);


	//fscanf_s(f, "%f", &getPrimitiveMaterial(&model).ambient.x);
	//fscanf_s(f, "%f", &getPrimitiveMaterial(&model).ambient.y);
	//fscanf_s(f, "%f", &getPrimitiveMaterial(&model).ambient.z);

	//fscanf_s(f, "%f", &getPrimitiveMaterial(&model).diffuse.x);
	//fscanf_s(f, "%f", &getPrimitiveMaterial(&model).diffuse.y);
	//fscanf_s(f, "%f", &getPrimitiveMaterial(&model).diffuse.z);

	fscanf_s(f, "%f", &color.x);
	fscanf_s(f, "%f", &color.y);
	fscanf_s(f, "%f", &color.z);
	fscanf_s(f, "%f", &color.w);

	fclose(f);
}





void testOBJ2::initialize(DirectX::XMFLOAT4 _color)
{
	position = { .0f,1.0f,.0f };
	scale = { 1.0f,1.0f,1.0f };
	angle = { .0f,.0f,.0f };
	color = _color;
	wireFlg = false;
	createCube(&model);
	collision = false;
	world = DirectX::XMMatrixIdentity();
	//	ägëÂÅEèkè¨
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	âÒì]
	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	ïΩçsà⁄ìÆ
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//	ÉèÅ[ÉãÉhïœä∑çsóÒ
	world = S * R * T;
	DirectX::XMStoreFloat4x4(&world_view_projection, world * getViewMatrix() * getProjectionMatrix());
}

void testOBJ2::initialize(float r, float g, float b, float a)
{
	position = { .0f,1.0f,.0f };
	scale = { 1.0f,1.0f,1.0f };
	angle = { .0f,.0f,.0f };
	color = { r,g,b,a };
	createCube(&model);
	collision = false;

	world = DirectX::XMMatrixIdentity();

	//	ägëÂÅEèkè¨
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	âÒì]
	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	ïΩçsà⁄ìÆ
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//	ÉèÅ[ÉãÉhïœä∑çsóÒ

	initial_verocity = { .0f,.0f,.0f };

}

void testOBJ2::update()
{
	angle = { angle.x *DirectX::XM_PI / 180.0f,angle.y*DirectX::XM_PI / 180.0f,angle.z*DirectX::XM_PI / 180.0f };
	//	ägëÂÅEèkè¨
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	âÒì]
	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	ïΩçsà⁄ìÆ
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	world = S * R * T;
	DirectX::XMStoreFloat4x4(&world_view_projection, world * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World, world);
	angle = { angle.x / DirectX::XM_PI * 180.0f,angle.y / DirectX::XM_PI * 180.0f,angle.z / DirectX::XM_PI * 180.0f };

}

void testOBJ2::render1(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _direct)
{
	DirectX::XMStoreFloat4x4(&world_view_projection, world * GameLib::camera::GetLightViewMatrix(_pos, _direct) * GameLib::camera::GetLightProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World, world);

	GameLib::staticMesh::staticMeshShadowRender1(&model, world_view_projection, World);
}

void testOBJ2::render2(ID3D11ShaderResourceView* _resouce, DirectX::XMFLOAT4 _direct)
{
	DirectX::XMStoreFloat4x4(&world_view_projection, world * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World, world);
	GameLib::staticMesh::staticMeshShadowRender2(&model, world_view_projection, World, _direct, color, _resouce, wireFlg);
}

void testOBJ2::imGui()
{
	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::Begin("OBJ", NULL, ImGuiWindowFlags_MenuBar);
	ImGui::Text("Position");
	ImGui::DragFloat("x##position", &position.x, 0.1f);
	ImGui::DragFloat("y##position", &position.y, 0.1f);
	ImGui::DragFloat("z##position", &position.z, 0.1f);


	ImGui::Text("diffuse");
	ImGui::DragFloat("x##diffuse", &getPrimitiveMaterial(&model).diffuse.x, 0.1f);
	ImGui::DragFloat("y##diffuse", &getPrimitiveMaterial(&model).diffuse.y, 0.1f);
	ImGui::DragFloat("z##diffuse", &getPrimitiveMaterial(&model).diffuse.z, 0.1f);

	ImGui::NewLine();

	ImGui::Text("Angle");
	ImGui::DragFloat("x##angle", &angle.x);
	ImGui::DragFloat("y##angle", &angle.y);
	ImGui::DragFloat("z##angle", &angle.z);

	ImGui::NewLine();

	ImGui::Text("Scale");
	ImGui::DragFloat("x##scale", &scale.x);
	ImGui::DragFloat("y##scale", &scale.y);
	ImGui::DragFloat("z##scale", &scale.z);

	if (ImGui::Button("wirer"))
	{
		wireFlg ^= 1;
	}
	ImGui::End();

}
