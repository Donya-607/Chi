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
	PlayerIdle,
	PlayerAtk,
	Stage,
	BossIdle,
	BossAtkFast,
	BossAtkSwing,
};

std::string GetModelPath( ModelAttribute modelAttribute );
