#include "scene.h"
#include "sceneManager.h"


void SceneEffect::init()  
{
	isStack = false;
	keyCnt = 0;

	flashParticle.Set();
	flashParticle.ImGuiDataInit();

	bubbleParticle.Set();
	bubbleParticle.ImGuiDataInit();

	bossAI.Init();

}
void SceneEffect::update()
{
	if (GetKeyState('T') < 0)
	{
		if (keyCnt == 0)
		{
			pSceneManager->setNextScene(new sceneTitle, false);
		}
		keyCnt++;
	}
	else
	{
		keyCnt = 0;
	}

	flashParticle.Emit();
	bubbleParticle.Emit();
	bossAI.Update();
}
void SceneEffect::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);
}
void SceneEffect::uninit()
{

}
void SceneEffect::imGui()
{
	ImGui::Begin("Particle");
	flashParticle.ImGui();
	bubbleParticle.ImGui();
	ImGui::End();

	flashParticle.UseImGui();

	ImGui::Begin("BossAI");
	bossAI.ImGui();
	ImGui::End();
}