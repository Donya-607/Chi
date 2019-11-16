#ifndef SCENE_H_
#define SCENE_H_

#include <Windows.h>	// Insert by Donya, must include this before "Xinput.h".
#include <Xinput.h>

#include "baseScene.h"

#include "rayPick.h"

#include "Sprite.h"
#include "Particle.h"
#include "Player.h"
#include "Catapult.h"
#include "GolemAI.h"
#include "OBB.h"
#include "Effect.h"
#include "Stage.h"
#include "light.h"
#include "Donya/ChiUtility.h"
#include "Donya/Serializer.h"

#include "Mouse.h"

using namespace DirectX;
class sceneTitle : public baseScene
{
private:
	Donya::Vector3 camPos = { 0.0f, 2450.0f, 15000.0f };
	Donya::Vector3 camTarget = { 0.0f, 2000.0f, 0.0f };
	float cameraDistance;

	Donya::Vector3 camTitlePos = { 0.0f, 2226.0f, 14446.0f };
	Donya::Vector3 camTitleTarget = { 0.0f, 5294.0f, 0.0f };
	fbx_shader shader;
	Lights	lights;

	Player	player;
	Catapult catapult;
	Donya::Vector3 titlePos;
	Donya::Vector3 titleScale;
	Donya::Vector3 cubePos;
	Donya::Vector3 cubeScale;
	std::unique_ptr<skinned_mesh> pStageModel;
	std::unique_ptr<skinned_mesh> pTitleModel;

	enum SceneState
	{
		TITLE,
		CAMERA_MOVE,
		TUTORIAL
	};
	int sceneState;

	enum TutorialState
	{
		START,
		GUARD,
		ATTACK,
		END
	};
	int tutorialState;


	static const int MAX_MOVE_CNT = 120;
	int moveCnt;

	bool titleExist;


#if 0
	struct model
	{
		skinned_mesh mesh;
		XMFLOAT3  model_pos;
		XMFLOAT3  model_scale;
		XMFLOAT3  model_angle;
		bool  is_enable;
		std::vector<ImTextureID> tex_SRV;
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

	//bloom test
	ID3D11ShaderResourceView* SRV;
	ID3D11RenderTargetView* RT;
	ImTextureID screen_SRV;
	float blur;
	DirectX::XMFLOAT4 judged_color;
#endif

public:
	sceneTitle() : 
		shader(),
		lights(),
		player(),
		pStageModel(std::make_unique<skinned_mesh>()),
		pTitleModel(std::make_unique<skinned_mesh>())
	{}
	~sceneTitle()
	{
		uninit();
	}
	void init();
	void update();
	void render();
	void uninit();
	void imGui();

	void TitleUpdate();
	void CameraMove();
	void TutorialUpdate();
	void TutorialStartUpdate();
	void TutorialGuardUpdate();
	void TutorialAttackUpdate();
	void TutorialEndUpdate();
	void ProcessCollision();
	void CameraUpdate(const Donya::Vector3& targetPosition);
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
EruptionParticle eruqtionParticle;
	AbsorptionParticle absorptionParticle;
	DustParticle dustParticle;
	SparkParticle sparkParticle;
	LocusParticle locusParticle;


	struct billboard
	{
		static_mesh pMesh;

		DirectX::XMFLOAT4 pos;			// 座標
		DirectX::XMFLOAT2 scale;		// 中心座標からの半径
		float angle;					// 角度

		DirectX::XMFLOAT2 texPos;
		DirectX::XMFLOAT2 texSize;
	};
	billboard data;
	
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

class SceneResult : public baseScene
{
private:
	static const int MAX_SIZE = 5;

	struct billboard
	{
		std::vector<static_mesh> pMesh;
		std::vector<int> num;

		DirectX::XMFLOAT4 pos;			// 座標
		DirectX::XMFLOAT2 scale;		// 中心座標からの半径
		float angle;					// 角度

		DirectX::XMFLOAT2 texPos;
		DirectX::XMFLOAT2 texSize;
	};

	// リザルトロゴ
//	std::shared_ptr<skinned_mesh> pLogo;		// temp

	// ランキング
//	std::shared_ptr<skinned_mesh> pRanking;		// temp
	int rankingScore[MAX_SIZE];
	billboard rankingRender[MAX_SIZE];

	// Your Score
//	std::shared_ptr<skinned_mesh> pYourScore;	// temp
	int yourScore;
	billboard yourScoreRender;

	// No.
//	std::shared_ptr<skinned_mesh> pNummber[10];	// temp

	// light
	Lights	lights;

	Sprite sprite;

	DirectX::XMFLOAT3 rankingPos;
	DirectX::XMFLOAT3 yourScorePos;

	DirectX::XMFLOAT2 rankingWidth;

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(rankingScore)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Ranking";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	SceneResult(int clearTime);
	~SceneResult() {}

	void init();
	void update();
	void render();
	void uninit();
	void imGui();
};
CEREAL_CLASS_VERSION(SceneResult, 0)

class SceneGameOver : public baseScene
{
private:


private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			//CEREAL_NVP(rankingScore)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Ranking";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI
public:
	SceneGameOver()
	{

	}
	~SceneGameOver()
	{

	}

	void init();
	void update();
	void render();
	void uninit();
	void imGui();
};

#endif // !SCENE_H_
