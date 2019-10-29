#include "scene.h"


void SceneEffect::init()  
{
	isStack = false;

	flashParticle.Set();
	flashParticle.ImGuiDataInit();

	bossAI.Init();
}
void SceneEffect::update()
{
	flashParticle.Emit();
	bossAI.Update();
}
void SceneEffect::render()
{

}
void SceneEffect::uninit()
{

}
void SceneEffect::imGui()
{
	ImGui::Begin("Particle");
	flashParticle.ImGui();
	ImGui::End();

	ImGui::Begin("BossAI");
	bossAI.ImGui();
	ImGui::End();
}