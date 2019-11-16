#include "AIBase.h"

#include "Donya/FilePath.h"

#define scast static_cast

#if USE_IMGUI

void ActionStorage::ShowImGuiNode( const std::string &prefix )
{
	const std::string nodeCaption = prefix; ".Actions";
	if ( ImGui::TreeNode( nodeCaption.c_str() ) )
	{
		enum WaitKind { Wait = 0, MoveNear, MoveFar, MoveSide, MoveAimSide, WAIT_KIND_COUNT };
		ImGui::SliderInt( "Wait", &waitNo, 0, WAIT_KIND_COUNT - 1 );
		switch ( waitNo )
		{
		case Wait:			ImGui::Text( "Wait:[Wait]" );			break;
		case MoveNear:		ImGui::Text( "Wait:[Move.Near]" );		break;
		case MoveFar:		ImGui::Text( "Wait:[Move.Far]" );		break;
		case MoveSide:		ImGui::Text( "Wait:[Move.Side]" );		break;
		case MoveAimSide:	ImGui::Text( "Wait:[Move.AimSIde]" );	break;
		default:			ImGui::Text( "Error" );					break;
		}

		// HACK:I should fetch the max of range from argument. because the user of "nextAttackNo" may be out of range.
		constexpr int ENOUGH_RANGE = 3;
		ImGui::SliderInt( "NextAttack", &nextAttackNo, -1, ENOUGH_RANGE );
		if ( nextAttackNo == -1 ) { ImGui::Text( "Attack:[Random]" ); }
		else { ImGui::Text( "Attack:[%d]", nextAttackNo ); }

		ImGui::TreePop();
	}
}

void ShowImGuiChooserNode( const std::string &prefix, std::unique_ptr<LotteryBase> *ppChooserBase, int *pNowKind )
{
	int oldKind = *pNowKind;
	ImGui::SliderInt( ( prefix + ".Kind" ).c_str(), pNowKind, 0, scast<int>( ChooserKind::CHOOSERS_COUNT ) - 1 );
	if ( oldKind != *pNowKind )
	{
		ResetLotteryKind( ppChooserBase, *pNowKind );
	}

	( *ppChooserBase )->ShowImGuiNode( prefix );
}

#endif // USE_IMGUI

void ResetLotteryKind( std::unique_ptr<LotteryBase> *ppChooserBase, int chooserKind )
{
	return ResetLotteryKind( ppChooserBase, scast<ChooserKind>( chooserKind ) );
}
void ResetLotteryKind( std::unique_ptr<LotteryBase> *ppChooserBase, ChooserKind chooserKind )
{
	_ASSERT_EXPR( chooserKind != ChooserKind::CHOOSERS_COUNT , L"Error : passed chooser's number is invalid !" );

	switch ( chooserKind )
	{
	case ChooserKind::Distance:
		ppChooserBase->reset( nullptr );
		*ppChooserBase = std::make_unique<DistanceLottery>();
		return;
	case ChooserKind::Fixed:
		ppChooserBase->reset( nullptr );
		*ppChooserBase = std::make_unique<FixedLottery>();
		return;
	default: return;
	}
}

DistanceLottery::DistanceLottery() : LotteryBase(), data()
{}
DistanceLottery::~DistanceLottery() = default;

void DistanceLottery::Init()
{
	LoadParameter();
}
void DistanceLottery::Uninit()
{
	// No op.
}

ActionStorage DistanceLottery::Lottery( float nDistance )
{
	if ( data.empty() ) { return ActionStorage::Zero(); }
	// else

	for ( const auto &it : data )
	{
		if ( nDistance <= it.distance )
		{
			return it.action;
		}
	}
	
	// Fail safe.
	return data.back().action;
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

void DistanceLottery::ShowImGuiNode( const std::string &prefix, bool isAllowUnitIO )
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

		if ( isAllowUnitIO && ImGui::TreeNode( "File.I/O" ) )
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

void FixedLottery::Init()
{
	LoadParameter();
}
void FixedLottery::Uninit()
{
	// No op.
}

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

void FixedLottery::ShowImGuiNode( const std::string &prefix, bool isAllowUnitIO )
{
	const std::string nodeCaption = prefix + ".Lottery.Fixed";
	if ( ImGui::TreeNode( nodeCaption.c_str() ) )
	{
		action.ShowImGuiNode( "Choice" );

		if ( isAllowUnitIO && ImGui::TreeNode( "File.I/O" ) )
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
