#include "scene.h"
#include "sceneManager.h"
#include "GameLibFunctions.h"


void SceneGameOver::init()  
{
	spriteLoad(&sprite, L"./Data/Images/UI/GameOver.png");

	logo.pos = Donya::Vector2(1920.0f / 2.0f, 1080.0f / 4.0f);
	logo.texPos = Donya::Vector2(0.0f, 228.0f);
	logo.texSize = Donya::Vector2(658.0f, 218.0f);
}
void SceneGameOver::update()
{

}
void SceneGameOver::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);

	spriteRenderRect(&sprite, logo.pos, logo.texPos, logo.texSize);
}
void SceneGameOver::uninit()
{

}
void SceneGameOver::imGui()
{
	ImGui::SetNextWindowSize(ImVec2(500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
	ImGui::Begin("Scene", NULL, ImGuiWindowFlags_MenuBar);

	if (ImGui::Button("Game"))
	{
		pSceneManager->setNextScene(new SceneGame(), false);
	}
	else if (ImGui::Button("Title"))
	{
		pSceneManager->setNextScene(new sceneTitle, false);
	}


	ImGui::End();

	ImGui::Begin("Sprite");

	ImGui::DragFloat2("pos", &logo.pos.x);
	ImGui::DragFloat2("tex pos", &logo.texPos.x);
	ImGui::DragFloat2("tex size", &logo.texSize.x);

	ImGui::End();
}