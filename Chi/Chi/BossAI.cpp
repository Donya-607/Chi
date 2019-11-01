#include "BossAI.h"

#include "Donya/FilePath.h"
#include "Donya/Serializer.h"

void BossAI::Init()
{
	state = ActionState::WAIT;

	for (int i = 0; i < ACTION_STATE_COUNT; i++)
	{
		cnt[i] = 0;
	}

	maxCnt[0] = 60;
	maxCnt[1] = 120;
	maxCnt[2] = 180;
	maxCnt[3] = 240;

	coolTimeMaxCnt[0] = 30;
	coolTimeMaxCnt[1] = 40;
	coolTimeMaxCnt[2] = 50;
	coolTimeMaxCnt[3] = 60;

	percent[0] = 25;
	percent[1] = 35;
	percent[2] = 40;
	percent[3] = 45;

	LoadParameter();
}

void BossAI::Update()
{
#if USE_IMGUI

	ImGui();

#endif // USE_IMGUI

#if 0
	if (cnt[state] <= maxCnt[state]) // 行動中
	{
		switch (state)
		{
		case ActionState::WAIT:
			// 待機

			break;
		case ActionState::MOVE:
			// 移動

			break;
		case ActionState::ATTACK:
			// 攻撃

			break;
		}
	}
	else // クールタイム中
	{

	}
#endif

	if (maxCnt[state] + coolTimeMaxCnt[state] < cnt[state]++) // クールタイム後
	{
		cnt[state] = 0;
		StateRand();
	}
}

void BossAI::StateRand()
{
#if 0
	state = rand() % ActionStateNum::END;
#else
	int percentRand = rand() % (percent[0] + percent[1] + percent[2]);
	     if (percentRand <= percent[0])
	{
		state = ActionState::WAIT;
	}
	else if (percentRand <= percent[0] + percent[1])
	{
		state = ActionState::MOVE;
	}
	else if (percentRand <= percent[0] + percent[1] + percent[2])
	{
		state = ActionState::ATTACK_SWING;
	}
	else if (percentRand <= percent[0] + percent[1] + percent[2] + percent[3])
	{
		state = ActionState::ATTACK_FAST;
	}
#endif
}

void BossAI::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void BossAI::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this ); 
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void BossAI::ImGui()
{
	if ( ImGui::BeginIfAllowed( "BossAI" ) )
	{
		if ( ImGui::TreeNode( "CurrentParameter" ) )
		{
			ImGui::Text("state : %d", state);

			ImGui::Text("wait_cnt : %d", cnt[0]);
			ImGui::Text("move_cnt : %d", cnt[1]);
			ImGui::Text("attack_swing_cnt : %d", cnt[2]);
			ImGui::Text("attack_fast_cnt : %d", cnt[3]);

			ImGui::Text("wait_max_cnt : %d", maxCnt[0]);
			ImGui::Text("move_max_cnt : %d", maxCnt[1]);
			ImGui::Text("attack_swing_max_cnt : %d", maxCnt[2]);
			ImGui::Text("attack_fast_max_cnt : %d", maxCnt[3]);

			ImGui::Text("wait_coolTime_max_cnt : %d", coolTimeMaxCnt[0]);
			ImGui::Text("move_coolTime_max_cnt : %d", coolTimeMaxCnt[1]);
			ImGui::Text("attack_swing_coolTime_max_cnt : %d", coolTimeMaxCnt[2]);
			ImGui::Text("attack_fast_coolTime_max_cnt : %d", coolTimeMaxCnt[3]);

			ImGui::TreePop();
		}

		if ( ImGui::TreeNode( "AdjustData" ) )
		{
			if ( ImGui::TreeNode( "Percents" ) )
			{
				const size_t COUNT = percent.size();
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::SliderInt( ( "[" + std::to_string( i ) + "]" ).c_str(), &percent[i], 0, 65535 );
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if ( ImGui::TreeNode( "File.I/O" ) )
		{
			static bool isBinary = true;
			if ( ImGui::RadioButton( "Binary", isBinary ) ) { isBinary = true; }
			if ( ImGui::RadioButton( "JSON", !isBinary ) ) { isBinary = false; }
			std::string loadStr{ "Load " };
			loadStr += ( isBinary ) ? "Binary" : "JSON";

			if ( ImGui::Button( "Save" ) )
			{
				SaveParameter();
			}
			// if ( ImGui::Button( Donya::MultiToUTF8( loadStr ).c_str() ) )
			if ( ImGui::Button( loadStr.c_str() ) )
			{
				LoadParameter( isBinary );
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI
