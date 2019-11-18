#include "scene.h"
#include "sceneManager.h"
#include "GameLibFunctions.h"
#include "XinputPad.h"
#include "gameLib.h"


void SceneGameOver::init()  
{
	spriteLoad(&sprite, L"./Data/Images/UI/GameOver.png");

	logo.pos = Donya::Vector2(631.0f, 175.0f);
	logo.texPos = Donya::Vector2(0.0f, 228.0f);
	logo.texSize = Donya::Vector2(658.0f, 218.0f);

	continuity.pos = Donya::Vector2(743.0f, 582.0f);
	continuity.texPos = Donya::Vector2(0.0f, 0.0f);
	continuity.texSize = Donya::Vector2(434.0f, 114.0f);

	toTitle.pos = Donya::Vector2(743.0f, 748.0f);
	toTitle.texPos = Donya::Vector2(434.0f, 113.0f);
	toTitle.texSize = Donya::Vector2(434.0f, 114.0f);

	state = 0;
}
void SceneGameOver::update()
{
	Fade::GetInstance()->Update();

	using namespace GameLib::input;
	constexpr int   PAD_NO = 0;
	constexpr float STICK_RANGE_MAX = 32768.0f;
	const auto leftStick = xInput::getThumbL(PAD_NO);
	if (0 < leftStick.y || xInput::pressedButtons(0, XboxPad_Button::DPAD_UP))
	{
		state = 0;
		continuity.texPos.x = 0.0f;
		toTitle.texPos.x = 434.0f;
	}
	if (leftStick.y < 0 || xInput::pressedButtons(0, XboxPad_Button::DPAD_DOWN))
	{
		state = 1;
		continuity.texPos.x = 434.0f;
		toTitle.texPos.x = 0.0f;
	}

	constexpr int TRIGGER_FLAG = 1;
	if (TRIGGER_FLAG == xInput::pressedButtons(PAD_NO, XboxPad_Button::A))
	{
		if (state == 0) Fade::GetInstance()->Init(1);
		else if (state == 1) Fade::GetInstance()->Init(0);
	}
}
void SceneGameOver::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);

	setBlendMode_NONE(0.75f);
	drawRect(Donya::Vector2(0.0f, 0.0f), Donya::Vector2(1920.0f, 1080.0f), Donya::Vector2(0.0f, 0.0f), 0.0f, Donya::Vector3(0.0f, 0.0f, 0.0f));
	spriteRenderRect(&sprite, logo.pos, logo.texPos, logo.texSize);
	spriteRenderRect(&sprite, continuity.pos, continuity.texPos, continuity.texSize);
	spriteRenderRect(&sprite, toTitle.pos, toTitle.texPos, toTitle.texSize);
	setBlendMode_ALPHA(1.0f);

	Fade::GetInstance()->Draw();
}
void SceneGameOver::uninit()
{

}
void SceneGameOver::imGui()
{
#ifdef DEBUG_MODE
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

	if (ImGui::TreeNode("logo Draw Parametor"))
	{
		ImGui::DragFloat2("pos", &logo.pos.x);
		ImGui::DragFloat2("tex pos", &logo.texPos.x);
		ImGui::DragFloat2("tex size", &logo.texSize.x);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("continuity Draw Parametor"))
	{
		ImGui::DragFloat2("pos", &continuity.pos.x);
		ImGui::DragFloat2("tex pos", &continuity.texPos.x);
		ImGui::DragFloat2("tex size", &continuity.texSize.x);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("To Title Draw Parametor"))
	{
		ImGui::DragFloat2("pos", &toTitle.pos.x);
		ImGui::DragFloat2("tex pos", &toTitle.texPos.x);
		ImGui::DragFloat2("tex size", &toTitle.texSize.x);
		ImGui::TreePop();
	}

	ImGui::End();
#endif
}