#include "Effect.h"
#include "GameLibFunctions.h"
#include "Donya/Useful.h"		// Use IsShowCollision().
#include "Player.h"


void EffectManager::Update()
{
	eruptionEffectManager.Update();
	longAttackEffectManager.Update();
	dustEffect.Update();
	sparkEffect.Update();
	playerAbsorptionParticle.Emit();
	bossAbsorptionParticle.Emit();
	locusParticle.Emit();
	jumpEffect.Update();
	stoneBreakParticle.Emit();
	catapultBreakParticle.Emit();
	shieldEffect.Update();
	disappearanceParticle.Emit();
}

void EffectManager::AccelEffectUpdate(Donya::Vector3 pos, Donya::Vector3 dir)
{
	accelParticle.Emit(pos, dir);
}

void EffectManager::BossAttackMomentEffectUpdate(Donya::Vector3 bossPos, Donya::Vector3 playerPos)
{
	bossAttackMomentEffect.Update(bossPos, playerPos);
}

void EffectManager::Render( fbx_shader &HLSL )
{
	eruptionEffectManager.Render( HLSL );
	longAttackEffectManager.Render( HLSL );
	dustEffect.Render( HLSL );
	sparkEffect.Render( HLSL );
	playerAbsorptionParticle.Draw();
	bossAbsorptionParticle.Draw();
	locusParticle.Draw();
	jumpEffect.Render( HLSL );
	accelParticle.Draw();
	bossAttackMomentEffect.Draw();
	stoneBreakParticle.Draw();
	catapultBreakParticle.Draw();
	shieldEffect.Draw();
	disappearanceParticle.Draw();
}

std::string GetEffectModelPath(EffectModel effectModel)
{

	switch (effectModel)
	{
	case EffectModel::ERUPTION:
		return "./Data/model/Effect/EFE_Boss03_Attack.fbx";			// break;
	case EffectModel::SHIELD_OPEN:
		return "./Data/model/Effect/EFE_Player_Shield_Open.fbx";	// break;
	case EffectModel::SHIELD_DEVELOPMENT:
		return "./Data/model/Effect/EFE_Player_Shield.fbx";			// break;
	case EffectModel::SHIELD_IMPACT:
		return "./Data/model/Effect/EFE_Player_Shield_Impact.fbx";	// break;
	case EffectModel::LONG_ATTACK:
		return "./Data/model/Effect/EFE_Boss04_LongAttack.fbx";		// break;
	default:
		assert(!"Error : Specified unexpect model type."); break;
		break;
	}

	return "ERROR_ATTRIBUTE";
}

void EruptionEffectManager::SetData(Donya::Vector3 _pos, int _startTimer)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (!eruptionEffect[i].GetActivated())
		{
			eruptionEffect[i].SetData(_pos, _startTimer);
			break;
		}
	}
}

void EruptionEffectManager::Update()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		eruptionEffect[i].Update();
	}
}

void EruptionEffectManager::Render( fbx_shader &HLSL )
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		eruptionEffect[i].Render( HLSL );
	}
}

EruptionEffect::EruptionEffect()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].SetPos(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		lengthData[i] = 0;
		angleData[i] = 0;
	}
	state = 0;
	activated = false;

	startFrame = 105;

	hitSphere.resize(MAX_SIZE);
}

void EruptionEffect::Init()
{
	activated = true;
	state = 0;
	timer = 0;
	state = 0;
}

