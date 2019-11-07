#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "keyInput.h"

#include "Donya/Keyboard.h" // Insert by Donya.

void sceneTitle::init()
{
	isStack = false;
	flg = false;
	pos = { getCamPos().x,getCamPos().y,getCamPos().z };
	target = { getCamTarget().x,getCamTarget().y,getCamTarget().z };
	setLightAmbient({ .1f,-1,0,1 }, { 1,1,1,1 });
	zoomSpeed = 2.0f;
	moveSpeed = 10.0f;
	mouse = std::make_unique<Mouse>();
	mousePos = { mouse->GetState().x,mouse->GetState().y };
	prevMPos = mousePos;
	moveVol = { 0,0 };
	mouse_right = false;
	prevMPos = mousePos;
	cam_angle = 90;
	models.clear();
	loadShader(shader, "./Data/shader/skinned_mesh_has_born_vs.cso", "./Data/shader/skinned_mesh_ps.cso", "./Data/shader/skinned_mesh_vs.cso", "./Data/shader/skinned_mesh_no_uv_ps.cso");

	createBillboard(&builborad, L"./Data/SpeedRing.png");
	builboard_pos = { 0,0,0,1 };
	builborad_angle = 0;
	builborad_size = 1.0f;
	texpos = { 0,0 };
	texsize = { 64,64 };
}

void sceneTitle::update()
{
	//model loading
	while (GameLib::getLoadFlg())
	{
		if (GameLib::getLoadedFileCount() <= 0)
			break;
		std::wstring sample = GameLib::getLoadedFileName();
		std::wstring name;
		size_t start = sample.find_last_of(L"\\") + 1;
		name = sample.substr(start, sample.size() - start);

		std::string dummy(name.begin(), name.end());
		std::string file_name = "./Data/" + dummy;
		start = dummy.find_last_of(".") + 1;
		std::string extend = dummy.substr(start, dummy.size() - start);
		models.emplace_back();
		loadFBX(&models.back().mesh, file_name);
		models.back().model_pos = { 0,0,0 };
		models.back().model_angle = { 0,0,0 };
		models.back().model_scale = { 0.1f,0.1f,0.1f };
		models.back().is_enable = true;
		for (int i = 0; i < models.back().mesh.getMeshCount(); i++)
		{
			for (auto& p : models.back().mesh.getMesh(i).subsets)
			{
				models.back().tex_SRV.emplace_back();
				models.back().tex_SRV.back() = (void*)p.diffuse.shader_resource_view;
			}
		}
	}

	//camera moving
	{
		mousePos = { mouse->GetState().x,mouse->GetState().y };
		moveVol = { prevMPos.x - mousePos.x,prevMPos.y - mousePos.y };

		pos = { getCamPos().x,getCamPos().y,getCamPos().z };
		target = { getCamTarget().x,getCamTarget().y,getCamTarget().z };

		//ズーム用のベクトル(単位ベクトル)
		zoomVec = { target.x - pos.x,target.y - pos.y,target.z - pos.z };
		float scalar = sqrtf(zoomVec.x * zoomVec.x + zoomVec.y * zoomVec.y + zoomVec.z * zoomVec.z);
		zoomVec = { zoomVec.x / scalar,zoomVec.y / scalar,zoomVec.z / scalar };

		if (mouse->GetState().scrollWheelValue > 0)
		{
			pos.x += zoomVec.x * zoomSpeed;
			pos.y += zoomVec.y * zoomSpeed;
			pos.z += zoomVec.z * zoomSpeed;
		}
		if (mouse->GetState().scrollWheelValue < 0)
		{
			pos.x -= zoomVec.x * zoomSpeed;
			pos.y -= zoomVec.y * zoomSpeed;
			pos.z -= zoomVec.z * zoomSpeed;
		}
		//マウスホイール値のリセット
		mouse->ResetScrollWheelValue();
		float radius = sqrtf(zoomVec.x * zoomVec.x * scalar * scalar + zoomVec.z * zoomVec.z * scalar * scalar);
		target.x = pos.x + cosf(DirectX::XMConvertToRadians(cam_angle)) * radius;
		target.z = pos.z + sinf(DirectX::XMConvertToRadians(cam_angle)) * radius;

		//カメラ回転
		if (mouse->GetState().leftButton && getKeyState(KEY_INPUT_LMENU))
		{
			if (moveVol.x > 0)
			{
				cam_angle += 0.025f * moveSpeed;
			}
			else if (moveVol.x < 0)
			{
				cam_angle -= 0.025f * moveSpeed;

			}
			if (moveVol.y > 0)
			{
				target.y += moveSpeed;
			}
			else if (moveVol.y < 0)
			{
				target.y -= moveSpeed;
			}

		}

		//カメラ移動
		if (mouse->GetState().middleButton && getKeyState(KEY_INPUT_LMENU))
		{
			rightVec = { zoomVec.z,0,-zoomVec.x };
			upVec = { zoomVec.y * rightVec.z - zoomVec.z * rightVec.y, zoomVec.z * rightVec.x - zoomVec.x * rightVec.z, zoomVec.x * rightVec.y - zoomVec.y * rightVec.x };

			if (moveVol.x < 0)
			{
				pos.z -= rightVec.z * moveSpeed;
				pos.x -= rightVec.x * moveSpeed;
				target.z -= rightVec.z * moveSpeed;
				target.x -= rightVec.x * moveSpeed;
			}
			else if (moveVol.x > 0)
			{
				pos.z += rightVec.z * moveSpeed;
				pos.x += rightVec.x * moveSpeed;
				target.z += rightVec.z * moveSpeed;
				target.x += rightVec.x * moveSpeed;
			}
			if (moveVol.y > 0)
			{
				pos.x -= upVec.x * moveSpeed;
				pos.y -= upVec.y * moveSpeed;
				pos.z -= upVec.z * moveSpeed;
				target.x -= upVec.x * moveSpeed;
				target.y -= upVec.y * moveSpeed;
				target.z -= upVec.z * moveSpeed;
			}
			else if (moveVol.y < 0)
			{
				pos.x += upVec.x * moveSpeed;
				pos.y += upVec.y * moveSpeed;
				pos.z += upVec.z * moveSpeed;
				target.x += upVec.x * moveSpeed;
				target.y += upVec.y * moveSpeed;
				target.z += upVec.z * moveSpeed;
			}

		}
		setCamPos(pos);
		setTarget(target);
		prevMPos = mousePos;
		mouse_right = mouse->GetState().rightButton;

	}

	if (GetAsyncKeyState('1') < 0)
		pSceneManager->setNextScene(new scenePose, true);
}

