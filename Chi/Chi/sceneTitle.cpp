#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "keyInput.h"

#include "Donya/Easing.h"
#include "Donya/FilePath.h"
#include "Donya/Keyboard.h"	// Insert by Donya.
#include "Donya/Useful.h"	// Insert by Donya.

#define scast static_cast


void sceneTitle::init()
{
	setCamPos(camPos);
	setTarget(camTarget);
	cameraDistance = 1000.0f;

	isStack = false;
	setLightAmbient({ .1f,-1,0,1 }, { 1,1,1,1 });
	loadShader(shader, "./Data/shader/skinned_mesh_has_born_vs.cso", "./Data/shader/skinned_mesh_ps.cso", "./Data/shader/skinned_mesh_vs.cso", "./Data/shader/skinned_mesh_no_uv_ps.cso");

	lights.direction.color = Donya::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	getLineLight().setLineLight(lights.direction.direction, lights.direction.color);

	resetPointLight();
	for (const auto i : lights.points)
	{
		setPointLight(i);
	}

	loadFBX(pStageModel.get(), GetModelPath(ModelAttribute::TutorialStage));
	loadFBX(pTitleModel.get(), "./Data/model/title01.fbx");

	std::vector<Donya::Box> wallHitBox_vector;
	Donya::Box wallHitBox;

	// left
	wallHitBox.cx = 650.0f;
	wallHitBox.cy = 0.0f;
	wallHitBox.w = 10.0f;
	wallHitBox.h = 16000.0f;
	wallHitBox_vector.push_back(wallHitBox);

	// right
	wallHitBox.cx = -650.0f;
	wallHitBox.cy = 0.0f;
	wallHitBox.w = 10.0f;
	wallHitBox.h = 16000.0f;
	wallHitBox_vector.push_back(wallHitBox);

	// left block
	wallHitBox.cx = 650.0f;
	wallHitBox.cy = 14000.0f;
	wallHitBox.w = 160.0f;
	wallHitBox.h = 300.0f;
	wallHitBox_vector.push_back(wallHitBox);

	// right block
	wallHitBox.cx = -650.0f;
	wallHitBox.cy = 14000.0f;
	wallHitBox.w = 160.0f;
	wallHitBox.h = 300.0f;
	wallHitBox_vector.push_back(wallHitBox);

	// front
	wallHitBox.cx = 0.0f;
	wallHitBox.cy = 4750.0f;
	wallHitBox.w = 881.0f;
	wallHitBox.h = 1000.0f;
	wallHitBox_vector.push_back(wallHitBox);

	// back
	wallHitBox.cx = 0.0f;
	wallHitBox.cy = 14200.0f;
	wallHitBox.w = 1000.0f;
	wallHitBox.h = 150.0f;
	wallHitBox_vector.push_back(wallHitBox);

	player.Init(Donya::Vector3(0.0f, 2222.5f, 14000.0f), Donya::Vector3(0.0f, 0.0f * 0.01745f, 0.0f), wallHitBox_vector);
	player.SetFieldRadius(9999999.0f);

	catapult.Init(Donya::Vector3(0.0f, 2208.0f, 4981.0f), Donya::Vector3(1.0f, 1.0f, 1.0f), Donya::Vector3(0.0f, 0.0f, 0.0f));

	titlePos = Donya::Vector3(0.0f, 3354.0f, 11531.0f);
	titleScale = Donya::Vector3(250.0f, 250.0f, 250.0f);
	cubePos = Donya::Vector3(0.0f, 2230.0f, 11531.0f);
	cubeScale = Donya::Vector3(100.0f, 250.0f, 100.0f);

	sceneState = 0;
	tutorialState = 0;

	moveCnt = 0;
	titleExist = true;
}

void sceneTitle::update()
{
	switch (sceneState)
	{
	case SceneState::TITLE:
		TitleUpdate();
		break;
	case SceneState::CAMERA_MOVE:
		CameraMove();
		break;
	case SceneState::TUTORIAL:
		TutorialUpdate();
		break;
	}
}

void sceneTitle::TitleUpdate()
{
	setCamPos(camTitlePos);
	setTarget(camTitleTarget);

	if (Donya::Keyboard::Trigger('T'))
	{
		sceneState++;
	}
}