void EruptionEffect::SetData(Donya::Vector3 _pos, int _startTimer)
{
	startTimer = _startTimer;
	pos = _pos;
	aliveNum = 1;
	Donya::Vector3 randPos;
	int length = 0;
	int angle = 0;
	int time = 0;
	for (int i = 0; i < MAX_SIZE; i++)
	{
		lengthData[i] = 0;
		angleData[i] = 0;
	}

	// ’·‚³‚ÆŠp“x‚Årand
	for (int i = 0; i < MAX_SIZE; i++)
	{
		while (1)
		{
			length = 500 + (rand() % (1420 - 500));
			if (rand() % 2 == 0) length *= -1;
			while (1)
			{
				bool setting = false;

				for (int j = 0; j < i; j++)
				{
					if (lengthData[j] == length)
					{
						setting = true;
						length = 500 + (rand() % (1420 - 500));
						if (rand() % 2 == 0) length *= -1;
						break;
					}
				}

				if (!setting) break;
			}
			lengthData[i] = length;

			angle = rand() % 360;
			while (1)
			{
				bool setting = false;

				for (int j = 0; j < i; j++)
				{
					if (angleData[j] - 5 <= angle && angle <= angleData[j] + 5)
					{
						setting = true;
						angle = rand() % 360;
						break;
					}
				}

				if (!setting) break;
			}
			angleData[i] = angle;

			randPos.x = pos.x + sinf(angle * 0.01745f) * length;
			randPos.z = pos.z + cosf(angle * 0.01745f) * length;
			randPos.y = 0.0f;

			int _time = 5 + rand() % 10;
			if (CollideToWall(randPos)) continue;
			//	CollideToWall(randPos);
			data[i].SetPos(randPos);
			data[i].SetTime(time + _time);
			hitSphere[i].pos.x = randPos.x;
			hitSphere[i].pos.y = randPos.y + 25.0f;
			hitSphere[i].pos.z = randPos.z;
			hitSphere[i].radius = 125.0f;
			hitSphere[i].exist = false;
			time += _time;
			break;
		}
	}
	Init();
}

bool EruptionEffect::CollideToWall(Donya::Vector3& pos)
{
	const float bodyRadius = PlayerParam::Get().HitBoxPhysic().radius;
	const float trueFieldRadius = 1420.0f - 50.0f;

	constexpr Donya::Vector3 ORIGIN = Donya::Vector3::Zero();
	const Donya::Vector3 currentDistance = pos - ORIGIN;
	const float currentLength = currentDistance.Length();

	if (trueFieldRadius < currentLength)
	{
		/*float diff = currentLength - trueFieldRadius;
		const Donya::Vector3 direction = currentDistance.Normalized();
		pos = ORIGIN + (direction * (currentLength - diff));*/
		return true;
	}

	return false;
}

void EruptionEffect::Update()
{
	if (activated)
	{
		if (startTimer <= timer)
		{
			switch (state)
			{
			case 0:
				data[0].SetExist(true);
				setAnimFlame(pModel[0].get(), 1);

				state++;
				break;
			case 1:
				for (int i = 0; i < MAX_SIZE; i++)
				{
					if (getAnimFlame(pModel[i].get()) == 107)
					{
						hitSphere[i].exist = true;
						eruqtionParticle[i].Set(hitSphere[i].pos);
					}
					if (getAnimFlame(pModel[i].get()) == 213)
					{
						hitSphere[i].exist = false;
					}
					if (getAnimFlame(pModel[i].get()) == 0)
					{
						data[i].SetExist(false);
					}
				}
				if (aliveNum < MAX_SIZE && data[aliveNum].GetTime() <= timer - startTimer)
				{
					data[aliveNum].SetExist(true);
					setAnimFlame(pModel[aliveNum].get(), 1);
					aliveNum++;
				}
				break;
			default:
				break;
			}

			bool exists = false;
			for (int i = 0; i < MAX_SIZE; i++)
			{
				if (data[i].GetExist())
				{
					exists = true;
				}
			}
			if (!exists)
			{
				activated = false;
				state = 0;
				timer = 0;
			}
		}
		timer++;
	}

	for (int i = 0; i < MAX_SIZE; i++)
	{
		eruqtionParticle[i].Emit();
	}
}

void EruptionEffect::Render( fbx_shader &HLSL )
{
	if (activated)
	{
		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix());
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(GameLib::camera::GetProjectionMatrix());

	#if DEBUG_MODE

		auto GenerateSphere = []()->std::shared_ptr<static_mesh>
		{
			std::shared_ptr<static_mesh> tmpSphere = std::make_shared<static_mesh>();
			createSphere(tmpSphere.get(), 6, 12);
			return tmpSphere;
		};
		static std::shared_ptr<static_mesh> pSphere = GenerateSphere();

	#endif // DEBUG_MODE

		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(1.0f);
				Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation(data[i].GetPos());
				Donya::Vector4x4 R = Donya::Vector4x4::MakeRotationEuler(data[i].GetAngle());
				Donya::Vector4x4 W = S * R * T;
				Donya::Vector4x4 WVP = W * V * P;
				FBXRender(pModel[i].get(), HLSL, WVP, W);
			}

		#if DEBUG_MODE

			if ( Donya::IsShowCollision() && hitSphere[i].exist)
			{
				Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling(hitSphere[i].radius); // Half size->Whole size.
				Donya::Vector4x4 CR = Donya::Vector4x4::MakeRotationEuler(Donya::Vector3(0.0f, 0.0f, 0.0f));
				Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation(hitSphere[i].pos);
				Donya::Vector4x4 CW = CS * CR * CT;
				Donya::Vector4x4 CWVP = CW * V * P;

				OBJRender(pSphere.get(), CWVP, CW, Donya::Vector4(0.0f, 0.3f, 0.3f, 0.6f));
			}

		#endif // DEBUG_MODE
			eruqtionParticle[i].Draw();
		}
	}
}

