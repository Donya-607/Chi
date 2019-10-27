#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "imgui.h"
#include <string>
#include <cstdio>
#include <vector>

void sceneLightting::init()
{
	isStack = false;
	//spriteLoad(&texture, L"./Data/sprite/player-sprites.png");
	model.initialize({ 1,0,0,1 });
	ground.initialize({ 0,0,0,1 });
	GameLib::createShadowRT(&renderTarget);
	loadLightInfo();
	spriteLoad(&test, L"./Data/sprite/n64.png");
	light_index = 0;
	loadFBX(&test2, "./Data/test.fbx");
}

void sceneLightting::update()
{

	model.update();
	ground.update();
	if (GetAsyncKeyState('T') < 0)
		pSceneManager->setNextScene(new sceneTitle, false);
	if (GetAsyncKeyState('P') < 0)
		pSceneManager->setNextScene(new scenePose, true);
}

void sceneLightting::render()
{
	setBlendMode_ALPHA(125);
	//GameLib::setRenderTarget(&renderTarget);
	clearWindow({ 0.7f,0.7f,0.7f,1 });
	//LighttingLib::debug::display(1, 1, 1, 2.0, 2.0);
	ground.render();
	model.render();
	DirectX::XMMATRIX worldM;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	‰Šú‰»
	worldM = DirectX::XMMatrixIdentity();

	//	Šg‘åEk¬
	S = DirectX::XMMatrixScaling(0.01f, 0.01f, 0.01f);

	//	‰ñ“]
	Rx = DirectX::XMMatrixRotationX(0);				//	XŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
	Ry = DirectX::XMMatrixRotationY(0);				//	YŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
	Rz = DirectX::XMMatrixRotationZ(0);				//	ZŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
	R = Rz * Rx * Ry;

	//	•½sˆÚ“®
	T = DirectX::XMMatrixTranslation(0, 0, 0);

	//	ƒ[ƒ‹ƒh•ÏŠ·s—ñ
	worldM = S * R * T;

	//	Matrix -> Float4x4 •ÏŠ·
	DirectX::XMFLOAT4X4 wvp, world;
	DirectX::XMStoreFloat4x4(&wvp, worldM * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&world, worldM);

	FBXRender(&test2, wvp, world);
	//GameLib::resetRendertarget();
	//clearWindow({ 0.7f,0.7f,0.7f,1 });
	//ground.render();
	//model.render();
	ImGui::Render();
	//LighttingLib::debug::setString({ 0,0 }, L"SceneLightting : %d/%f", 1, 1.0f);

}

void sceneLightting::uninit()
{
	resetPointLight();
}

