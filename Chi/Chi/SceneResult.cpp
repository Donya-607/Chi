#include "scene.h"
#include "SceneManager.h"
#include "AssetLoader.h"

#include "Donya/Serializer.h"
#include "Donya/UseImGui.h"
#include "Donya/FilePath.h"
#include "Donya/Keyboard.h"


SceneResult::SceneResult() :
	rankingScore(), rankingRender(), yourScore(), yourScoreRender(),
	lights(), rankingPos(), yourScorePos(), rankingWidth()
{
	/*pLogo = std::make_shared<skinned_mesh>();
	pRanking = std::make_shared<skinned_mesh>();
	pYourScore = std::make_shared<skinned_mesh>();
	for (int i = 0; i < 10; i++)
	{
		pNummber[i] = std::make_shared<skinned_mesh>();
	}*/

	for (int i = 0; i < MAX_SIZE; i++)
	{
		rankingScore[i] = 0;
	}

	yourScore = GameTimer::GetInstance()->totalTimer.AllCurren();

	spriteLoad(&back, L"./Data/Images/UI/ResultFlame.png");
	spriteLoad(&text, L"./Data/Images/UI/ResultFont.png");
	spriteLoad(&result, L"./Data/Images/UI/ResultParts.png");
	spriteLoad(&ranking, L"./Data/Images/UI/Ranking.png");

	backData.pos = Donya::Vector2(0.0f, 0.0f);
	backData.texPos = Donya::Vector2(0.0f, 0.0f);
	backData.texSize = Donya::Vector2(1920.0f, 1080.0f);

	textData.pos = Donya::Vector2(1920.0f - 1110.0f, 0.0f);
	textData.texPos = Donya::Vector2(0.0f, 0.0f);
	textData.texSize = Donya::Vector2(1110.0f, 1080.0f);

	for (int i = 0; i < 3; i++)
	{
		bossBackData[i].pos = Donya::Vector2(1920.0f - 1054.0f, 0.0f);
		bossBackData[i].texPos = Donya::Vector2(0.0f, 88.0f * i);
		bossBackData[i].texSize = Donya::Vector2(1054.0f, 88.0f);
	}
	for (int i = 0; i < 10; i++)
	{
		boss1TimeData[i].pos = Donya::Vector2(0.0f, 0.0f);
		boss1TimeData[i].texPos = Donya::Vector2(0.0f, 0.0f);
		boss1TimeData[i].texSize = Donya::Vector2(1920.0f, 1080.0f);

		boss2TimeData[i].pos = Donya::Vector2(0.0f, 0.0f);
		boss2TimeData[i].texPos = Donya::Vector2(0.0f, 0.0f);
		boss2TimeData[i].texSize = Donya::Vector2(1920.0f, 1080.0f);

		boss3TimeData[i].pos = Donya::Vector2(0.0f, 0.0f);
		boss3TimeData[i].texPos = Donya::Vector2(0.0f, 0.0f);
		boss3TimeData[i].texSize = Donya::Vector2(1920.0f, 1080.0f);

		totalTimeData[i].pos = Donya::Vector2(0.0f, 0.0f);
		totalTimeData[i].texPos = Donya::Vector2(0.0f, 0.0f);
		totalTimeData[i].texSize = Donya::Vector2(1920.0f, 1080.0f);
	}
}

void SceneResult::init()  
{
	//	pLogo = AssetLoader::GetInstance()->logoModels.result;

	LoadParameter();

	int inNum = 0;
	bool aliveZero = false;
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (rankingScore[i] == 0)
		{
			rankingScore[i] = yourScore;
			inNum = i;
			aliveZero = true;
			break;
		}
	}

	if (aliveZero)
	{
		for (int i = inNum - 1; 0 <= i; i--)
		{
			if (yourScore <= rankingScore[i])
			{
				rankingScore[i+1] = rankingScore[i];
				rankingScore[i] = yourScore;
			}
		}
	}
	else
	{
		if (yourScore <= rankingScore[4])
		{
			rankingScore[4] = yourScore;
		}
		if (yourScore <= rankingScore[3])
		{
			rankingScore[4] = rankingScore[3];
			rankingScore[3] = yourScore;
		}
		if (yourScore <= rankingScore[2])
		{
			rankingScore[3] = rankingScore[2];
			rankingScore[2] = yourScore;
		}
		if (yourScore <= rankingScore[1])
		{
			rankingScore[2] = rankingScore[1];
			rankingScore[1] = yourScore;
		}
		if (yourScore <= rankingScore[0])
		{
			rankingScore[1] = rankingScore[0];
			rankingScore[0] = yourScore;
		}
	}

	for (int i = 0; i < MAX_SIZE; i++)
	{
		rankingRender[i].num.clear();
		int _score = rankingScore[i];
		int _num = 0;
		if (_score == 0) rankingRender[i].num.push_back(_score);
		while (0 < _score)
		{
			_num = _score % 10;
			rankingRender[i].num.push_back(_num);
			_score /= 10;
		}
	}

	yourScoreRender.num.clear();
	int _score = yourScore;
	int _num = 0;
	if (_score == 0) yourScoreRender.num.push_back(_score);
	while (0 < _score)
	{
		_num = _score % 10;
		yourScoreRender.num.push_back(_num);
		_score /= 10;
	}


	//for (int i = 0; i < MAX_SIZE; i++)
	//{
	//	rankingRender[i].pMesh.resize(rankingRender[i].num.size());
	//	for (int j = 0; j < static_cast<int>( rankingRender[i].num.size() ); j++)
	//	{
	//		createBillboard(&rankingRender[i].pMesh[j], L"./Data/Images/UI/UI.png");
	//	}
	//}

	rankingPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	yourScorePos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	rankingWidth = DirectX::XMFLOAT2(20.0f, 20.0f);

	SaveParameter();

	lights.direction.color = Donya::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	getLineLight().setLineLight(lights.direction.position,lights.direction.direction, lights.direction.color);
	for (const auto i : lights.points)
	{
		setPointLight(i);
	}
}

