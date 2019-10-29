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
}

void sceneTitle::update()
{

	if (GetAsyncKeyState('1') < 0)
		pSceneManager->setNextScene(new scenePose, true);
}

void sceneTitle::render()
{
	clearWindow(0.5f,0.5f,0.5f, 1.0f);
	setString({ 0,0 }, L"sceneTitle %d : %f", 1, 20.2f);
	//textOut(L"TITLE", .0f, 0.f);

	ImGui::Render();
}

void sceneTitle::uninit()
{

}

void sceneTitle::imGui()
{
	ImGui::SetNextWindowSize(ImVec2(500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
	ImGui::Begin("Scene", NULL, ImGuiWindowFlags_MenuBar);

	if ( ImGui::Button( "Game" ) )
	{
		pSceneManager->setNextScene( new SceneGame(), false );
	}
	else if ( Donya::Keyboard::Press( VK_LCONTROL ) && Donya::Keyboard::Trigger( 'G' ) )
	{
		pSceneManager->setNextScene( new SceneGame(), false );
	}
	else
	if (ImGui::Button( "Effect" ) )
	{
		pSceneManager->setNextScene( new SceneEffect, false );
	}

	ImGui::End();


	static int a, b, c, d = 0;
	ImGui::SetNextWindowSize(ImVec2(500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500.0f, .0f), ImGuiSetCond_Once);
	ImGui::Begin("debug", NULL, ImGuiWindowFlags_MenuBar);

	if (getKeyState(KEY_INPUT_ADD))
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
		ImGui::SetNextWindowPos(ImVec2(.0f, getWindowSize().y / 2.0f - 100.0f), ImGuiCond_Appearing);
		ImGui::Begin("ray", NULL, ImGuiWindowFlags_NoTitleBar);
		ImGui::End();
	}
}
