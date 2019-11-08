#pragma once
#include <memory>
#include "skinned_mesh.h"


class AssetLoader
{
private:
	struct PlayerModels
	{
		std::shared_ptr<skinned_mesh> pIdle		 { nullptr };
		std::shared_ptr<skinned_mesh> pRun		 { nullptr };
		std::shared_ptr<skinned_mesh> pDefend	 { nullptr };
		std::shared_ptr<skinned_mesh> pAttack	 { nullptr };
	};
	struct GolemModels
	{
		std::shared_ptr<skinned_mesh> pIdle		 { nullptr };
		std::shared_ptr<skinned_mesh> pAtkFast	 { nullptr };
		std::shared_ptr<skinned_mesh> pAtkSwing	 { nullptr };
		std::shared_ptr<skinned_mesh> pAtkRotate { nullptr };
	};

public: // model
	PlayerModels playerModels;
	GolemModels golemModels;

public: // texture


public: // sound


public:
	AssetLoader() : playerModels(), golemModels() {}
	~AssetLoader() {}

	static AssetLoader* GetInstance()
	{
		static AssetLoader instance;
		return &instance;
	}

public:
	void LoadModel();
};