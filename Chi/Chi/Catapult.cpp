#include "Catapult.h"
#include "Donya/FilePath.h"


void Catapult::Init( Donya::Vector3 _pos, Donya::Vector3 _scale, Donya::Vector3 _angle )
{
	loadFBX(pModel.get(), GetModelPath(ModelAttribute::TutorialBoss));
	loadFBX(stone.pModel.get(), GetModelPath(ModelAttribute::TutorialBossStone));

	setStopAnimation(pModel.get(), true);

	pos   = _pos;
	scale = _scale;
	angle = _angle;

	attackCnt = 0;

	hitOBB.pos = pos;
	hitOBB.size = Donya::Vector3(881.0f, 2345.0f, 1473.0f);
	hitOBB.exist = true;
	hitOBB.enable = true;

	//stone.Init(Donya::Vector3(0.0f, 3490.0f, 7350.0f), // pos
	//	Donya::Vector3(0.0f, 0.0f, 0.0f),  // speed
	//	Donya::Vector3(0.0f, -1.0f, 0.0f), // accel
	//	Donya::Vector3(1.0f, 1.0f, 1.0f),  // scale
	//	Donya::Vector3(0.0f, 0.0f, 0.0f)); // angle
}

void Catapult::Update( Donya::Vector3 _playerPos )
{
	if (getAnimFlame(pModel.get()) == 22)
	{
		Donya::Vector3 catapultToPlayerVec = _playerPos - Donya::Vector3(0.0f, pos.y + 1350.0f, pos.z + 500.0f);
		catapultToPlayerVec.Normalize();

		stone.Init(Donya::Vector3(0.0f, pos.y + 1350.0f, pos.z + 500.0f), // pos
			Donya::Vector3(75.0f * catapultToPlayerVec.x, 0.0f, 75.0f * catapultToPlayerVec.z), // speed
			Donya::Vector3(0.0f, 2.0f * catapultToPlayerVec.y, 0.0f), // accel
			Donya::Vector3(1.0f, 1.0f, 1.0f),  // scale
			Donya::Vector3(0.0f, 0.0f, 0.0f)); // angle
	}

	stone.Update();
}

void Catapult::Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection)
{
	if (hitOBB.exist)
	{
		Donya::Vector4x4 W = CalcWorldMatrix();
		Donya::Vector4x4 WVP = W * matView * matProjection;

		FBXRender(pModel.get(), HLSL, WVP, W);

		auto GenerateCube = []()->std::shared_ptr<static_mesh>
		{
			std::shared_ptr<static_mesh> tmpCube = std::make_shared<static_mesh>();
			createCube(tmpCube.get());
			return tmpCube;
		};
		static std::shared_ptr<static_mesh> pCube = GenerateCube();

		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling(hitOBB.size); // Half size->Whole size.
		Donya::Vector4x4 CR = Donya::Vector4x4::MakeRotationEuler(Donya::Vector3(0.0f, 0.0f, 0.0f));
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation(hitOBB.pos);
		Donya::Vector4x4 CW = CS * CR * CT;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender(pCube.get(), CWVP, CW, Donya::Vector4(0.0f, 0.8f, 0.3f, 0.6f));

		stone.Draw(HLSL, matView, matProjection);
	}
}

void Catapult::UnInit()
{
	pos   = Donya::Vector3::Zero();
	scale = Donya::Vector3::Zero();
	angle = Donya::Vector3::Zero();
	attackCnt = 0;
}

void Catapult::ImGui()
{
	ImGui::Begin("Catapult");
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::DragFloat3("hitOBB.size", &hitOBB.size.x);
	ImGui::End();

	stone.ImGui();
}


void Stone::Init( Donya::Vector3 _pos, Donya::Vector3 _speed, Donya::Vector3 _accel, Donya::Vector3 _scale, Donya::Vector3 _angle )
{
	pos   = _pos;
	speed = _speed;
	accel = _accel;
	scale = _scale;
	angle = _angle;
	exist = true;

	setAnimFlame(pModel.get(), 0);

	hitSphere.pos.x = pos.x;
	hitSphere.pos.y = pos.y + 100.0f;
	hitSphere.pos.z = pos.z;
	hitSphere.radius = 230.0f * 2.0f;
	hitSphere.exist = true;
	hitSphere.enable = true;
}

void Stone::Update()
{
	if (exist)
	{
		pos.x += speed.x;
		pos.z += speed.z;

		speed.y += accel.y;
		pos.y += speed.y;

		hitSphere.pos.x = pos.x;
		hitSphere.pos.y = pos.y + 100.0f;
		hitSphere.pos.z = pos.z;
	}
}

void Stone::Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection)
{
	if (exist)
	{
		Donya::Vector4x4 W = CalcWorldMatrix();
		Donya::Vector4x4 WVP = W * matView * matProjection;

		FBXRender(pModel.get(), HLSL, WVP, W);

		auto GenerateSphere = []()->std::shared_ptr<static_mesh>
		{
			std::shared_ptr<static_mesh> tmpSphere = std::make_shared<static_mesh>();
			createSphere(tmpSphere.get(), 6, 12);
			return tmpSphere;
		};
		static std::shared_ptr<static_mesh> pSphere = GenerateSphere();

		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling(hitSphere.radius); // Half size->Whole size.
		Donya::Vector4x4 CR = Donya::Vector4x4::MakeRotationEuler(Donya::Vector3(0.0f, 0.0f, 0.0f));
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation(hitSphere.pos);
		Donya::Vector4x4 CW = CS * CR * CT;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender(pSphere.get(), CWVP, CW, Donya::Vector4(0.0f, 0.3f, 0.3f, 0.6f));
	}
}

void Stone::UnInit()
{
	pos   = Donya::Vector3::Zero();
	speed = Donya::Vector3::Zero();
	accel = Donya::Vector3::Zero();
	scale = Donya::Vector3::Zero();
	angle = Donya::Vector3::Zero();
	exist = false;
}

void Stone::ImGui() 
{
	ImGui::Begin("Stone");
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::DragFloat("radius", &hitSphere.radius);
	ImGui::End();
}