void sceneLightting::imGui()
{
	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
	ImGui::Begin("Scene", NULL, ImGuiWindowFlags_MenuBar);
	if (ImGui::Button("title"))
		pSceneManager->setNextScene(new sceneTitle, false);

	ImGui::End();


	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500, 0), ImGuiSetCond_Once);
	ImGui::Begin("Neutral", NULL, ImGuiWindowFlags_MenuBar);
	if (ImGui::Button("Save##light"))
	{
		saveLightInfo();
	}
	ImGui::DragFloat3("camPos", camPos);
	setCamPos(camPos[0], camPos[1], camPos[2]);
	ImGui::DragFloat3("LightAmbient", lightAmbient);
	ImGui::DragFloat3("LightColor", lightColor);
	setLightAmbient({ lightAmbient[0],lightAmbient[1],lightAmbient[2],1.0 }, {lightColor[0],lightColor[1],lightColor[2],1});
	ImGui::Text("pointLight");

	ImGui::InputInt("index", &light_index, 1, 1);
	enable = pointLight[light_index].pos.w;

	ImGui::Checkbox("enableable", &enable);
	ImGui::NewLine();
	ImGui::Text("pos");
	ImGui::DragFloat("x##point", &pointLight[light_index].pos.x, 0.1f);
	ImGui::DragFloat("y##point", &pointLight[light_index].pos.y, 0.1f);
	ImGui::DragFloat("z##point", &pointLight[light_index].pos.z, 0.1f);

	ImGui::NewLine();

	ImGui::Text("color");
	ImGui::DragFloat("x##color_point", &pointLight[light_index].color.x, 0.01f);
	ImGui::DragFloat("y##color_point", &pointLight[light_index].color.y, 0.01f);
	ImGui::DragFloat("z##color_point", &pointLight[light_index].color.z, 0.01f);

	ImGui::NewLine();

	ImGui::Text("diffuse");
	ImGui::DragFloat("x##diffuse_point", &pointLight[light_index].diffuse.x, 0.01f);
	ImGui::DragFloat("y##diffuse_point", &pointLight[light_index].diffuse.y, 0.01f);
	ImGui::DragFloat("z##diffuse_point", &pointLight[light_index].diffuse.z, 0.01f);

	ImGui::NewLine();

	ImGui::Text("specular");
	ImGui::DragFloat("x##specular_point", &pointLight[light_index].specular.x, 0.01f);
	ImGui::DragFloat("y##specular_point", &pointLight[light_index].specular.y, 0.01f);
	ImGui::DragFloat("z##specular_point", &pointLight[light_index].specular.z, 0.01f);

	ImGui::NewLine();

	ImGui::Text("attenuate");
	ImGui::DragFloat("x##attenuate_point", &pointLight[light_index].attenuate.x, 0.01f);
	ImGui::DragFloat("y##attenuate_point", &pointLight[light_index].attenuate.y, 0.01f);
	ImGui::DragFloat("z##attenuate_point", &pointLight[light_index].attenuate.z, 0.01f);

	pointLight[light_index].pos.w = enable;

	setPointlightInfo(light_index, pointLight[light_index]);



	ImGui::End();

	model.imGui();
	ground.imGui();
}

void sceneLightting::testOBJImGui()
{
	static int OBJNum = 0;
	static const char* kind_PS = { "phong\0rambart\0\0" };
	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::Begin("OBJ", NULL, ImGuiWindowFlags_MenuBar);

	
	ImGui::Text("Position");
	ImGui::DragFloat("x##position", &model.position.x, 0.1f);
	ImGui::DragFloat("y##position", &model.position.y, 0.1f);
	ImGui::DragFloat("z##position", &model.position.z, 0.1f);

	ImGui::Text("ambient");
	ImGui::DragFloat("x##ambient", &getPrimitiveMaterial(model.getAddress()).ambient.x, 0.1f);
	ImGui::DragFloat("y##ambient", &getPrimitiveMaterial(model.getAddress()).ambient.y, 0.1f);
	ImGui::DragFloat("z##ambient", &getPrimitiveMaterial(model.getAddress()).ambient.z, 0.1f);

	ImGui::Text("diffuse");
	ImGui::DragFloat("x##diffuse", &getPrimitiveMaterial(model.getAddress()).diffuse.x, 0.1f);
	ImGui::DragFloat("y##diffuse", &getPrimitiveMaterial(model.getAddress()).diffuse.y, 0.1f);
	ImGui::DragFloat("z##diffuse", &getPrimitiveMaterial(model.getAddress()).diffuse.z, 0.1f);

	ImGui::Text("specular");
	ImGui::DragFloat("r##specular", &model.color.x, 0.1f);
	ImGui::DragFloat("g##specular", &model.color.y, 0.1f);
	ImGui::DragFloat("b##specular", &model.color.z, 0.1f);
	ImGui::DragFloat("w##specular", &model.color.w, 0.1f);

	ImGui::NewLine();

	ImGui::Text("Angle");
	ImGui::DragFloat("x##angle", &model.angle.x);
	ImGui::DragFloat("y##angle", &model.angle.y);
	ImGui::DragFloat("z##angle", &model.angle.z);

	ImGui::NewLine();

	ImGui::Text("Scale");
	ImGui::DragFloat("x##scale", &model.scale.x);
	ImGui::DragFloat("y##scale", &model.scale.y);
	ImGui::DragFloat("z##scale", &model.scale.z);

	ImGui::End();

}

