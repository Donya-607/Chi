#pragma once

#include "../gameLib.h"
#include "../light.h"

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include "Serializer.h"

namespace DirectX
{
	// For defining serialize function of below lights.

	template<class Archive>
	void serialize( Archive &archive, XMFLOAT4 &float4 )
	{
		archive
		(
			cereal::make_nvp( "x", float4.x ),
			cereal::make_nvp( "y", float4.y ),
			cereal::make_nvp( "z", float4.z ),
			cereal::make_nvp( "w", float4.w )
		);
	}
}

template<class Archive>
void serialize( Archive &archive, LineLight &lineLight, std::uint32_t version )
{
	archive
	(
		cereal::make_nvp( "Color", lineLight.color ),
		cereal::make_nvp( "Direction", lineLight.direction )
	);

	if ( 1 <= version )
	{
		// archive( CEREAL_NVP( x ) );
	}
}
CEREAL_CLASS_VERSION( LineLight, 0 )

template<class Archive>
void serialize( Archive &archive, PointLight &pointLight, std::uint32_t version )
{
	archive
	(
		cereal::make_nvp( "Position", pointLight.pos ),
		cereal::make_nvp( "Color", pointLight.color ),
		cereal::make_nvp( "Attenuate", pointLight.attenuate )
	);

	if ( 1 <= version )
	{
		// archive( CEREAL_NVP( x ) );
	}
}
CEREAL_CLASS_VERSION( PointLight, 0 )

struct Lights
{
public:
	LineLight direction{};
	std::vector<PointLight> points{};
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
			CEREAL_NVP( direction ),
			CEREAL_NVP( points )
		);

		if ( 1 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
};

CEREAL_CLASS_VERSION( Lights, 0 )
