#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "imgui.h"
#include <string>
#include <cstdio>
#include <vector>

void sceneShadow::init()
{
	isStack = false;
	GameLib::createShadowRT(&renderTarget);
	model.initialize({ 1,0,0,1 });
	ground.initialize({ 0,0,0,1 });

}

void sceneShadow::update()
{

	model.update();
	ground.update();

	if (GetAsyncKeyState('T') < 0)
		pSceneManager->setNextScene(new sceneTitle, false);
	if (GetAsyncKeyState('P') < 0)
		pSceneManager->setNextScene(new scenePose, true);
}

void sceneShadow::render()
{
	GameLib::setRenderTarget(&renderTarget);
	clearWindow({ 0.1f,0.5f,0.1f,1 });
	ground.render1({ lightPos[0],lightPos[1],lightPos[2] }, {0,0,0});
	model.render1({ lightPos[0],lightPos[1],lightPos[2] }, {0,0,0});
	//GameLib::createShadowSRV(shaderResource);
	//GameLib::resetRendertarget();
	//ground.render();
	//model.render();

	ImGui::Render();
}

void sceneShadow::uninit()
{
	GameLib::resetRendertarget();
}

void sceneShadow::imGui()
{

	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
	ImGui::Begin("Scene", NULL, ImGuiWindowFlags_MenuBar);
	if (ImGui::Button("title"))
		pSceneManager->setNextScene(new sceneTitle, false);

	if (ImGui::Button("lighting"))
		pSceneManager->setNextScene(new sceneLightting, false);

	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(500, getWindowSize().y / 2), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500, 0), ImGuiSetCond_Once);
	ImGui::Begin("Neutral", NULL, ImGuiWindowFlags_MenuBar);
	if (ImGui::Button("Save##shadow"))
	{
		saveLightInfo();
	}

	ImGui::DragFloat3("camPos", camPos);
	setCamPos(camPos[0], camPos[1], camPos[2]);
	ImGui::DragFloat3("LightPos", lightPos);
	ImGui::DragFloat3("LightDirection", lightDirection);


	ImGui::End();

	model.imGui();
	ground.imGui();
}

bool sceneShadow::saveLightInfo()
{
	FILE* f;

	fopen_s(&f, "./Data/LightingInfo/sceneShadow.txt", "w");

	for (int i = 0; i < 3; i++)
	{
		fprintf_s(f, "%f\n", lightPos[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		fprintf_s(f, "%f\n", lightDirection[i]);
	}

	fclose(f);
	return true;
}

void sceneShadow::loadLightInfo()
{
	FILE* f;
	fopen_s(&f, "./Data/LightingInfo/sceneShadow.txt", "r");
	for (int i = 0; i < 3; i++)
	{
		fscanf_s(f, "%f", &lightPos[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		fscanf_s(f, "%f", &lightDirection[i]);
	}

	fclose(f);
}
