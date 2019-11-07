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
	case ModelAttribute::PlayerIdle:
		return "./Data/model/Player/MDL_Player_Wait.fbx";				// break;
	case ModelAttribute::PlayerRun:
		return "./Data/model/Player/MDL_Player_Run.fbx";				// break;
	case ModelAttribute::PlayerAtk:
		return "./Data/model/Player/MDL_Player_Attack.fbx";				// break;
	case ModelAttribute::PlayerDefend:
		return "./Data/model/Player/MDL_Player_Shield.fbx";				// break;
	case ModelAttribute::Stage:
		return "./Data/model/Stage/MDL_Stage01.fbx";					// break;
	case ModelAttribute::GolemIdle:
		return "./Data/model/Boss/MDL_Boss03_Wait.fbx";					// break;
	case ModelAttribute::GolemAtkSwing:
		return "./Data/model/Boss/MDL_Boss03_Attack_Bandgap.fbx";		// break;
	case ModelAttribute::GolemAtkFast:
		return "./Data/model/Boss/MDL_Boss03_Attack_Continuous.fbx";	// break;
	case ModelAttribute::GolemAtkRotate:
		return "./Data/model/Boss/MDL_Boss03_Attack_Rotate.fbx";		// break;
	case ModelAttribute::KnightIdle:
		return "./Data/model/Boss/MDL_Boss02_Wait.fbx";					// break;
	case ModelAttribute::KnightRunFront:
		return "./Data/model/Boss/MDL_Boss02_Walk_Forward.fbx";			// break;
	case ModelAttribute::KnightAtkExplosion:
		return "./Data/model/Boss/MDL_Boss02_Attack_Explosion.fbx";		// break;
	case ModelAttribute::KnightAtkSwing:
		return "./Data/model/Boss/MDL_Boss02_Attack_Swing.fbx";			// break;
	case ModelAttribute::KnightAtkRaid:
		return "./Data/model/Boss/MDL_Boss02_Attack_JampAttack.fbx";	// break;
	default:
		assert( !"Error : Specified unexpect model type." ); break;
		break;
	}

	return "ERROR_ATTRIBUTE";
}

std::string GetMusicPath( MusicAttribute musicAttribute )
{
	switch ( musicAttribute )
	{
	case MusicAttribute::PlayerDefend:
		return "./Data/Sounds/SE/Player/Defend.wav";				// break;
	case MusicAttribute::PlayerProtected:
		return "./Data/Sounds/SE/Player/Protected.wav";				// break;
	default:
		assert( !"Error : Specified unexpect music type." ); break;
		break;
	}

	return "ERROR_ATTRIBUTE";
}