void sceneTitle::CameraMove()
{
	if (MAX_MOVE_CNT <= moveCnt++)
	{
		setCamPos(camPos);
		setTarget(camTarget);

		moveCnt = 0;
		titleExist = false;
		sceneState++;
	}
	else
	{
		using namespace Donya::Easing;

		float currentTime = static_cast<float>( moveCnt ) / static_cast<float>( MAX_MOVE_CNT );
		float ease = Ease( Kind::Exponential, Type::Out, currentTime );

		Donya::Vector3 posDiff		= camPos - camTitlePos;
		Donya::Vector3 targetDiff	= camTarget - camTitleTarget;

		Donya::Vector3 resultPos	= camTitlePos + ( posDiff * ease );
		Donya::Vector3 resultTarget	= camTitleTarget + ( targetDiff * ease );

		setCamPos( resultPos );
		setTarget( resultTarget );

		constexpr float START	= 3354.0f;
		constexpr float LAST	= 4000.0f;
		constexpr float DIFF	= LAST - START;
		titlePos.y = START + ( DIFF * ease );

		/*
		// OLD process.
		float posX = easing::OutExp(moveCnt, MAX_MOVE_CNT, camPos.x, camTitlePos.x);
		float posY = easing::OutExp(moveCnt, MAX_MOVE_CNT, camPos.y, camTitlePos.y);
		float posZ = easing::OutExp(moveCnt, MAX_MOVE_CNT, camPos.z, camTitlePos.z);

		float targetX = easing::OutExp(moveCnt, MAX_MOVE_CNT, camTarget.x, camTitleTarget.x);
		float targetY = easing::OutExp(moveCnt, MAX_MOVE_CNT, camTarget.y, camTitleTarget.y);
		float targetZ = easing::OutExp(moveCnt, MAX_MOVE_CNT, camTarget.z, camTitleTarget.z);

		setCamPos(Donya::Vector3(posX, posY, posZ));
		setTarget(Donya::Vector3(targetX, targetY, targetZ));

		titlePos.y = easing::OutExp(moveCnt, MAX_MOVE_CNT, 4000.0f, 3354.0f);
		*/
	}
}

void sceneTitle::TutorialUpdate()
{
	switch (tutorialState)
	{
	case sceneTitle::START:
		TutorialStartUpdate();
		break;
	case sceneTitle::GUARD:
		TutorialGuardUpdate();
		break;
	case sceneTitle::ATTACK:
		TutorialAttackUpdate();
		break;
	case sceneTitle::END:
		TutorialEndUpdate();
		break;
	}
}

void sceneTitle::TutorialStartUpdate()
{
	setCamPos(camPos);
	setTarget(camTarget);

	auto MakePlayerInput = [](Donya::Vector4x4 viewMat)->Player::Input
	{
		Player::Input input{};

#if !DEBUG_MODE
		// TODO : コントローラーがあるか判定する
		constexpr bool IS_CONTROLLER_CONNECTED = true;
		if (IS_CONTROLLER_CONNECTED)
#endif // !DEBUG_MODE
		{
			// XINPUT_GAMEPAD : https://docs.microsoft.com/ja-jp/windows/win32/api/xinput/ns-xinput-xinput_gamepad

			constexpr int   PAD_NO = 0;
			constexpr float STICK_RANGE_MAX = 32768.0f;
			const auto leftStick = GameLib::input::xInput::getThumbL(PAD_NO);
			if (leftStick.x != 0) { input.moveVector.x = scast<float>(leftStick.x) / STICK_RANGE_MAX; }
			if (leftStick.y != 0) { input.moveVector.z = scast<float>(leftStick.y) / STICK_RANGE_MAX; }

			constexpr int TRIGGER_FLAG = 1;
			if (GameLib::input::xInput::pressedButtons(PAD_NO, XboxPad_Button::RIGHT_SHOULDER) == TRIGGER_FLAG) { input.doDefend = true; }
			if (GameLib::input::xInput::pressedButtons(PAD_NO, XboxPad_Button::X) == TRIGGER_FLAG) { input.doAttack = true; }
		}
#if !DEBUG_MODE
		// else
#endif // !DEBUG_MODE
		{
			if (Donya::Keyboard::Press(VK_UP)) { input.moveVector.z = +1.0f; }
			if (Donya::Keyboard::Press(VK_DOWN)) { input.moveVector.z = -1.0f; }
			if (Donya::Keyboard::Press(VK_LEFT)) { input.moveVector.x = -1.0f; }
			if (Donya::Keyboard::Press(VK_RIGHT)) { input.moveVector.x = +1.0f; }

			if (Donya::Keyboard::Trigger('Z')) { input.doDefend = true; }
			if (Donya::Keyboard::Trigger('X')) { input.doAttack = true; }
		}

		input.moveVector.Normalize();

		// Transform to camera space from world space.
		{
			Donya::Vector4x4 cameraRotation{};
			{
				// Extract inverse rotation matrix here.
				cameraRotation = viewMat;
				cameraRotation._14 = 0.0f;
				cameraRotation._24 = 0.0f;
				cameraRotation._34 = 0.0f;
				cameraRotation._41 = 0.0f;
				cameraRotation._42 = 0.0f;
				cameraRotation._43 = 0.0f;
				cameraRotation._44 = 1.0f;

				// XXX : If "viewMat" is invalid matrix, Inverse() will returns NaN.

				cameraRotation = cameraRotation.Inverse();
			}

			Donya::Vector4 moveVector4{};
			moveVector4.x = input.moveVector.x;
			moveVector4.y = input.moveVector.y;
			moveVector4.z = input.moveVector.z;
			moveVector4.w = 0.0f;

			moveVector4 = cameraRotation * moveVector4;

			input.moveVector.x = moveVector4.x;
			input.moveVector.z = moveVector4.z;
		}

		return input;
	};
	player.Update(MakePlayerInput(Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix())));

	std::vector<Donya::Circle> nullBodies;
	player.PhysicUpdate(nullBodies);

	if (11431.0f - 100.0f / 2.0f <= player.GetPosition().z && player.GetPosition().z <= 11431.0f + 100.0f / 2.0f
		&& -100.0f / 2.0f <= player.GetPosition().x && player.GetPosition().x <= 100.0f / 2.0f)
	{
		catapult.StartAnim();
		tutorialState++;
	}

	CameraUpdate(camTarget);
}