bool sceneLightting::saveLightInfo()
{
	FILE* f;

	fopen_s(&f, "./Data/LightingInfo/sceneLighting.txt", "w");

	for (int i = 0; i < 3; i++)
	{
		fprintf_s(f, "%f\n", lightAmbient[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		fprintf_s(f, "%f\n", lightColor[i]);
	}

	fprintf_s(f, "\n");

	for (int i = 0; i < 5; i++)
	{
		fprintf_s(f, "%f\n", pointLight[i].pos.x);
		fprintf_s(f, "%f\n", pointLight[i].pos.y);
		fprintf_s(f, "%f\n", pointLight[i].pos.z);

		fprintf_s(f, "%f\n", pointLight[i].color.x);
		fprintf_s(f, "%f\n", pointLight[i].color.y);
		fprintf_s(f, "%f\n", pointLight[i].color.z);
		fprintf_s(f, "%f\n", pointLight[i].color.w);


		fprintf_s(f, "%f\n", pointLight[i].diffuse.x);
		fprintf_s(f, "%f\n", pointLight[i].diffuse.y);
		fprintf_s(f, "%f\n", pointLight[i].diffuse.z);
		fprintf_s(f, "%f\n", pointLight[i].diffuse.w);

		fprintf_s(f, "%f\n", pointLight[i].specular.x);
		fprintf_s(f, "%f\n", pointLight[i].specular.y);
		fprintf_s(f, "%f\n", pointLight[i].specular.z);
		fprintf_s(f, "%f\n", pointLight[i].specular.w);

		fprintf_s(f, "%f\n", pointLight[i].attenuate.x);
		fprintf_s(f, "%f\n", pointLight[i].attenuate.y);
		fprintf_s(f, "%f\n", pointLight[i].attenuate.z);
		fprintf_s(f, "%f\n\n", pointLight[i].attenuate.w);





	}
	fclose(f);
	return true;
}

void sceneLightting::loadLightInfo()
{
	FILE* f;
	fopen_s(&f, "./Data/LightingInfo/sceneLighting.txt", "r");
	for (int i = 0; i < 3; i++)
	{
		fscanf_s(f, "%f", &lightAmbient[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		fscanf_s(f, "%f", &lightColor[i]);
		//lightColor[i] = 1;
	}

	setLightAmbient({ lightAmbient[0],lightAmbient[1],lightAmbient[2],1.0 }, {lightColor[0],lightColor[1],lightColor[2],1});
	for (auto& p : pointLight)
	{
		fscanf_s(f, "%f", &p.pos.x);
		fscanf_s(f, "%f", &p.pos.y);
		fscanf_s(f, "%f", &p.pos.z);
		p.pos.w = 1;
		fscanf_s(f, "%f", &p.color.x);
		fscanf_s(f, "%f", &p.color.y);
		fscanf_s(f, "%f", &p.color.z);
		fscanf_s(f, "%f", &p.color.w);

		fscanf_s(f, "%f", &p.diffuse.x);
		fscanf_s(f, "%f", &p.diffuse.y);
		fscanf_s(f, "%f", &p.diffuse.z);
		fscanf_s(f, "%f", &p.diffuse.w);

		fscanf_s(f, "%f", &p.specular.x);
		fscanf_s(f, "%f", &p.specular.y);
		fscanf_s(f, "%f", &p.specular.z);
		fscanf_s(f, "%f", &p.specular.w);

		fscanf_s(f, "%f", &p.attenuate.x);
		fscanf_s(f, "%f", &p.attenuate.y);
		fscanf_s(f, "%f", &p.attenuate.z);
		fscanf_s(f, "%f", &p.attenuate.w);

		setPointLight(p);
	}

	fclose(f);
}
