#include "Particle.h"
#include "scene.h"
#include "sceneManager.h"
#include "Donya/Vector.h"
#include "GameLibFunctions.h"
#include "Effect.h"
#include "GameLibFunctions.h"
#include "UI.h"


void SceneEffect::init()  
{
	isStack = false;
	keyCnt = 0;
	keyCnt2 = 0;

	stage.Init(0);

	cameraPos = Donya::Vector3{ 0.0f, 256.0f, -1000.0f };
	cameraFocusOffset = Donya::Vector3{ 0.0f, 80.0f, 0.0f };

	loadShader
	(
		shader,
		"./Data/shader/skinned_mesh_has_born_vs.cso",
		"./Data/shader/skinned_mesh_ps.cso",
		"./Data/shader/skinned_mesh_vs.cso",
		"./Data/shader/skinned_mesh_no_uv_ps.cso"
	);

	lights.direction.color = Donya::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	getLineLight().setLineLight(lights.direction.position,lights.direction.direction, lights.direction.color);
	for (const auto i : lights.points)
	{
		setPointLight(i);
	}

//	EffectManager::GetInstance()->Init();
	originPos = Donya::Vector3::Zero();
	direction = Donya::Vector3(1.0f, 1.0f, 1.0f);
#if 0
	createBillboard(&data.pMesh, L"./Data/Images/UI/particle.png");
#else
	//createBillboard(&data.pMesh, L"./Data/Images/UI/Telop.png");
#endif
	//UIManager::GetInstance()->Init();

	shieldExist = false;

	playerPos = DirectX::XMFLOAT3(0.0f, 0.0f, -750.0);
	bossPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0);
}
void SceneEffect::update()
{
	GameLib::camera::setPos(cameraPos);
	GameLib::camera::setTarget(Donya::Vector3(0.0f, 0.0f, 0.0f) + cameraFocusOffset);

	if (GetKeyState('T') < 0)
	{
		if (keyCnt == 0)
		{
			pSceneManager->setNextScene(new sceneTitle, false);
		}
		keyCnt++;
	}
	else
	{
		keyCnt = 0;
	}

	if (GetKeyState('E') < 0)
	{
		if (keyCnt2 == 0)
		{
		//	eruptionEffect.SetData();
		//	EffectManager::GetInstance()->Set(EffectManager::GetInstance()->ERUPTION, originPos, 0);
		}
		keyCnt2++;
	}
	else
	{
		keyCnt2 = 0;
	}

	EffectManager::GetInstance()->Update();
	EffectManager::GetInstance()->AccelEffectUpdate(originPos, direction);
	EffectManager::GetInstance()->BossAttackMomentEffectUpdate(playerPos, bossPos);

	stage.Update();

	eruqtionParticle.Emit();
	absorptionParticle.Emit();
	dustParticle.Emit();
	sparkParticle.Emit();
	locusParticle.Emit();
	accelParticle.Emit(originPos, direction);

	//UIManager::GetInstance()->Update(Donya::Vector3(0.0f, 0.0f, 0.0f));
}
void SceneEffect::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);


	Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix());
	Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(GameLib::camera::GetProjectionMatrix());

	EffectManager::GetInstance()->Render( shader );

	stage.Draw( shader, V, P );

	eruqtionParticle.Draw();
	absorptionParticle.Draw();
	dustParticle.Draw();
	sparkParticle.Draw();
	locusParticle.Draw();
	accelParticle.Draw();

	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
	//billboardRender(&data.pMesh, viewProjection, data.pos, data.scale, data.angle, getCamPos(), data.texPos, data.texSize);
	origin_SRV = (void*)GameLib::getOriginalScreen();

	//blur’l‚ðŒã‚Å‘ã“ü
	postEffect_Bloom(0, false);
	//ƒ‚ƒmƒg[ƒ“‚É‚·‚é‚Æ‚«‚Í‘æˆêˆø”‚ð‰º‚°‚é
	filterScreen(1.0f);