void sceneTitle::TutorialGuardUpdate()
{
	setCamPos(camPos);
	setTarget(camTarget);

	auto MakePlayerInput = [](Donya::Vector4x4 viewMat)->Player::Input
	{
		Player::Input input{};

#if !DEBUG_MODE
		// TODO : コントローラーがあるか判定する
		constexpr bool IS_CONTROLLER_CONNECTED = true;
		if (IS_CONTROLLER_CONNECTED)
#endif // !DEBUG_MODE
		{
			// XINPUT_GAMEPAD : https://docs.microsoft.com/ja-jp/windows/win32/api/xinput/ns-xinput-xinput_gamepad

			constexpr int   PAD_NO = 0;
			constexpr int TRIGGER_FLAG = 1;
			if (GameLib::input::xInput::pressedButtons(PAD_NO, XboxPad_Button::RIGHT_SHOULDER) == TRIGGER_FLAG) { input.doDefend = true; }
		}

#if !DEBUG_MODE
		// else
#endif // !DEBUG_MODE
		{
			if (Donya::Keyboard::Trigger('Z')) { input.doDefend = true; }
		}

		return input;
	};
	player.Update(MakePlayerInput(Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix())));

	std::vector<Donya::Circle> nullBodies;
	player.PhysicUpdate(nullBodies);

	ProcessCollision();

	catapult.Update(player.GetPosition());
}

