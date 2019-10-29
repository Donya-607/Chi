#pragma once

#include "Donya/Collision.h"
#include "Donya/Vector.h"
#include "Donya/Quaternion.h"

#include "BossAI.h"

class Boss
{
private:
	BossAI::ActionStateNum	status;
	Donya::Vector3			pos;
	Donya::Vector3			velocity;
	Donya::Quaternion		orientaion;
	BossAI					AI;
};
