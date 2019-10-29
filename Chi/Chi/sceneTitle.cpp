#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "keyInput.h"

#include "Donya/Keyboard.h" // Insert by Donya.

void sceneTitle::init()
{
	isStack = false;
	pl_down_ray.direction = { 0,-1,0 };
	pl_down_ray.invers_direction = { 0,1,0 };
	pl_down_ray.pos = { 0.1f,10.0f,0 };
	flg = false;
	pos = { 0,0,0 };
	setLightAmbient({ .1f,-1,0,1 } ,{1,1,1,1});
	
	loadFBX(&test, "./Data/FBX_Tree.fbx");
}

void sceneTitle::update()
{
	pKEY->update();
	DirectX::XMMATRIX worldM;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	‰Šú‰»
	worldM = DirectX::XMMatrixIdentity();

	//	Šg‘åEk¬
	S = DirectX::XMMatrixScaling(0.1f,0.1f,0.1f);

	//	‰ñ“]
	Rx = DirectX::XMMatrixRotationX(0);				//	XŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
	Ry = DirectX::XMMatrixRotationY(0);				//	YŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
	Rz = DirectX::XMMatrixRotationZ(0);				//	ZŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
	R = Rz * Rx * Ry;

	//	•½sˆÚ“®
	T = DirectX::XMMatrixTranslation(0,0,0);

	//	ƒ[ƒ‹ƒh•ÏŠ·s—ñ
	worldM = S * R * T;

	//	Matrix -> Float4x4 •ÏŠ·
	DirectX::XMStoreFloat4x4(&world_view_projection[0], worldM * getViewMatrix() * getProjectionMatrix());
	DirectX::XMStoreFloat4x4(&World[0], worldM);

	pl_down_ray.pos = pos;

	worldM = DirectX::XMMatrixIdentity();


	//	•½sˆÚ“®

	//	ƒ[ƒ‹ƒh•ÏŠ·s—ñ
	worldM = S * R * T;

	//	Matrix -> Float4x4 •ÏŠ·
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
	else if ( Donya::Keyboard::Press( VK_LCONTROL ) && Donya::Keyboard::Trigger( 'G' ) )
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