void sceneTitle::TutorialAttackUpdate()
{
	setCamPos(camPos);
	setTarget(camTarget);

	auto MakePlayerInput = [](Donya::Vector4x4 viewMat)->Player::Input
	{
		Player::Input input{};

#if !DEBUG_MODE
		// TODO : コントローラーがあるか判定する
		constexpr bool IS_CONTROLLER_CONNECTED = true;
		if (IS_CONTROLLER_CONNECTED)
#endif // !DEBUG_MODE
		{
			// XINPUT_GAMEPAD : https://docs.microsoft.com/ja-jp/windows/win32/api/xinput/ns-xinput-xinput_gamepad

			constexpr int   PAD_NO = 0;
			constexpr float STICK_RANGE_MAX = 32768.0f;
			const auto leftStick = GameLib::input::xInput::getThumbL(PAD_NO);
			if (leftStick.x != 0) { input.moveVector.x = scast<float>(leftStick.x) / STICK_RANGE_MAX; }
			if (leftStick.y != 0) { input.moveVector.z = scast<float>(leftStick.y) / STICK_RANGE_MAX; }

			constexpr int TRIGGER_FLAG = 1;
			if (GameLib::input::xInput::pressedButtons(PAD_NO, XboxPad_Button::RIGHT_SHOULDER) == TRIGGER_FLAG) { input.doDefend = true; }
			if (GameLib::input::xInput::pressedButtons(PAD_NO, XboxPad_Button::X) == TRIGGER_FLAG) { input.doAttack = true; }
		}
#if !DEBUG_MODE
		// else
#endif // !DEBUG_MODE
		{
			if (Donya::Keyboard::Press(VK_UP)) { input.moveVector.z = +1.0f; }
			if (Donya::Keyboard::Press(VK_DOWN)) { input.moveVector.z = -1.0f; }
			if (Donya::Keyboard::Press(VK_LEFT)) { input.moveVector.x = -1.0f; }
			if (Donya::Keyboard::Press(VK_RIGHT)) { input.moveVector.x = +1.0f; }

			if (Donya::Keyboard::Trigger('Z')) { input.doDefend = true; }
			if (Donya::Keyboard::Trigger('X')) { input.doAttack = true; }
		}

		input.moveVector.Normalize();

		// Transform to camera space from world space.
		{
			Donya::Vector4x4 cameraRotation{};
			{
				// Extract inverse rotation matrix here.
				cameraRotation = viewMat;
				cameraRotation._14 = 0.0f;
				cameraRotation._24 = 0.0f;
				cameraRotation._34 = 0.0f;
				cameraRotation._41 = 0.0f;
				cameraRotation._42 = 0.0f;
				cameraRotation._43 = 0.0f;
				cameraRotation._44 = 1.0f;

				// XXX : If "viewMat" is invalid matrix, Inverse() will returns NaN.

				cameraRotation = cameraRotation.Inverse();
			}

			Donya::Vector4 moveVector4{};
			moveVector4.x = input.moveVector.x;
			moveVector4.y = input.moveVector.y;
			moveVector4.z = input.moveVector.z;
			moveVector4.w = 0.0f;

			moveVector4 = cameraRotation * moveVector4;

			input.moveVector.x = moveVector4.x;
			input.moveVector.z = moveVector4.z;
		}

		return input;
	};
	player.Update(MakePlayerInput(Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix())));

	std::vector<Donya::Circle> nullBodies;
	player.PhysicUpdate(nullBodies);

	ProcessCollision();

	CameraUpdate(camTarget);
}

void sceneTitle::TutorialEndUpdate()
{
	pSceneManager->setNextScene(new SceneGame, false);
}