void LongAttackEffectManager::SetData(Donya::Vector3 _playerPos, Donya::Vector3 _bossPos, int _startTimer)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (!longAttackEffect[i].GetActivated())
		{
			longAttackEffect[i].SetData(_playerPos, _bossPos, _startTimer);
			break;
		}
	}
}

void LongAttackEffectManager::Update()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		longAttackEffect[i].Update();
	}
}

void LongAttackEffectManager::Render(fbx_shader& HLSL)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		longAttackEffect[i].Render(HLSL);
	}
}

LongAttackEffect::LongAttackEffect()
{
	/*pModel.resize(MAX_SIZE);
	data.resize(MAX_SIZE);*/
	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].SetPos(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	state = 0;
	activated = false;
}

void LongAttackEffect::Init()
{
	activated = true;
	state = 0;
	timer = 0;
	state = 0;
}

void LongAttackEffect::SetData(Donya::Vector3 _playerPos, Donya::Vector3 _bossPos, int _startTimer)
{
	startTimer = _startTimer;
	Donya::Vector3 playerPos;
	Donya::Vector3 bossPos;
	playerPos = _playerPos;
	bossPos = _bossPos;

	Donya::Vector3 dir = playerPos - bossPos;
	float dis = sqrtf(powf(dir.x, 2.0f) + powf(dir.y, 2.0f) + powf(dir.z, 2.0f));
	dir.Normalize();

	aliveMaxNum = MAX_SIZE;
	hitSphere.resize(aliveMaxNum);

	aliveMaxReductionNum = 0;

	for (int i = 0; i < aliveMaxNum; i++)
	{
		Donya::Vector3 _pos = _bossPos + (dir * (SIZE * (i + 1)));
		data[i].SetPos(_pos);
		data[i].SetTime(20 * i);
		data[i].SetExist(false);

		hitSphere[i].pos.x = _pos.x;
		hitSphere[i].pos.y = _pos.y + 25.0f;
		hitSphere[i].pos.z = _pos.z;
		hitSphere[i].radius = SIZE - 75.0f;
		hitSphere[i].exist = false;

		if (CollideToWall(data[i].GetPos()))
		{
			aliveMaxReductionNum++;
		}
	}

	aliveNum = 0;
	Init();
}

bool LongAttackEffect::CollideToWall(Donya::Vector3 pos)
{
	const float bodyRadius = PlayerParam::Get().HitBoxPhysic().radius;
	const float trueFieldRadius = 1420.0f - (SIZE - 100.0f);

	constexpr Donya::Vector3 ORIGIN = Donya::Vector3::Zero();
	const Donya::Vector3 currentDistance = pos - ORIGIN;
	const float currentLength = currentDistance.Length();

	if (trueFieldRadius < currentLength)
	{
		/*float diff = currentLength - trueFieldRadius;
		const Donya::Vector3 direction = currentDistance.Normalized();
		pos = ORIGIN + (direction * (currentLength - diff));*/
		return true;
	}

	return false;
}

