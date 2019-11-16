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
	Stage01,
	TutorialStage,
	TutorialBoss,
	TutorialBossStone,
	KnightIdle,
	KnightDefeat,
	KnightRunFront,
	KnightRunLeft,
	KnightRunRight,
	KnightRunBack,
	KnightAtkExplosion,
	KnightAtkSwing,
	KnightAtkRaid,
	KnightFxExplosion,
	GolemIdle,
	GolemDefeat,
	GolemAtkSwing,
	GolemAtkFast,
	GolemAtkRotate,
	RivalIdle,
	RivalRunFront,
	RivalRunLeft,
	RivalRunRight,
	RivalBreak,
	RivalLeave,
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
