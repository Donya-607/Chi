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
#if DEBUG_MODE
	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( 0.2f );
	Donya::Vector4x4 R = Donya::Vector4x4::Identity();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( { 0.0f, 0.0f, 0.0f } );
	Donya::Vector4x4 W = S * R * T;
	Donya::Vector4x4 WVP = W * matView * matProjection;
#else
	Donya::Vector4x4 W = Donya::Vector4x4::Identity();
	Donya::Vector4x4 WVP = W * matView * matProjection;
#endif // DEBUG_MODE

	FBXRender( pModel.get(), WVP, W );
}

void Stage::LoadModel()
{
	loadFBX( pModel.get(), GetModelPath( ModelAttribute::Stage ) );
}