void sceneTitle::CameraUpdate(const Donya::Vector3& targetPosition)
{
#if 0
	DirectX::XMFLOAT3 originVec(0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 boss_Player_Vec(player.GetPosition().x - boss.pos.x, player.GetPosition().y - boss.pos.y, player.GetPosition().z - boss.pos.z);
	DirectX::XMFLOAT3 _boss_Player_Vec(player.GetPosition().x - boss.pos.x, player.GetPosition().y - boss.pos.y, player.GetPosition().z - boss.pos.z);

	float l1, l2;
	l1 = sqrtf((originVec.x * originVec.x) + (originVec.y * originVec.y) + (originVec.z * originVec.z));
	l2 = sqrtf((boss_Player_Vec.x * boss_Player_Vec.x) + 0.0f + (boss_Player_Vec.z * boss_Player_Vec.z));
	boss_Player_Vec.x /= l2;
	boss_Player_Vec.y /= l2;
	boss_Player_Vec.z /= l2;

	float dot = 0.0f;
	float angle = 0.0f;
	l2 = sqrtf((boss_Player_Vec.x * boss_Player_Vec.x) + 0.0f + (boss_Player_Vec.z * boss_Player_Vec.z));
	dot = (originVec.x * boss_Player_Vec.x) + 0.0f + (originVec.z * boss_Player_Vec.z);
	angle = acos(dot / (l1 * l2)) / (3.141592f / 180.0f);

	if (_boss_Player_Vec.x < 0)
	{
		angle *= -1;
	}
	float distance = 1000.0f;
	DirectX::XMFLOAT3 _player_pos = player.GetPosition();
	cameraPos.x = _player_pos.x + sinf(angle * 0.01745f) * distance;
	cameraPos.y = 400.0f;
	cameraPos.z = _player_pos.z + cosf(angle * 0.01745f) * distance;
#elif 0
	Donya::Vector3 boss_Player_Vec(player.GetPosition().x - boss.pos.x, 0.0f, player.GetPosition().z - boss.pos.z);
	Donya::Vector3 boss_Player_Vec_N = boss_Player_Vec;
	boss_Player_Vec_N.Normalize();

	float add_length = 1000.0f;

	cameraPos.x = boss_Player_Vec_N.x * (boss_Player_Vec.Length() + add_length);
	cameraPos.y = player.GetPosition().y + add_length;
	cameraPos.z = boss_Player_Vec_N.z * (boss_Player_Vec.Length() + add_length);
#else
	Donya::Vector3 playerPos = player.GetPosition();
	Donya::Vector3 targetPos = targetPosition;

	// DirectX::XMFLOAT3 player_to_target_vec = DirectX::XMFLOAT3( targetPos.x - playerPos.x, targetPos.y - playerPos.y, targetPos.z - playerPos.z );
	Donya::Vector3 player_to_target_vec = targetPos - playerPos;
	if (player_to_target_vec.IsZero())
	{
		// Prevent NaN when calculate Player's input.
		player_to_target_vec = -Donya::Vector3::Front();
	}
	float distPlayerBoss = player_to_target_vec.Length();
	Donya::Vector3 unitvec_player_to_target = player_to_target_vec.Normalized();

	camPos = Donya::Vector3
	{
		playerPos.x - unitvec_player_to_target.x * cameraDistance,
		camPos.y,
		playerPos.z - unitvec_player_to_target.z * cameraDistance
	};

#if 0
	DirectX::XMFLOAT3 cameraTarget = DirectX::XMFLOAT3(targetPos.x + unitvec_player_to_target.x, targetPos.y + unitvec_player_to_target.y, targetPos.z + unitvec_player_to_target.z);
#else
	// TODO : 後々、変更するかも。
	float tanY = tanf(90.0f);
	float cameraFocusY = tanY * (distPlayerBoss / 8.0f);
	// DirectX::XMFLOAT3 cameraTarget = DirectX::XMFLOAT3( targetPos.x + unitvec_player_to_target.x, targetY, targetPos.z + unitvec_player_to_target.z );
	Donya::Vector3 cameraTarget = Donya::Vector3
	{
		targetPos.x + unitvec_player_to_target.x,
		cameraFocusY,
		targetPos.z + unitvec_player_to_target.z
	};
#endif

#endif
}

void sceneTitle::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);
//	setString({ 0,0 }, L"sceneTitle %d : %f", 1, 20.2f);

	Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(getViewMatrix());
	Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(getProjectionMatrix());

	Donya::Vector4x4 W = Donya::Vector4x4::Identity();
	Donya::Vector4x4 WVP = W * V * P;
	FBXRender(pStageModel.get(), shader, WVP, W);

	if (titleExist)
	{
		Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(titleScale);
		Donya::Vector4x4 R = Donya::Vector4x4::MakeRotationEuler(Donya::Vector3(0.0f, 0.0f, 0.0f));
		Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation(titlePos);
		W = S * R * T;
		WVP = W * V * P;
		FBXRender(pTitleModel.get(), shader, WVP, W);
	}

	player.Draw(shader, V, P);
	catapult.Draw(shader, V, P);

	auto GenerateCube = []()->std::shared_ptr<static_mesh>
	{
		std::shared_ptr<static_mesh> tmpCube = std::make_shared<static_mesh>();
		createCube(tmpCube.get());
		return tmpCube;
	};
	static std::shared_ptr<static_mesh> pCube = GenerateCube();
	
	Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling(cubeScale); // Half size->Whole size.
	Donya::Vector4x4 CR = Donya::Vector4x4::MakeRotationEuler(Donya::Vector3(0.0f, 0.0f, 0.0f));
	Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation(cubePos);
	Donya::Vector4x4 CW = CS * CR * CT;
	Donya::Vector4x4 CWVP = CW * V * P;

	OBJRender(pCube.get(), CWVP, CW, Donya::Vector4(0.0f, 0.8f, 0.3f, 0.6f));

}

void sceneTitle::uninit()
{

}

