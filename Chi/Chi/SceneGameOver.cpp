#include "scene.h"
#include "sceneManager.h"


void SceneGameOver::init()  
{

}
void SceneGameOver::update()
{

}
void SceneGameOver::render()
{

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
}