void sceneTitle::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);
	setString({ 0,0 }, L"sceneTitle %d : %f", 1, 20.2f);
	//textOut(L"TITLE", .0f, 0.f);

	if (!models.empty())
		for (model* p = models.data(); p < models.data() + models.size(); p++)
		{
			if (!p->is_enable)
				continue;
			XMFLOAT4X4 W, WVP;

			DirectX::XMMATRIX worldM;
			DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;
			T = DirectX::XMMatrixTranslation(p->model_pos.x, p->model_pos.y, p->model_pos.z);

			//	拡大・縮小
			S = DirectX::XMMatrixScaling(p->model_scale.x, p->model_scale.y, p->model_scale.z);

			//	回転
			Rx = DirectX::XMMatrixRotationX(p->model_angle.x);				//	X軸を基準とした回転行列
			Ry = DirectX::XMMatrixRotationY(p->model_angle.y);				//	Y軸を基準とした回転行列
			Rz = DirectX::XMMatrixRotationZ(p->model_angle.z);				//	Z軸を基準とした回転行列
			R = Rz * Rx * Ry;

			//	平行移動

			//	ワールド変換行列
			worldM = S * R * T;

			//	Matrix -> Float4x4 変換
			DirectX::XMStoreFloat4x4(&WVP, worldM * getViewMatrix() * getProjectionMatrix());
			DirectX::XMStoreFloat4x4(&W, worldM);


			FBXRender(&p->mesh, shader, WVP, W);
		}

	DirectX::XMFLOAT4X4 view_projection;
	DirectX::XMStoreFloat4x4(&view_projection, getViewMatrix() * getProjectionMatrix());
	billboardRender(&builborad, view_projection, builboard_pos, builborad_size, builborad_angle, getCamPos(), texpos, texsize);
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

	ImGui::End();
	static int index = 0;
	static float dragPower[3] = { 1.0f };

	static int index_tex = 0;
	static float dragPower_tex = { 1.0f };

	//billboard ImGui
	if (ImGui::TreeNode("billboard"))
	{
		ImGui::DragFloat("x##position", &builboard_pos.x);
		ImGui::DragFloat("y##position", &builboard_pos.y);
		ImGui::DragFloat("z##position", &builboard_pos.z);
		ImGui::NewLine();

		ImGui::DragFloat("scale", &builborad_size);
		ImGui::DragFloat("angle", &builborad_angle);
		ImGui::NewLine();

		ImGui::Text("texpos");
		ImGui::DragFloat("x##texpos", &texpos.x);
		ImGui::DragFloat("y##texpos", &texpos.y);
		ImGui::NewLine();
		ImGui::Text("texsize");
		ImGui::DragFloat("x##texsize", &texsize.x);
		ImGui::DragFloat("y##texsize", &texsize.y);
		ImGui::TreePop();
	}
	//model ImGui
	{
		ImGui::SetNextWindowSize(ImVec2(500.0f, getWindowSize().y / 2.0f), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(getWindowSize().x - 500.0f, .0f), ImGuiSetCond_Once);
		ImGui::Begin("model", NULL, ImGuiWindowFlags_MenuBar);


		if (ImGui::TreeNode("model"))
		{
			if (!models.empty())
			{
				ImGui::Text("model_count : %d", models.size());

				ImGui::DragInt("index", &index, 1, 0, models.size() - 1);
				if (ImGui::Button("+"))
				{
					if (index < static_cast<int>( models.size() ) - 1)
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
					const int SIZE = static_cast<int>( models[index].tex_SRV.size() );
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

}
