#include "AIBase.h"

#include "Donya/FilePath.h"

#if USE_IMGUI

void ActionStorage::ShowImGuiNode( const std::string &prefix )
{
	const std::string nodeCaption = prefix; ".Actions";
	if ( ImGui::TreeNode( nodeCaption.c_str() ) )
	{
		ImGui::DragInt( "Wait", &waitNo );
		ImGui::DragInt( "NextAttack", &nextAttackNo );

		ImGui::TreePop();
	}
}

#endif // USE_IMGUI

DistanceLottery::DistanceLottery() : LotteryBase(), data()
{}
DistanceLottery::~DistanceLottery() = default;

ActionStorage DistanceLottery::Lottery( float distance )
{
	if ( data.empty() ) { return ActionStorage::Zero(); }
	// else

#if DEBUG_MODE
	return data[0].action;
#endif // DEBUG_MODE
}

void DistanceLottery::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void DistanceLottery::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void DistanceLottery::ShowImGuiNode( const std::string &prefix )
{
	const std::string nodeCaption = prefix + ".Lottery.Distance";
	if ( ImGui::TreeNode( nodeCaption.c_str() ) )
	{
		if ( ImGui::Button( "Append" ) )
		{
			data.push_back( {} );
		}
		if ( 1 < data.size() )
		{
			if ( ImGui::Button( "PopBack" ) )
			{
				data.pop_back();
			}
		}
		else
		{
			ImGui::Text( "" );
		}

		auto ShowContent = []( size_t index, Content &refContent )
		{
			std::string indexStr = "[" + std::to_string( index ) + "]";
			ImGui::SliderFloat( ( indexStr + ".Distance" ).c_str(), &refContent.distance, 0.0f, 1.0f );
			refContent.action.ShowImGuiNode( indexStr );
		};

		const size_t COUNT = data.size();
		for ( size_t i = 0; i < COUNT; ++i )
		{
			ShowContent( i, data[i] );
			ImGui::Text( "" );
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

		ImGui::TreePop();
	}
}

#endif // USE_IMGUI

FixedLottery::FixedLottery() : LotteryBase(), action()
{}
FixedLottery::~FixedLottery() = default;

ActionStorage FixedLottery::Lottery( float distance )
{
	return action;
}

void FixedLottery::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void FixedLottery::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void FixedLottery::ShowImGuiNode( const std::string &prefix )
{
	const std::string nodeCaption = prefix + ".Lottery.Fixed";
	if ( ImGui::TreeNode( nodeCaption.c_str() ) )
	{
		action.ShowImGuiNode( "Choice" );

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

		ImGui::TreePop();
	}
}

#endif // USE_IMGUI
