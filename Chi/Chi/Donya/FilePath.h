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
	PlayerRun,
	PlayerAtk,
	PlayerDefend,
	Stage,
	GolemIdle,
	GolemAtkSwing,
	GolemAtkFast,
	GolemAtkRotate,
	KnightIdle,
	KnightRunFront,
	KnightAtkExplosion,
	KnightAtkSwing,
	KnightAtkRaid,
	KnightFxExplosion,
	RivalIdle,
	RivalRun,
	RivalBreak,
	RivalDefeat,
	RivalAtkBarrage,
	RivalAtkLine,
	RivalAtkRaid,
	RivalAtkRushWait,
	RivalAtkRushSlash,
};

std::string GetModelPath( ModelAttribute modelAttribute );

enum class MusicAttribute
{
	PlayerDefend,
	PlayerProtected,

	MUSIC_COUNT
};

std::string GetMusicPath( MusicAttribute musicAttribute );
