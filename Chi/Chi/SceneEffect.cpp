#include "scene.h"
#include "sceneManager.h"
#include "Donya/Vector.h"


void SceneEffect::init()  
{
	isStack = false;
	keyCnt = 0;

	flashParticle.Set();
	flashParticle.ImGuiDataInit();

	bubbleParticle.Set();
	bubbleParticle.ImGuiDataInit();

	bossAI.Init();

	auto GenerateCube = []()->std::shared_ptr<static_mesh>
	{
		std::shared_ptr<static_mesh> tmpCube = std::make_shared<static_mesh>();
		createCube(tmpCube.get());
		return tmpCube;
	};

	pCube1 = GenerateCube();
	pCube2 = GenerateCube();

	obb1.pos = Donya::Vector3(5.0f, 0.0f, 0.0f);
	obb2.pos = Donya::Vector3(-5.0f, 0.0f, 0.0f);
	obb1.scale = Donya::Vector3(25.0f, 20.0f, 15.0f);
	obb2.scale = Donya::Vector3(25.0f, 20.0f, 15.0f);
	obb1.orientation.Identity();
	obb2.orientation.Identity();

	color1 = DirectX::XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	color2 = DirectX::XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);

	angle1[0] = 0.0f;
	angle1[1] = 0.0f;
	angle1[2] = 0.0f;
	angle2[0] = 0.0f;
	angle2[1] = 0.0f;
	angle2[2] = 0.0f;
}
void SceneEffect::update()
{
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

	flashParticle.Emit();
	bubbleParticle.Emit();
	bossAI.Update();

	if (obb1.JudgeOBB(&obb2))
	{
		color1.x = 1.0f;
		color2.y = 1.0f;
	}
	else
	{
		color1.x = 0.7f;
		color2.y = 0.7f;
	}
}
void SceneEffect::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);

	Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix());
	Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(GameLib::camera::GetProjectionMatrix());

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(obb1.scale);
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation(obb1.pos);
	Donya::Vector4x4 R = obb1.orientation.RequireRotationMatrix();
	Donya::Vector4x4 W = S * R * T;
	Donya::Vector4x4 WVP = W * V * P;

	OBJRender(pCube1.get(), WVP, W, color1);

	S = Donya::Vector4x4::MakeScaling(obb2.scale);
	T = Donya::Vector4x4::MakeTranslation(obb2.pos);
	R = obb2.orientation.RequireRotationMatrix();
	W = S * R * T;
	WVP = W * V * P;

	OBJRender(pCube2.get(), WVP, W, color2);
}
void SceneEffect::uninit()
{

}
void SceneEffect::imGui()
{
	ImGui::Begin("Particle");
	flashParticle.ImGui();
	bubbleParticle.ImGui();
	ImGui::End();

	flashParticle.UseImGui();

	ImGui::Begin("GolemAI");
	bossAI.ImGui();
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
		ImGui::DragFloat3("pos",   _pos);
		ImGui::DragFloat3("scale", _scale);
		ImGui::SliderFloat3("orientation", angle1, 0.0f, 360.0f * 0.01745f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"2"))
	{
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