#include "BossAI.h"

#include <numeric> // Use accumulate().

#include "Donya/FilePath.h"
#include "Donya/Serializer.h"
#include "Donya/Random.h"

void BossAI::Init()
{
	state = ActionState::WAIT;

	for ( auto &it : cnt )
	{
		it = 0;
	};
	for ( auto &it : percent )
	{
		// Must set Lowest percent.
		it = 1;
	};

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

	if ( maxCnt[state] + coolTimeMaxCnt[state] < cnt[state]++ ) // クールタイム後
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
	const int percentSum = std::accumulate( percent.begin(), percent.end(), 0 );
	
	// Prevent continuous chosen same state.
	int oldState = state;
	while ( oldState == state )
	{
		const int percentRand = Donya::Random::GenerateInt( percentSum );

		for ( int i = 0; i < ACTION_STATE_COUNT; ++i )
		{
			int partSum = ( i == ACTION_STATE_COUNT - 1 )
			? percentSum
			: std::accumulate( percent.begin(), ( percent.begin() + i + 1 ), 0 );

			if ( percentRand <= partSum )
			{
				state = static_cast<ActionState>( i );
				break;
			}
		}
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

			ImGui::Text("wait_cnt : %d",			cnt[0]);
			ImGui::Text("move_cnt : %d",			cnt[1]);
			ImGui::Text("attack_swing_cnt : %d",	cnt[2]);
			ImGui::Text("attack_fast_cnt : %d",		cnt[3]);
			ImGui::Text("attack_rotate_cnt : %d",	cnt[4]);

			ImGui::Text("wait_max_cnt : %d",			maxCnt[0]);
			ImGui::Text("move_max_cnt : %d",			maxCnt[1]);
			ImGui::Text("attack_swing_max_cnt : %d",	maxCnt[2]);
			ImGui::Text("attack_fast_max_cnt : %d",		maxCnt[3]);
			ImGui::Text("attack_rotate_max_cnt : %d",	maxCnt[4]);

			ImGui::Text("wait_coolTime_max_cnt : %d",			coolTimeMaxCnt[0]);
			ImGui::Text("move_coolTime_max_cnt : %d",			coolTimeMaxCnt[1]);
			ImGui::Text("attack_swing_coolTime_max_cnt : %d",	coolTimeMaxCnt[2]);
			ImGui::Text("attack_fast_coolTime_max_cnt : %d",	coolTimeMaxCnt[3]);
			ImGui::Text("attack_rotate_coolTime_max_cnt : %d",	coolTimeMaxCnt[4]);

			ImGui::TreePop();
		}

		if ( ImGui::TreeNode( "AdjustData" ) )
		{
			auto GetStateName = []( int i )->std::string
			{
				constexpr std::array<const char *, ACTION_STATE_COUNT> NAMES
				{
					"Wait",
					"Move",
					"Attack.Swing",
					"Attack.Fast",
					"Attack.Rotate",
				};

				if ( ACTION_STATE_COUNT <= i ) { return "Error Name"; }
				// else
				return std::string{ NAMES[i] };
			};

			if ( ImGui::TreeNode( "Percents" ) )
			{
				const size_t COUNT = percent.size();
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::SliderInt( GetStateName( i ).c_str(), &percent[i], 1, 1024 );
				}

				ImGui::TreePop();
			}
			
			if ( ImGui::TreeNode( "AttackingFrames" ) )
			{
				const size_t COUNT = maxCnt.size();
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::SliderInt( ( "[" + std::to_string( i ) + "]:" + GetStateName( i ) ).c_str(), &maxCnt[i], 0, 1024 );
				}

				ImGui::TreePop();
			}
			
			if ( ImGui::TreeNode( "CoolTimeCounts" ) )
			{
				const size_t COUNT = coolTimeMaxCnt.size();
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::SliderInt( ( "[" + std::to_string( i ) + "]:" + GetStateName( i ) ).c_str(), &coolTimeMaxCnt[i], 0, 1024 );
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
