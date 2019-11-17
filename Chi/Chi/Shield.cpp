#include "Shield.h"

#include "Donya/FilePath.h"
#include "Donya/Useful.h"	// Use ToRadian().

#include "GameLibFunctions.h"
#include "skinned_mesh.h"

ShieldParam::ShieldParam() :
	m()
{}
ShieldParam::~ShieldParam() = default;

void ShieldParam::Init( size_t motionCount )
{
	m.motionSpeeds.resize( motionCount );
	LoadParameter();
}
void ShieldParam::Uninit()
{
	// No op.
}

ShieldParam::Member ShieldParam::Open()
{
	return Get().Content();
}

void ShieldParam::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void ShieldParam::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void ShieldParam::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Shield.AdjustData" ) )
		{
			ImGui::DragInt( "Frame.MaxUnfoldableFrame", &m.maxUnfoldableFrame );
			ImGui::DragInt( "Frame.DecreaseSpeed", &m.decreaseSpeed );
			ImGui::DragInt( "Frame.IncreaseSpeed", &m.increaseSpeed );
			ImGui::DragInt( "Frame.When.Use.ConsumptionAmount", &m.consumptionAmount );
			ImGui::DragInt( "Frame.When.Protected.RecoveryAmount", &m.recoveryAmount );
			ImGui::Text( "" );

			ImGui::DragFloat ( "Draw.Scale",  &m.drawScale );
			ImGui::DragFloat3( "Draw.Offset", &m.drawOffset.x );
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "Motion.Speed" ) )
			{
				const std::vector<std::string> motionNames
				{
					"Open",
					"Idle",
					"React",
				};
				const size_t COUNT = m.motionSpeeds.size();
				_ASSERT_EXPR( motionNames.size() == COUNT, L"Error : Logical error by human !" );
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::DragFloat( motionNames[i].c_str(), &m.motionSpeeds[i], 0.005f );
				}

				ImGui::TreePop();
			}
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "Collisions" ) )
			{
				auto ShowAABB = []( const std::string &prefix, Donya::AABB *pAABB )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pAABB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pAABB->size.x );
					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pAABB->exist );
				};
				auto ShowOBB  = []( const std::string &prefix, Donya::OBB *pOBB )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pOBB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pOBB->size.x );

					Donya::Vector3 euler = pOBB->orientation.GetEulerAngles();
					ImGui::SliderFloat3( ( prefix + ".EulerAngle(Radian)" ).c_str(), &euler.x, ToRadian( -360.0f ), ToRadian( 360.0f ) );
					pOBB->orientation = Donya::Quaternion::Make( euler.x, euler.y, euler.z );

					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pOBB->exist );
				};

				if ( ImGui::TreeNode( "Maximum Size" ) )
				{
					ShowAABB( "HitBox", &m.hitBoxMax );
					ImGui::TreePop();
				}
				if ( ImGui::TreeNode( "Minimum Size" ) )
				{
					ShowAABB( "HitBox", &m.hitBoxMax );
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "File.I/O" ) )
			{
				static bool isBinary = true;
				if ( ImGui::RadioButton( "Binary", isBinary ) ) { isBinary = true; }
				if ( ImGui::RadioButton( "JSON", !isBinary ) ) { isBinary = false; }
				std::string loadStr{ "Load " };
				loadStr += ( isBinary ) ? "Binary" : "JSON";

				if ( ImGui::Button( "Save" ) )
				{
					SaveParameter();
				}
				// if ( ImGui::Button( Donya::MultiToUTF8( loadStr ).c_str() ) )
				if ( ImGui::Button( loadStr.c_str() ) )
				{
					LoadParameter( isBinary );
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI

Shield::Shield() :
	status( State::NotExist ),
	unfoldTimer(),
	currentMotion( Idle ),
	models(),
	nowUnfolding( false )
{

}
Shield::~Shield() = default;

void Shield::Init()
{

}
void Shield::Uninit()
{

}

void Shield::Update( bool isUnfolding )
{

}

void Shield::Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection, const Donya::Vector4x4 &matParent )
{

}

bool Shield::CanUnfold() const
{

}

Donya::OBB Shield::GetHitBox( const Donya::Quaternion &parentOrientation ) const
{

}

void Shield::Recover()
{

}

#if USE_IMGUI

void Shield::UseImGui()
{

}

#endif // USE_IMGUI
