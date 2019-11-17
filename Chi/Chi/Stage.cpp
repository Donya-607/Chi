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

void Stage::Draw( fbx_shader& hlsl,const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	Donya::Vector4x4 W = Donya::Vector4x4::Identity();
	Donya::Vector4x4 WVP = W * matView * matProjection;

	// HACK : Should serialize this.
	constexpr float ANIMATION_SPEED = 0.5f;
	FBXRender( pModel.get(),hlsl, WVP, W, ANIMATION_SPEED );
}

void Stage::LoadModel()
{
	std::string filePath{};
	switch ( stageNo )
	{
	case 0:		filePath = GetModelPath( ModelAttribute::Stage01 ); break;
	case 1:		filePath = GetModelPath( ModelAttribute::Stage02 ); break;
	case 2:		filePath = GetModelPath( ModelAttribute::Stage03 ); break;
	default:	_ASSERT_EXPR( 0, L"Error : Passed stage number does not registered !" ); return;
	}

	loadFBX( pModel.get(), filePath );
}