void SceneResult::update()
{
	GameLib::camera::setPos(DirectX::XMFLOAT3(0.0f, 0.0f, 100.0f));
	GameLib::camera::setTarget(Donya::Vector3(0.0f, 0.0f, 0.0f));

	if (Donya::Keyboard::Trigger('T') || GameLib::input::xInput::pressedButtons(0, XboxPad_Button::X) == 1)
	{
		pSceneManager->setNextScene(new sceneTitle, false);
	}
}

void SceneResult::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);

	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());

	setBlendMode_NONE(0.75f);
	//spriteRenderRect(&back, logo.pos, logo.texPos, logo.texSize);
	setBlendMode_ALPHA(1.0f);
}

void SceneResult::uninit()
{

}

void SceneResult::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}


#if USE_IMGUI

void SceneResult::imGui()
{
	if ( ImGui::BeginIfAllowed( "Result" ) )
	{
		ImGui::Text("ranking");
		ImGui::Text("1 : %d", rankingScore[0]);
		ImGui::Text("2 : %d", rankingScore[1]);
		ImGui::Text("3 : %d", rankingScore[2]);
		ImGui::Text("4 : %d", rankingScore[3]);
		ImGui::Text("5 : %d", rankingScore[4]);

		ImGui::Text(" ");

		if ( ImGui::Button( "GoTo Title" ) )
		{
			pSceneManager->setNextScene( new sceneTitle(), false );
		}
		ImGui::Text(" ");

		ImGui::Text("Your stage 1 Time : %d, %d : %d : %d", GameTimer::GetInstance()->timer[0].AllCurren(), GameTimer::GetInstance()->timer[0].Minute(), GameTimer::GetInstance()->timer[0].Second(), GameTimer::GetInstance()->timer[0].Current());
		ImGui::Text("Your stage 2 Time : %d, %d : %d : %d", GameTimer::GetInstance()->timer[1].AllCurren(), GameTimer::GetInstance()->timer[1].Minute(), GameTimer::GetInstance()->timer[1].Second(), GameTimer::GetInstance()->timer[1].Current());
		ImGui::Text("Your stage 3 Time : %d, %d : %d : %d", GameTimer::GetInstance()->timer[2].AllCurren(), GameTimer::GetInstance()->timer[2].Minute(), GameTimer::GetInstance()->timer[2].Second(), GameTimer::GetInstance()->timer[2].Current());
		ImGui::Text("Your Total Time : %d, %d : %d : %d", yourScore, GameTimer::GetInstance()->totalTimer.Minute(), GameTimer::GetInstance()->totalTimer.Second(), GameTimer::GetInstance()->totalTimer.Current());

		if (ImGui::TreeNode("BillBoard Test Draw Parametor"))
		{
			/*ImGui::DragFloat3("pos",		&data[0].pos.x);
			ImGui::DragFloat2("scale",		&data[0].scale.x);
			ImGui::DragFloat2("texPos",		&data[0].texPos.x);
			ImGui::DragFloat2("texSize",	&data[0].texSize.x);
			ImGui::Text(" ");
			ImGui::DragFloat3("pos ",		&data[1].pos.x);
			ImGui::DragFloat2("scale ",		&data[1].scale.x);
			ImGui::DragFloat2("texPos ",	&data[1].texPos.x);
			ImGui::DragFloat2("texSize ",	&data[1].texSize.x);*/

			ImGui::DragFloat3("rankingPos", &rankingPos.x);
			ImGui::DragFloat3("yourScorePos", &yourScorePos.x);
			ImGui::DragFloat2("rankingWidth", &rankingWidth.x);
			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI

void SceneResult::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}
