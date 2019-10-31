#include "FilePath.h"

#include "Constant.h"	// Use DEBUG_MODE.
#include "Useful.h"		// Use IsExistFile().

std::string GenerateSerializePath( std::string identifier, Donya::Serializer::Extension extension )
{
	std::string ext{};
	switch ( extension )
	{
	case Donya::Serializer::Extension::BINARY:	ext = ".bin";	break;
	case Donya::Serializer::Extension::JSON:	ext = ".json";	break;
	default: return "ERROR_EXTENSION";
	}

	return "./Data/Parameters/" + identifier + ext;
}

std::wstring GetSpritePath( SpriteAttribute sprAttribute )
{
	switch ( sprAttribute )
	{
	case SpriteAttribute::FMODLogoBlack:
		return L"./Data/Images/Rights/FMOD Logo Black - White Background.png";	// break;
	case SpriteAttribute::FMODLogoWhite:
		return L"./Data/Images/Rights/FMOD Logo White - Black Background.png";	// break;
	
	default:
		assert( !"Error : Specified unexpect sprite type." ); break;
	}

	return L"ERROR_ATTRIBUTE";
}

std::string GetModelPath( ModelAttribute modelAttribute )
{
	switch ( modelAttribute )
	{
	case ModelAttribute::Player:
		return "./Data/model/Player/MDL_Player.fbx";					// break;
	case ModelAttribute::PlayerAtk:
		return "./Data/model/Player/MDL_Player_Attack.fbx";				// break;
	case ModelAttribute::Stage:
		return "./Data/model/Stage/MDL_Stage01.fbx";					// break
	;case ModelAttribute::BossIdle:
		return "./Data/model/Boss/MDL_Boss03_Wait.fbx";					// break;
	;case ModelAttribute::BossAtkFast:
		return "./Data/model/Boss/MDL_Boss03_Attack_Continuous.fbx";	// break;
	;case ModelAttribute::BossAtkSwing:
		return "./Data/model/Boss/MDL_Boss03_Attack_Bandgap.fbx";		// break;
	default:
		assert( !"Error : Specified unexpect model type." ); break;
		break;
	}

	return "ERROR_ATTRIBUTE";
}