//	UIManager::GetInstance()->Draw();
}
void SceneEffect::uninit()
{

}
void SceneEffect::imGui()
{
	//UIManager::GetInstance()->Imgui();
	ImGui::Begin("Effect");
	if (ImGui::TreeNode("BillBoard Test Draw Parametor"))
	{
		ImGui::DragFloat3("pos", &data.pos.x);
		ImGui::DragFloat2("scale", &data.scale.x);
		ImGui::DragFloat2("texPos", &data.texPos.x);
		ImGui::DragFloat2("texSize", &data.texSize.x);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Eruqtion Particle Emit"))
	{
		if (ImGui::Button("Emit"))
		{
			eruqtionParticle.Set(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Dust Particle Emit"))
	{
		static int stageNum = 1;
		ImGui::InputInt("stageNum", &stageNum);
		if (ImGui::Button("Emit"))
		{
			// dustParticle.Set(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), stageNum);
			EffectManager::GetInstance()->DustEffectSet(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), stageNum);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Jump Particle Emit"))
	{
		static DirectX::XMFLOAT3 _pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0);
		ImGui::DragFloat3("emit pos", &_pos.x);
		if (ImGui::Button("Emit"))
		{
			//eruqtionParticle.Set(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
			//dustParticle.Set(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), stageNum);
			EffectManager::GetInstance()->JumpEffectSet(_pos);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Absorption Particle Emit"))
	{
		static DirectX::XMFLOAT3 _pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0);
		ImGui::DragFloat3("emit pos", &_pos.x);
		if (ImGui::Button("Player Set"))
		{
			//absorptionParticle.Set(_pos);
			EffectManager::GetInstance()->PlayerAbsorptionEffectSet(_pos);
		}
		if (ImGui::Button("Player ReSet"))
		{
			//absorptionParticle.ReSet();
			EffectManager::GetInstance()->PlayerAbsorptionEffectReSet();
		}
		if (ImGui::Button("Boss Set"))
		{
			//absorptionParticle.Set(_pos);
			EffectManager::GetInstance()->BossAbsorptionEffectSet(_pos);
		}
		if (ImGui::Button("Boss ReSet"))
		{
			//absorptionParticle.ReSet();
			EffectManager::GetInstance()->BossAbsorptionEffectReSet();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("SparkParticle Particle Emit"))
	{
		static DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0);
		ImGui::DragFloat3("emit pos ", &pos.x);
		if (ImGui::Button("Emit"))
		{
			//sparkParticle.Set(pos);
			EffectManager::GetInstance()->SparkEffectSet(pos);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("LocusParticle Particle Emit"))
	{
		static DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0);
		static DirectX::XMFLOAT3 dir = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0);
		ImGui::DragFloat3("emit pos  ", &pos.x);
		ImGui::DragFloat3("emit direction", &dir.x);
		if (ImGui::Button("Emit"))
		{
			//locusParticle.Set(pos, dir);
			EffectManager::GetInstance()->LocusParticleSet(pos, dir);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Boss4 LongAttack Effect"))
	{
		static DirectX::XMFLOAT3 playerPos = DirectX::XMFLOAT3(0.0f, 0.0f, -750.0);
		static DirectX::XMFLOAT3 bossPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0);
		//ImGui::InputFloat("size", &size);
		ImGui::DragFloat3("player pos", &playerPos.x);
		ImGui::DragFloat3("boss pos", &bossPos.x);
		if (ImGui::Button("On"))
		{
			EffectManager::GetInstance()->LongAttackEffectSet(playerPos, bossPos);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Accel Particle Emit"))
	{
		ImGui::DragFloat3("emit pos", &originPos.x);
		ImGui::DragFloat3("emit direction", &direction.x);
		if (ImGui::Button("Set"))
		{
			//accelParticle.Set();
			EffectManager::GetInstance()->AccelEffectSet();
		}
		if (ImGui::Button("ReSet"))
		{
			//accelParticle.ReSet();
			EffectManager::GetInstance()->AccelEffectReSet();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Boss Attack Effect Emit"))
	{
		ImGui::DragFloat3("player pos", &playerPos.x);
		ImGui::DragFloat3("boss pos", &bossPos.x);
		if (ImGui::Button("Set"))
		{
			EffectManager::GetInstance()->BossAttackMomentEffectSet(30, 0);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Stone Break Effect Emit"))
	{
		static DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0);
		ImGui::DragFloat3("emit pos ", &pos.x);
		if (ImGui::Button("Set"))
		{
			EffectManager::GetInstance()->StoneBreakEffectSet(pos);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Catapult Break Effect Emit"))
	{
		static DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0);
		ImGui::DragFloat3("emit pos ", &pos.x);
		if (ImGui::Button("Set"))
		{
			EffectManager::GetInstance()->CatapultBreakEffectSet(pos);
		}
		ImGui::TreePop();
	}
	//if (ImGui::TreeNode("UI Effect"))
	//{
	//	if (ImGui::Button("On"))
	//	{
	//		UIManager::GetInstance()->Init();
	//	}
	//	ImGui::TreePop();
	//}
	ImGui::End();

	ImGui::Begin("Light");
	if (ImGui::TreeNode("DirectionalLight"))
	{
		ImGui::ColorEdit4("Color", &lights.direction.color.x);
		ImGui::SliderFloat3("Direction", &lights.direction.direction.x, -8.0f, 8.0f);

		getLineLight().setLineLight(lights.direction.position, lights.direction.direction, lights.direction.color);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("PointLight"))
	{
		constexpr size_t MAX_COUNT = 4U;
		bool canAppend = (lights.points.size() < MAX_COUNT) ? true : false;
		bool canPopBack = (1U <= lights.points.size()) ? true : false;
		if (canAppend && ImGui::Button("Append"))
		{
			PointLight add{};
			add.pos = { 0.0f, 0.0f, 0.0f, 1.0f };
			add.color = { 1.0f, 1.0f, 1.0f, 1.0f };
			lights.points.push_back(add);
			setPointLight(add);
		}
		if (canPopBack && ImGui::Button("PopBack"))
		{
			lights.points.pop_back();
			removePointLight(static_cast<int>(lights.points.size()));
		}

		const size_t COUNT = lights.points.size();
		for (size_t i = 0; i < COUNT; ++i)
		{
			auto& point = lights.points[i];
			ImGui::DragFloat3(std::string("Position" + std::to_string(i)).c_str(), &point.pos.x); point.pos.w = 1.0f;
			ImGui::ColorEdit4(std::string("Color" + std::to_string(i)).c_str(), &point.color.x);
			ImGui::SliderFloat4(std::string("Attenuate" + std::to_string(i)).c_str(), &point.attenuate.x, 0.0f, 1.0f);

			setPointlightInfo(static_cast<int>(i), point);
		}

		ImGui::TreePop();
	}
	ImGui::End();

	ImGui::Begin("GolemAI");
	bossAI.ImGui( NULL );
	ImGui::End();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("Camera.Pos", &cameraPos.x);
	ImGui::DragFloat3("Camera.FocusOffset", &cameraFocusOffset.x);
	ImGui::End();

	float _pos[3];
	_pos[0] = obb1.pos.x;
	_pos[1] = obb1.pos.y;
	_pos[2] = obb1.pos.z;
	float _scale[3];
	_scale[0] = obb1.scale.x;
	_scale[1] = obb1.scale.y;
	_scale[2] = obb1.scale.z;

	float _pos2[3];
	_pos2[0] = obb2.pos.x;
	_pos2[1] = obb2.pos.y;
	_pos2[2] = obb2.pos.z;
	float _scale2[3];
	_scale2[0] = obb2.scale.x;
	_scale2[1] = obb2.scale.y;
	_scale2[2] = obb2.scale.z;

	ImGui::Begin("cube");
	if (ImGui::TreeNode(u8"1"))
	{
		ImGui::Checkbox("exist", &exist1);
		ImGui::DragFloat3("pos",   _pos);
		ImGui::DragFloat3("scale", _scale);
		ImGui::SliderFloat3("orientation", angle1, 0.0f, 360.0f * 0.01745f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"2"))
	{
		ImGui::Checkbox("exist ", &exist2);
		ImGui::DragFloat3("pos ", _pos2);
		ImGui::DragFloat3("scale ", _scale2);
		ImGui::SliderFloat3("orientation ", angle2, 0.0f, 360.0f * 0.01745f);
		ImGui::TreePop();
	}
	ImGui::End();

	obb1.pos.x = _pos[0];
	obb1.pos.y = _pos[1];
	obb1.pos.z = _pos[2];
	obb1.scale.x = _scale[0];
	obb1.scale.y = _scale[1];
	obb1.scale.z = _scale[2];

	obb2.pos.x = _pos2[0];
	obb2.pos.y = _pos2[1];
	obb2.pos.z = _pos2[2];
	obb2.scale.x = _scale2[0];
	obb2.scale.y = _scale2[1];
	obb2.scale.z = _scale2[2];

	obb1.orientation = Donya::Quaternion::Make(angle1[2], angle1[1], angle1[0]);
	obb2.orientation = Donya::Quaternion::Make(angle2[2], angle2[1], angle2[0]);

	/*obb1.angle.x = angle1[0];
	obb1.angle.y = angle1[1];
	obb1.angle.z = angle1[2];
	obb2.angle.x = angle2[0];
	obb2.angle.y = angle2[1];
	obb2.angle.z = angle2[2];*/
}