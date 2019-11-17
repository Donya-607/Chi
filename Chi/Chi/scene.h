#ifndef SCENE_H_
#define SCENE_H_

#include <Windows.h>	// Insert by Donya, must include this before "Xinput.h".
#include <Xinput.h>

#include "baseScene.h"

#include "rayPick.h"

#include "Sprite.h"
#include "Particle.h"
#include "GolemAI.h"
#include "OBB.h"
#include "Effect.h"
#include "Stage.h"
#include "light.h"
#include "Donya/ChiUtility.h"

#include "Mouse.h"

using namespace DirectX;
class sceneTitle : public baseScene
{
private:
	struct model
	{
		skinned_mesh mesh;
		XMFLOAT3  model_pos;
		XMFLOAT3  model_scale;
		XMFLOAT3  model_angle;
		bool  is_enable;
		float stop_timer;
		std::vector<ImTextureID> tex_SRV;
		DirectX::XMFLOAT4 judge_color;
	};

	float camPos[3] = { 0,6,-10 };
	bool flg;

	std::unique_ptr<DirectX::Mouse> mouse;
	DirectX::XMFLOAT3 pos, target, angle;
	DirectX::XMFLOAT3 setPos, setAngle;
	DirectX::XMFLOAT3 zoomVec;
	DirectX::XMFLOAT3 rightVec, upVec;
	DirectX::XMFLOAT2 power;
	float zoomSpeed, moveSpeed;
	float cam_angle;
	DirectX::XMINT2 mousePos, prevMPos, moveVol;
	bool mouse_right;
	fbx_shader shader;
	std::vector<model> models;

	static_mesh builborad;
	DirectX::XMFLOAT4 builboard_pos;
	float builborad_angle;
	DirectX::XMFLOAT2 builborad_size;
	DirectX::XMFLOAT2 texpos;
	DirectX::XMFLOAT2 texsize;
	float alpha;
	DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT4 billbord_judge;
	//bloom test
	ID3D11ShaderResourceView* SRV;
	ID3D11RenderTargetView* RT;
	ImTextureID screen_SRV;
	ImTextureID bloom_SRV;
	ImTextureID z_SRV;
	ImTextureID filter_SRV;
	float blur;
	DirectX::XMFLOAT4 judged_color;
	bool enable_bloom;

	DirectX::XMFLOAT4 status;
	Sprite test;
public:
	sceneTitle(){}
	~sceneTitle()
	{
		uninit();
	}
	void init();
	void update();
	void render();
	void uninit();
	void imGui();
};

#include <memory>
/// <summary>
/// By Donya.
/// </summary>
class SceneGame : public baseScene
{
public:
	struct Impl;
private:
	std::unique_ptr<Impl> pImpl;

public:
	SceneGame();
	~SceneGame();
public:
	void init()   override;
	void update() override;
	void render() override;
	void uninit() override;
	void imGui()  override;
};


class scenePose : public baseScene
{
private:

public:
	scenePose() {}
	~scenePose()
	{
		uninit();
	}
	void init();
	void update();
	void render();
	void uninit();
	void imGui();

};

class SceneEffect : public baseScene
{
private:
	FlashParticle flashParticle;
	BubbleParticle bubbleParticle;
	GolemAI bossAI;
	int keyCnt;
	int keyCnt2;

	std::shared_ptr<static_mesh> pCube1;
	std::shared_ptr<static_mesh> pCube2;
	::OBB obb1;
	::OBB obb2;
	DirectX::XMFLOAT4 color1;
	DirectX::XMFLOAT4 color2;
	float angle1[3];
	float angle2[3];
	bool exist1;
	bool exist2;

//	EruptionEffect eruptionEffect;
	Stage stage;

	Donya::Vector3 cameraPos;
	Donya::Vector3 cameraFocusOffset;
	Lights	lights;

	Donya::Vector3 originPos;

	fbx_shader shader;

public:
	SceneEffect() : stage(), cameraPos(), cameraFocusOffset(), lights(), shader(){}
	~SceneEffect()
	{
		uninit();
	}
	void init();
	void update();
	void render();
	void uninit();
	void imGui();
};

#endif // !SCENE_H_
