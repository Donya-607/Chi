#include "Effect.h"
#include "GameLibFunctions.h"
#include "Player.h"

//
//std::string EffectManager::GetModelPath(int effectNum)
//{
//	switch (effectNum)
//	{
//	case EffectNum::FIRE:
//		return "./Data/model/Effect/fire.fbx";					// break;
//	case EffectNum::THUNDER:
//		return "./Data/model/Effect/thunder.fbx";				// break;
//	default:
//		assert(!"Error : Specified unexpect model type."); break;
//		break;
//	}
//
//	return "ERROR_ATTRIBUTE";
//}
//
//EffectManager::EffectManager()
//{
//	for (int i = 0; i < MAX_SIZE; i++)
//	{
//		pEffect[i] = std::make_shared<skinned_mesh>();
//	}
//}
//
//void EffectManager::Load()
//{
//	std::shared_ptr<skinned_mesh> tmpFbx = std::make_shared<skinned_mesh>();
//	for (int i = 0; i < MAX_SIZE; i++)
//	{
//		loadFBX(tmpFbx.get(), GetModelPath(i));
//		pEffect[i] = tmpFbx;
//	}
//}
//
//std::shared_ptr<skinned_mesh> EffectManager::GetEffect(EffectNum effectNum)
//{
//	return pEffect[effectNum];
//}


void EffectManager::Init()
{
//	effect.resize(MAX_SIZE);

//	for (int i = 0; i < MAX_SIZE; i++)
//	{
//		switch (i)
//		{
//		case EffectManager::ERUPTION:
//			effect[i] = std::make_unique<EruptionEffectManager>();
//			break;
//		case EffectManager::SHIELD_DEVELOPMENT:
//			effect[i] = std::make_unique<ShieldDevelopment>();
//			break;
//		// TODO : add effect
//		}
//	}
}

void EffectManager::Set(EffectType effectType, Donya::Vector3 pos, int startTime)
{
	//effect[effectType]->SetData(pos, startTime);

	switch (effectType)
	{
	case EffectManager::ERUPTION:
		eruqtionEffectManager.SetData(pos, startTime);
		break;
		// TODO : add effect
	}
}

void EffectManager::Set(EffectType effectType)
{
	//effect[effectType]->Set();

	switch (effectType)
	{
	case EffectManager::SHIELD_DEVELOPMENT:
		shieldDevelopment.Set();
		break;
		// TODO : add effect
	}
}

void EffectManager::ReSet(EffectType effectType)
{
	//effect[effectType]->ReSet();

	switch (effectType)
	{
	case EffectManager::SHIELD_DEVELOPMENT:
		shieldDevelopment.ReSet();
		break;
		// TODO : add effect
	}
}

void EffectManager::Update()
{
	/*for (int i = 0; i < MAX_SIZE; i++)
	{
		effect[i]->Update();
	}*/

	eruqtionEffectManager.Update();
}

void EffectManager::Update(Donya::Vector3 pos)
{
	shieldDevelopment.Update(pos);
}

void EffectManager::Render( fbx_shader &HLSL )
{
	/*for (int i = 0; i < MAX_SIZE; i++)
	{
		effect[i]->Render();
	}*/

	eruqtionEffectManager.Render( HLSL );
	shieldDevelopment.Render( HLSL );
}

std::string GetEffectModelPath(EffectModel effectModel)
{

	switch (effectModel)
	{
	case EffectModel::ERUPTION:
		return "./Data/model/Effect/EFE_Boss03_Attack.fbx";		// break;
	case EffectModel::SHIELD_DEVELOPMENT:
		return "./Data/model/Effect/EFE_Player_Shield.fbx";		// break;
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
	/*pModel.resize(MAX_SIZE);
	data.resize(MAX_SIZE);*/
	auto GenerateFBX = []()->std::shared_ptr<skinned_mesh>
	{
		std::shared_ptr<skinned_mesh> tmpFBX = std::make_shared<skinned_mesh>();
		loadFBX(tmpFBX.get(), GetEffectModelPath(EffectModel::ERUPTION));
		return tmpFBX;
	};
	for (int i = 0; i < MAX_SIZE; i++)
	{
		pModel[i] = std::make_unique<skinned_mesh>();
		pModel[i] = GenerateFBX();
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
}

void EruptionEffect::Render( fbx_shader &HLSL )
{
	if (activated)
	{
		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix());
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(GameLib::camera::GetProjectionMatrix());

		auto GenerateSphere = []()->std::shared_ptr<static_mesh>
		{
			std::shared_ptr<static_mesh> tmpSphere = std::make_shared<static_mesh>();
			createSphere(tmpSphere.get(), 6, 12);
			return tmpSphere;
		};
		static std::shared_ptr<static_mesh> pSphere = GenerateSphere();

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

			if (hitSphere[i].exist)
			{
				Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling(hitSphere[i].radius); // Half size->Whole size.
				Donya::Vector4x4 CR = Donya::Vector4x4::MakeRotationEuler(Donya::Vector3(0.0f, 0.0f, 0.0f));
				Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation(hitSphere[i].pos);
				Donya::Vector4x4 CW = CS * CR * CT;
				Donya::Vector4x4 CWVP = CW * V * P;

				OBJRender(pSphere.get(), CWVP, CW, Donya::Vector4(0.0f, 0.3f, 0.3f, 0.6f));
			}
		}
	}
}

ShieldDevelopment::ShieldDevelopment()
{
	auto GenerateFBX = []()->std::shared_ptr<skinned_mesh>
	{
		std::shared_ptr<skinned_mesh> tmpFBX = std::make_shared<skinned_mesh>();
		loadFBX(tmpFBX.get(), GetEffectModelPath(EffectModel::SHIELD_DEVELOPMENT));
		return tmpFBX;
	};
	pModel = std::make_unique<skinned_mesh>();
	pModel = GenerateFBX();
	data.SetPos(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
}

void ShieldDevelopment::Init()
{

}

void ShieldDevelopment::Set()
{
	data.SetExist(true);
}

void ShieldDevelopment::ReSet()
{
	data.SetExist(false);
}

void ShieldDevelopment::Update(Donya::Vector3 pos)
{
	if (data.GetExist())
	{
		data.SetPos(pos);
	}
}

void ShieldDevelopment::Render( fbx_shader &HLSL )
{
	if (data.GetExist())
	{
		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix());
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(GameLib::camera::GetProjectionMatrix());

		Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(1.0f);
		Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation(data.GetPos());
		Donya::Vector4x4 R = Donya::Vector4x4::MakeRotationEuler(data.GetAngle());
		Donya::Vector4x4 W = S * R * T;
		Donya::Vector4x4 WVP = W * V * P;
		FBXRender(pModel.get(), HLSL, WVP, W);
	}
}
