#include "AssetLoader.h"
#include "GameLibFunctions.h"
#include "Donya/FilePath.h"
#include "Donya/Useful.h"


void AssetLoader::LoadModel()
{
	auto InitializeModel = [](std::shared_ptr<skinned_mesh>* ppMesh)
	{
		*ppMesh = std::make_unique<skinned_mesh>();
		setAnimFlame(ppMesh->get(), 0);
	};

	// Player
	std::vector<std::shared_ptr<skinned_mesh>*> playerModelRefs
	{
		&playerModels.pIdle,
		&playerModels.pRun,
		&playerModels.pDefend,
		&playerModels.pAttack,
	};
	for (auto& it : playerModelRefs)
	{
		InitializeModel(it);
	}
	loadFBX( playerModels.pIdle.get(),	 GetModelPath( ModelAttribute::PlayerIdle	) );
	loadFBX( playerModels.pRun.get(),	 GetModelPath( ModelAttribute::PlayerRun	) );
	loadFBX( playerModels.pDefend.get(), GetModelPath( ModelAttribute::PlayerDefend	) );
	loadFBX( playerModels.pAttack.get(), GetModelPath( ModelAttribute::PlayerAtk	) );

	// Boss Golem
	std::vector<std::shared_ptr<skinned_mesh>*> golemModelRefs
	{
		&golemModels.pIdle,
		&golemModels.pAtkSwing,
		&golemModels.pAtkFast,
		&golemModels.pAtkRotate,
	};
	for (auto& it : golemModelRefs)
	{
		InitializeModel(it);
	}
	loadFBX( golemModels.pIdle.get(),		GetModelPath( ModelAttribute::GolemIdle		 ) );
	loadFBX( golemModels.pAtkSwing.get(),	GetModelPath( ModelAttribute::GolemAtkSwing  ) );
	loadFBX( golemModels.pAtkFast.get(),	GetModelPath( ModelAttribute::GolemAtkFast	 ) );
	loadFBX( golemModels.pAtkRotate.get(),	GetModelPath( ModelAttribute::GolemAtkRotate ) );
}

