#include "Stage.h"

#include "Donya/Constant.h"	// Use DEBUG_MODE macro.
#include "Donya/FilePath.h"

#include "GameLibFunctions.h"

Stage::Stage() : stageNo( 0 ), pModel( std::make_unique<skinned_mesh>())
{}
Stage::~Stage() = default;

void Stage::Init( int stageNumber )
{
	stageNo = stageNumber;

	LoadModel();
}
void Stage::Uninit()
{
	pModel.reset();
}

void Stage::Update()
{
	// No op.
}

void Stage::Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	Donya::Vector4x4 W = Donya::Vector4x4::Identity();
	Donya::Vector4x4 WVP = W * matView * matProjection;

	FBXRender( pModel.get(), WVP, W );
}

void Stage::LoadModel()
{
	loadFBX( pModel.get(), GetModelPath( ModelAttribute::Stage ) );
}
