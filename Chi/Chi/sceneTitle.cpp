#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "keyInput.h"

void sceneTitle::init()
{
	isStack = false;
	pl_down_ray.direction = { 0,-1,0 };
	pl_down_ray.invers_direction = { 0,1,0 };
	pl_down_ray.pos = { 0.1f,10.0f,0 };
	flg = false;
	pos = { 0,0,0 };
	setLightAmbient({ .1f,-1,0,1 } ,{1,1,1,1});
	FILE* f;
	float lightAmbient[3];
	PointLight pointLight[5];

	fopen_s(&f, "./Data/LightingInfo/sceneLighting.txt", "r");
	for (int i = 0; i < 3; i++)
	{
		fscanf_s(f, "%f", &lightAmbient[i]);
	}
	setLightAmbient({ lightAmbient[0],lightAmbient[1],lightAmbient[2],1.0 }, {1,1,1,1});
	for (auto& p : pointLight)
	{
		fscanf_s(f, "%f", &p.pos.x);
		fscanf_s(f, "%f", &p.pos.y);
		fscanf_s(f, "%f", &p.pos.z);
		p.pos.w = 1;
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
	loadFBX(&test, "./Data/FBX_Tree.fbx");
}

void sceneTitle::update()
{
	pKEY->update();
	DirectX::XMMATRIX worldM;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	初期化
	worldM = DirectX::XMMatrixIdentity();

	//	拡大・縮小
	S = DirectX::XMMatrixScaling(0.1f,0.1f,0.1f);

	//	回転
	Rx = DirectX::XMMatrixRotationX(0);				//	X軸を基準とした回転行列
	Ry = DirectX::XMMatrixRotationY(0);				//	Y軸を基準とした回転行列
	Rz = DirectX::XMMatrixRotationZ(0);				//	Z軸を基準とした回転行列
	R = Rz * Rx * Ry;

	//	平行移動
	T = DirectX::XMMatrixTranslation(0,0,0);

	//	ワールド変換行列
	worldM = S * R * T;

	//	Matrix -> Float4x4 変換
	DirectX::XMStoreFloat4x4(&world_view_projection[0], worldM * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World[0], worldM);

	pl_down_ray.pos = pos;

	worldM = DirectX::XMMatrixIdentity();


	//	平行移動

	//	ワールド変換行列
	worldM = S * R * T;

	//	Matrix -> Float4x4 変換
	DirectX::XMStoreFloat4x4(&world_view_projection[1], worldM * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World[1], worldM);

	if (GetAsyncKeyState('1') < 0)
		pSceneManager->setNextScene(new scenePose, true);
}

void sceneTitle::render()
{
	clearWindow(1.0f, 0.5f, 0.5f, 1.0f);
	setString({ 0,0 }, L"sceneTitle %d : %f", 1, 20.2f);
	//textOut(L"TITLE", .0f, 0.f);
	XMFLOAT4X4 matrix;
	FBXRender(&test, world_view_projection[0], World[0]);

	ImGui::Render();
}

void sceneTitle::uninit()
{

}

void sceneTitle::imGui()
{
	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
	ImGui::Begin("Scene", NULL, ImGuiWindowFlags_MenuBar);

	if (ImGui::Button("lighting"))
		pSceneManager->setNextScene(new sceneLightting, false);

	if ( ImGui::Button( "Game" ) )
	{
		pSceneManager->setNextScene( new SceneGame(), false );
	}

	ImGui::End();


	static int a, b, c, d = 0;
	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500, 0), ImGuiSetCond_Once);
	ImGui::Begin("debug", NULL, ImGuiWindowFlags_MenuBar);

	if (pKEY->getState(KEY_INPUT_ADD))
	{
		a++;
	}
	if (pressedButtons(0, XboxPad_Button::A))
	{
		a++;
	}
	a = getThumbL(0).x;
	b = getThumbL(0).y;
	c = getThumbR(0).x;
	d = getThumbR(0).y;

	ImGui::Text("L\nx:%d\ny:%d\nR\nx:%d\ny:%d", a, b, c, d);
	ImGui::NewLine();

	ImGui::Text("rayPos");
	ImGui::DragFloat("x", &pos.x,0.1f);
	ImGui::DragFloat("y", &pos.y,0.1f);
	ImGui::DragFloat("z", &pos.z,0.1f);
	ImGui::End();

	if (flg)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(0, getWindowSize().y / 2 - 100), ImGuiCond_Appearing);
		ImGui::Begin("ray", NULL, ImGuiWindowFlags_NoTitleBar);
		ImGui::End();
	}
}
