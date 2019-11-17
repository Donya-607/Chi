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
		return "./Data/model/Player/MDL_Player_Wait.fbx";					// break;
	case ModelAttribute::PlayerRun:
		return "./Data/model/Player/MDL_Player_Run.fbx";					// break;
	case ModelAttribute::PlayerAtk:
		return "./Data/model/Player/MDL_Player_Attack.fbx";					// break;
	case ModelAttribute::PlayerDefend:
		return "./Data/model/Player/MDL_Player_Shield.fbx";					// break;
	case ModelAttribute::PlayerDefeat:
		return "./Data/model/Player/MDL_Player_Death.fbx";					// break;
	case ModelAttribute::ShieldOpen:
		return "./Data/model/Effect/EFE_Player_Shield_Open.fbx";			// break;
	case ModelAttribute::ShieldIdle:
		return "./Data/model/Effect/EFE_Player_Shield.fbx";					// break;
	case ModelAttribute::ShieldReact:
		return "./Data/model/Effect/EFE_Player_Shield_Impact.fbx";			// break;
	case ModelAttribute::Stage01:
		return "./Data/model/Stage/MDL_Stage01.fbx";						// break;
	case ModelAttribute::TutorialStage:
		return "./Data/model/Tutorial/MDL_TutorialMap.fbx";					// break;
	case ModelAttribute::TutorialBoss:
		return "./Data/model/Tutorial/MDL_Catapult_Attack.fbx";				// break;
	case ModelAttribute::TutorialBossStone:
		return "./Data/model/Tutorial/MDL_Stone.fbx";						// break;
	case ModelAttribute::KnightIdle:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Wait.fbx";				// break;
	case ModelAttribute::KnightDefeat:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Death.fbx";				// break;
	case ModelAttribute::KnightRunFront:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Walk_Forward.fbx";		// break;
	case ModelAttribute::KnightRunLeft:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Walk_Left.fbx";			// break;
	case ModelAttribute::KnightRunRight:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Walk_Right.fbx";		// break;
	case ModelAttribute::KnightRunBack:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Walk_Back.fbx";			// break;
	case ModelAttribute::KnightAtkExplosion:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Attack_Explosion.fbx";	// break;
	case ModelAttribute::KnightAtkSwing:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Attack_Swing.fbx";		// break;
	case ModelAttribute::KnightAtkRaid:
		return "./Data/model/Boss/Boss02/MDL_Boss02_Attack_JumpAttack.fbx";	// break;
	case ModelAttribute::KnightFxExplosion:
		return "./Data/model/Effect/EFE_Boss02_Explosion.fbx";				// break;
	case ModelAttribute::GolemIdle:
		return "./Data/model/Boss/Boss03/MDL_Boss03_Wait.fbx";				// break;
	case ModelAttribute::GolemDefeat:
		return "./Data/model/Boss/Boss03/MDL_Boss03_Death.fbx";				// break;
	case ModelAttribute::GolemAtkSwing:
		return "./Data/model/Boss/Boss03/MDL_Boss03_Attack_Bandgap.fbx";	// break;
	case ModelAttribute::GolemAtkFast:
		return "./Data/model/Boss/Boss03/MDL_Boss03_Attack_Continuous.fbx";	// break;
	case ModelAttribute::GolemAtkRotate:
		return "./Data/model/Boss/Boss03/MDL_Boss03_Attack_Rotate.fbx";		// break;
	case ModelAttribute::RivalIdle:
		return "./Data/model/Boss/Boss04/Boss04_Wait.fbx";					// break;
	case ModelAttribute::RivalRunFront:
		return "./Data/model/Boss/Boss04/Boss04_Run.fbx";					// break;
	case ModelAttribute::RivalRunLeft:
		return "./Data/model/Boss/Boss04/Boss04_Left.fbx";					// break;
	case ModelAttribute::RivalRunRight:
		return "./Data/model/Boss/Boss04/Boss04_Right.fbx";					// break;
	case ModelAttribute::RivalBreak:
		return "./Data/model/Boss/Boss04/Boss04_Impact.fbx";				// break;
	case ModelAttribute::RivalLeave:
		return "./Data/model/Boss/Boss04/Boss04_Back.fbx";					// break;
	case ModelAttribute::RivalDefeat:
		return "./Data/model/Boss/Boss04/Boss04_Death.fbx";					// break;
	case ModelAttribute::RivalAtkBarrage:
		return "./Data/model/Boss/Boss04/Boss04_3Attack.fbx";				// break;
	case ModelAttribute::RivalAtkLine:
		return "./Data/model/Boss/Boss04/Boss04_LongAttack.fbx";			// break;
	case ModelAttribute::RivalAtkRaid:
		return "./Data/model/Boss/Boss04/Boss04_JumpAttack.fbx";			// break;
	case ModelAttribute::RivalAtkRushWait:
		return "./Data/model/Boss/Boss04/Boss04_RushWait.fbx";				// break;
	case ModelAttribute::RivalAtkRushSlash:
		return "./Data/model/Boss/Boss04/Boss04_RushSlash.fbx";				// break;
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
