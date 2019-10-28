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
		return "./Data/model/Player/Player.fbx"; // break;
	case ModelAttribute::Stage:
		return "./Data/model/Stage/Stage01.fbx"; // break;
	default:
		assert( !"Error : Specified unexpect model type." ); break;
		break;
	}

	return "ERROR_ATTRIBUTE";
}