void sceneTitle::imGui()
{
	ImGui::SetNextWindowSize(ImVec2(500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
	ImGui::Begin("Scene", NULL, ImGuiWindowFlags_MenuBar);

	if (ImGui::Button("Game"))
	{
		pSceneManager->setNextScene(new SceneGame(), false);
	}
	else if (Donya::Keyboard::Press(VK_LCONTROL) && Donya::Keyboard::Trigger('G'))
	{
		pSceneManager->setNextScene(new SceneGame(), false);
	}
	else
	if (ImGui::Button("Effect"))
	{
		pSceneManager->setNextScene(new SceneEffect, false);
	}
	else if (ImGui::Button("Result"))
	{
		pSceneManager->setNextScene(new SceneResult(60), false);
	}
	else if (ImGui::Button("GameOver"))
	{
		pSceneManager->setNextScene(new SceneGameOver, false);
	}

	ImGui::End();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &camPos.x);
	ImGui::DragFloat3("target", &camTarget.x);
	ImGui::DragFloat3("Title camera pos", &camTitlePos.x);
	ImGui::DragFloat3("Title camera target", &camTitleTarget.x);
	ImGui::End();

	ImGui::Begin("OBJ Para");
	ImGui::DragFloat3("Title pos", &titlePos.x);
	ImGui::DragFloat3("Title scale", &titleScale.x);
	ImGui::DragFloat3("cube pos", &cubePos.x);
	ImGui::DragFloat3("cube scale", &cubeScale.x);
	ImGui::End();

	ImGui::Begin("Player");
	Donya::Vector3 _pos = player.GetPosition();
	ImGui::Text("pos.x : %f, pos.y : %f, pos.z : %f", _pos.x, _pos.y, _pos.z);
	ImGui::End();

	catapult.ImGui();

#if 0
	static int index = 0;
	static float dragPower[3] = { 1.0f };

	static int index_tex = 0;
	static float dragPower_tex = { 1.0f };

	//inported info ImGui
	{
		ImGui::SetNextWindowSize(ImVec2(500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500.0f, .0f), ImGuiSetCond_Once);
		ImGui::Begin("model", NULL, ImGuiWindowFlags_MenuBar);

		//billboard ImGui
		if (ImGui::TreeNode("billboard"))
		{
			ImGui::DragFloat("x##position", &builboard_pos.x);
			ImGui::DragFloat("y##position", &builboard_pos.y);
			ImGui::DragFloat("z##position", &builboard_pos.z);
			ImGui::NewLine();

			ImGui::Text("scale");
			ImGui::DragFloat("x##scale", &builborad_size.x);
			ImGui::DragFloat("y##scale", &builborad_size.y);
			ImGui::DragFloat("angle", &builborad_angle);
			ImGui::NewLine();

			ImGui::Text("texpos");
			ImGui::DragFloat("x##texpos", &texpos.x);
			ImGui::DragFloat("y##texpos", &texpos.y);
			ImGui::NewLine();
			ImGui::Text("texsize");
			ImGui::DragFloat("x##texsize", &texsize.x);
			ImGui::DragFloat("y##texsize", &texsize.y);
			ImGui::NewLine();
			ImGui::Text("color");
			ImGui::DragFloat("r##color", &color.x);
			ImGui::DragFloat("g##color", &color.y);
			ImGui::DragFloat("b##color", &color.z);
			ImGui::NewLine();
			ImGui::DragFloat("alpha", &alpha);


			ImGui::TreePop();
		}

		//model ImGui
		if (ImGui::TreeNode("model"))
		{
			if (!models.empty())
			{
				ImGui::Text("model_count : %d", models.size());

				ImGui::DragInt("index", &index, 1, 0, models.size() - 1);
				if (ImGui::Button("+"))
				{
					if (index < static_cast<int>(models.size()) - 1)
						index++;
				}
				ImGui::SameLine();
				if (ImGui::Button("-"))
				{
					if (index)
						index--;
				}

				if (ImGui::TreeNode("info##model"))
				{

					//is_Enable
					if (ImGui::Button("is_enable"))
					{
						models[index].is_enable ^= 1;
					}

					//Position
					ImGui::Text("Position");
					ImGui::DragFloat("x##position", &models[index].model_pos.x, dragPower[0]);
					ImGui::DragFloat("y##position", &models[index].model_pos.y, dragPower[0]);
					ImGui::DragFloat("z##position", &models[index].model_pos.z, dragPower[0]);
					ImGui::NewLine();
					ImGui::DragFloat("drag_speed##position", &dragPower[0], 0.01f, 0);
					ImGui::NewLine();

					//Scale
					ImGui::DragFloat("Scale", &models[index].model_scale.x, dragPower[1]);
					models[index].model_scale.y = models[index].model_scale.x;
					models[index].model_scale.z = models[index].model_scale.x;
					ImGui::NewLine();
					ImGui::DragFloat("drag_speed##scale", &dragPower[1], 0.01f, 0);
					ImGui::NewLine();

					//Angle
					ImGui::Text("Angle");
					ImGui::DragFloat("x##angle", &models[index].model_angle.x, dragPower[2]);
					ImGui::DragFloat("y##angle", &models[index].model_angle.y, dragPower[2]);
					ImGui::DragFloat("z##angle", &models[index].model_angle.z, dragPower[2]);
					ImGui::NewLine();
					ImGui::DragFloat("drag_speed##angle", &dragPower[2], 0.01f, 0);
					ImGui::NewLine();

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("material"))
				{
					const int SIZE = static_cast<int>(models[index].tex_SRV.size());
					for (int i = 0; i < SIZE; i++)
					{
						ImGui::Image(models[index].tex_SRV[i], { 512,512 });
						ImGui::Text("image_%d", i);
						ImGui::NewLine();
					}
					ImGui::TreePop();
				}
			}
			else
				ImGui::Text("model none");

			ImGui::TreePop();
		}


		ImGui::End();
	}

	{}
	//bloom info ImGui
	{
		ImGui::SetNextWindowSize(ImVec2(500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
		ImGui::Begin("bloom", NULL, ImGuiWindowFlags_MenuBar);

		if (ImGui::TreeNode("parameter"))
		{
			ImGui::DragFloat("blur", &blur, 0.1f, 0, 10000.0f);
			ImGui::NewLine();

			ImGui::Text("judged_color");
			ImGui::DragFloat("r##bloom", &judged_color.x, 0.01f, 0, 1.0f);
			ImGui::DragFloat("g##bloom", &judged_color.y, 0.01f, 0, 1.0f);
			ImGui::DragFloat("b##bloom", &judged_color.z, 0.01f, 0, 1.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("screen"))
		{
			ImGui::Image(screen_SRV, { 640,360 });
			ImGui::TreePop();
		}

		ImGui::End();
	}


	//camera_info ImGui
	{
		ImGui::SetNextWindowSize(ImVec2(500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
		ImGui::Begin("camera", NULL, ImGuiWindowFlags_MenuBar);

		ImGui::Text("position");
		ImGui::DragFloat("x##camPosision", &pos.x);
		ImGui::DragFloat("y##camPosision", &pos.y);
		ImGui::DragFloat("z##camPosision", &pos.z);
		ImGui::NewLine();

		ImGui::Text("target");
		ImGui::DragFloat("x##camTarget", &target.x);
		ImGui::DragFloat("y##camTarget", &target.y);
		ImGui::DragFloat("z##camTarget", &target.z);
		ImGui::NewLine();

		ImGui::DragFloat("zoom_speed", &zoomSpeed, 0.01f, 0, 100.0f);
		setCamPos(pos);
		setTarget(target);

		ImGui::End();
	}
#endif
}

void sceneTitle::ProcessCollision()
{
	const Donya::OBB playerBodyBox = player.GetHurtBox();
	const Donya::OBB playerShieldBox = player.GetShieldHitBox();
	const Donya::OBB playerAttackBox = player.CalcAttackHitBox();

	bool wasHitToShield = false;

	if (Donya::OBB::IsHitSphere(playerShieldBox, catapult.stone.hitSphere))
	{
		setStopAnimation(catapult.pModel.get(), true);
		catapult.stone.exist = false;
		catapult.stone.hitSphere.exist = false;
		catapult.stone.hitSphere.enable = false;
		wasHitToShield = true;
		player.SucceededDefence();
		tutorialState++;
	}
	if (!wasHitToShield && Donya::OBB::IsHitSphere(playerBodyBox, catapult.stone.hitSphere))
	{
		player.ReceiveImpact();
		pSceneManager->setNextScene(new sceneTitle(), false);
	}

	// PlayerAttack VS CatapuletBudy
	{
		if (tutorialState == TutorialState::ATTACK && Donya::OBB::IsHitOBB(playerAttackBox, catapult.hitOBB))
		{
			catapult.hitOBB.exist = false;
			tutorialState++;
		}
	}

	wasHitToShield = false;
}