void LongAttackEffect::Update()
{
	if (activated)
	{
		if (startTimer <= timer)
		{
			/*switch (state)
			{
			case 0:
				data[0].SetExist(true);
				setAnimFlame(pModel[0].get(), 1);

				state++;
				break;
			case 1:
				for (int i = 0; i < MAX_SIZE; i++)
				{
					if (getAnimFlame(pModel[i].get()) == 107)
					{
						hitSphere[i].exist = true;
					}
					if (getAnimFlame(pModel[i].get()) == 213)
					{
						hitSphere[i].exist = false;
					}
					if (getAnimFlame(pModel[i].get()) == 0)
					{
						data[i].SetExist(false);
					}
				}
				if (aliveNum < MAX_SIZE && data[aliveNum].GetTime() <= timer - startTimer)
				{
					data[aliveNum].SetExist(true);
					setAnimFlame(pModel[aliveNum].get(), 1);
					aliveNum++;
				}
				break;
			default:
				break;
			}*/

			for (int i = 0; i < aliveMaxNum - aliveMaxReductionNum; i++)
			{
				if (getAnimFlame(pModel[i].get()) == 62)
				{
					hitSphere[i].exist = true;
				}
				if (getAnimFlame(pModel[i].get()) == 79)
				{
					hitSphere[i].exist = false;
				}
				if (getAnimFlame(pModel[i].get()) == 0)
				{
					data[i].SetExist(false);
				}
			}
			if (aliveNum < aliveMaxNum - aliveMaxReductionNum && data[aliveNum].GetTime() <= timer - startTimer)
			{
				data[aliveNum].SetExist(true);
				setAnimFlame(pModel[aliveNum].get(), 1);
				aliveNum++;
			}

			bool exists = false;
			for (int i = 0; i < aliveMaxNum - aliveMaxReductionNum; i++)
			{
				if (data[i].GetExist())
				{
					exists = true;
				}
			}
			if (!exists)
			{
				activated = false;
				state = 0;
				timer = 0;
			}
		}
		timer++;
	}
}

void LongAttackEffect::Render(fbx_shader& HLSL)
{
	if (activated)
	{
		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix());
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(GameLib::camera::GetProjectionMatrix());

	#if DEBUG_MODE

		auto GenerateSphere = []()->std::shared_ptr<static_mesh>
		{
			std::shared_ptr<static_mesh> tmpSphere = std::make_shared<static_mesh>();
			createSphere(tmpSphere.get(), 6, 12);
			return tmpSphere;
		};
		static std::shared_ptr<static_mesh> pSphere = GenerateSphere();

	#endif // DEBUG_MODE

		for (int i = 0; i < aliveMaxNum - aliveMaxReductionNum; i++)
		{
			if (data[i].GetExist())
			{
				Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(1.0f);
				Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation(data[i].GetPos());
				Donya::Vector4x4 R = Donya::Vector4x4::MakeRotationEuler(data[i].GetAngle());
				Donya::Vector4x4 W = S * R * T;
				Donya::Vector4x4 WVP = W * V * P;
				FBXRender(pModel[i].get(), HLSL, WVP, W);
			}

		#if DEBUG_MODE

			if ( Donya::IsShowCollision() && hitSphere[i].exist)
			{
				Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling(hitSphere[i].radius); // Half size->Whole size.
				Donya::Vector4x4 CR = Donya::Vector4x4::MakeRotationEuler(Donya::Vector3(0.0f, 0.0f, 0.0f));
				Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation(hitSphere[i].pos);
				Donya::Vector4x4 CW = CS * CR * CT;
				Donya::Vector4x4 CWVP = CW * V * P;

				OBJRender(pSphere.get(), CWVP, CW, Donya::Vector4(0.0f, 0.3f, 0.3f, 0.6f));
			}

		#endif // DEBUG_MODE
		}
	}
}

void DustEffect::SetData(Donya::Vector3 _pos, int _stageNo)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (!dustParticle[i].GetEmitting())
		{
			dustParticle[i].Set(_pos, _stageNo);
			break;
		}
	}
}

void DustEffect::Update()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		dustParticle[i].Emit();
	}
}

void DustEffect::Render(fbx_shader& HLSL)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		dustParticle[i].Draw();
	}
}

void SparkEffect::SetData(Donya::Vector3 _pos)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (!sparkParticle[i].GetEmitting())
		{
			sparkParticle[i].Set(_pos);
			break;
		}
	}
}

void SparkEffect::Update()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		sparkParticle[i].Emit();
	}
}

void SparkEffect::Render(fbx_shader& HLSL)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		sparkParticle[i].Draw();
	}
}

void JumpEffect::SetData(Donya::Vector3 _pos, int _stageNo)
{
	if (!dustParticle.GetEmitting())
	{
		eruqtionParticle.Set(_pos);
		dustParticle.Set(_pos, _stageNo);
	}
}

void JumpEffect::Update()
{
	eruqtionParticle.Emit();
	dustParticle.Emit();
}

void JumpEffect::Render(fbx_shader& HLSL)
{
	eruqtionParticle.Draw();
	dustParticle.Draw();
}