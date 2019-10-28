#pragma once

#include <string>

#include "Serializer.h"

std::string GenerateSerializePath( std::string identifier, Donya::Serializer::Extension extension );

enum class SpriteAttribute
{
	FMODLogoBlack,
	FMODLogoWhite,
};

std::wstring GetSpritePath( SpriteAttribute spriteAttribute );

enum class ModelAttribute
{
	Player,
	Stage,
};

std::string GetModelPath( ModelAttribute modelAttribute );
