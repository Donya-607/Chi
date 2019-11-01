#ifndef INCLUDED_COLLISION_H_
#define INCLUDED_COLLISION_H_

#include <cstdint> // use for std::uint32_t

#include "cereal/cereal.hpp"

#include "Vector.h"
#include "Quaternion.h"

namespace Donya
{
	class Circle;

	/// <summary>
	/// Hit-Box of rectangle.<para></para>
	/// float cx, cy : Position of center. I recommend to setting world space.<para></para>
	/// float w, h : Specify half size.<para></para>
	/// bool exist : Is enable collision ?
	/// </summary>
	class Box
	{
	public:
		float	cx;		// Position of center.
		float	cy;		// Position of center.
		float	w;		// Half-width.
		float	h;		// Half-height.
		bool	exist;	// Is enable collision ?
	public:
		Box() : cx( 0 ), cy( 0 ), w( 0 ), h( 0 ), exist( true ) {}
		Box
		(
			float centerX, float centerY,
			float halfWidth, float halfHeight,
			bool  isExist = true
		) :
			cx( centerX ), cy( centerY ),
			w( halfWidth ), h( halfHeight ),
			exist( isExist )
		{}
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( cx ), CEREAL_NVP( cy ),
				CEREAL_NVP( w ), CEREAL_NVP( h ),
				CEREAL_NVP( exist )
			);
			if ( 1 <= version )
			{
				// archive();
			}
		}
	public:
		void Set				( float centerX, float centerY, float halfWidth, float halfHeight, bool isExist = true );
		void SetPosition		( const Donya::Vector2 &position )
		{
			cx = position.x;
			cy = position.y;
		}
		void SetHalfSize		( const Donya::Vector2 &halfSize )
		{
			w = halfSize.x;
			h = halfSize.y;
		}
		Donya::Vector2 GetPosition() const
		{
			return Donya::Vector2{ cx, cy };
		}
		Donya::Vector2 GetHalfSize() const
		{
			return Donya::Vector2{ w, h };
		}
	public:
		static bool IsHitPoint	( const Box &L, const float &RX, const float &RY, bool ignoreExistFlag = false );
		/// <summary>
		/// The "ScreenPos"s are add to position.
		/// </summary>
		static bool IsHitPoint	( const Box &L, const float &LBoxScreenPosX, const float &LBoxScreenPosY, const float &RX, const float &RY, bool ignoreExistFlag = false );
		static bool IsHitBox	( const Box &L, const Box &R, bool ignoreExistFlag = false );
		/// <summary>
		/// The "ScreenPos"s are add to position.
		/// </summary>
		static bool IsHitBox	( const Box &L, const float &LBoxScreenPosX, const float &LBoxScreenPosY, const Box &R, const float &RBoxScreenPosX, const float &RBoxScreenPosY, bool ignoreExistFlag = false );
		static bool IsHitCircle	( const Box &L, const Circle &R, bool ignoreExistFlag = false );
		/// <summary>
		/// The "ScreenPos"s are add to position.
		/// </summary>
		static bool IsHitCircle	( const Box &L, const float &LBoxScreenPosX, const float &LBoxScreenPosY, const Circle &R, const float &RCircleScreenPosX, const float &RCircleScreenPosY, bool ignoreExistFlag = false );
	public:
		static inline Box Nil()
		{
			return{ 0, 0, 0, 0, false };
		}
	};

	/// <summary>
	/// Hit-Box of circle.<para></para>
	/// float cx, cy : Position of center. I recommend to setting world space.<para></para>
	/// float radius : Specify radius.<para></para>
	/// bool exist :  Is enable collision ?
	/// </summary>
	class Circle
	{
	public:
		float	cx;		// Position of center.
		float	cy;		// Position of center.
		float	radius;
		bool	exist;	// Is enable collision ?
	public:
		Circle() : cx( 0 ), cy( 0 ), radius( 0 ), exist( true ) {}
		Circle
		(
			float centerX, float centerY,
			float rad,
			bool  isExist = true
		) :
			cx( centerX ), cy( centerY ),
			radius( rad ),
			exist( isExist )
		{}
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( cx ), CEREAL_NVP( cy ),
				CEREAL_NVP( radius ),
				CEREAL_NVP( exist )
			);
			if ( 1 <= version )
			{
				// archive();
			}
		}
	public:
		void Set				( float centerX, float centerY, float rad, bool isExist = true );
		void SetPosition		( const Donya::Vector2 &position )
		{
			cx = position.x;
			cy = position.y;
		}
		Donya::Vector2 GetPosition() const
		{
			return Donya::Vector2{ cx, cy };
		}
	public:
		static bool IsHitPoint	( const Circle &L, const float &RX, const float &RY, bool ignoreExistFlag = false );
		/// <summary>
		/// The "ScreenPos"s are add to position.
		/// </summary>
		static bool IsHitPoint	( const Circle &L, const float &LCircleScreenPosX, const float &LCircleScreenPosY, const float &RX, const float &RY, bool ignoreExistFlag = false );
		static bool IsHitCircle	( const Circle &L, const Circle &R, bool ignoreExistFlag = false );
		/// <summary>
		/// The "ScreenPos"s are add to position.
		/// </summary>
		static bool IsHitCircle	( const Circle &L, const float &LCircleScreenPosX, const float &LCircleScreenPosY, const Circle &R, const float &RCircleScreenPosX, const float &RCircleScreenPosY, bool ignoreExistFlag = false );
		static bool IsHitBox	( const Circle &L, const Box &R, bool ignoreExistFlag = false );
		/// <summary>
		/// The "ScreenPos"s are add to position.
		/// </summary>
		static bool IsHitBox	( const Circle &L, const float &LCircleScreenPosX, const float &LCircleScreenPosY, const Box &R, const float &RBoxScreenPosX, const float &RBoxScreenPosY, bool ignoreExistFlag = false );
	public:
		static inline Circle Nil()
		{
			return Circle{ 0, 0, 0, false };
		}
	};

	// TODO:Rename the box class, and apply Donya::Vector to position, and size.

	class Sphere;

	/// <summary>
	/// Hit-Box of AABB.<para></para>
	/// Vector3 pos : Center-position. the belong space is world-space when checking collision method.<para></para>
	/// Vector3 size : Half-size(like radius). the left is pos.x - size.x. please set to only positive value.<para></para>
	/// bool exist : Is enable collision ?
	/// </summary>
	class AABB
	{
	public:
		Donya::Vector3 pos;		// Center-position. the belong space is world-space when checking collision method.
		Donya::Vector3 size;	// Half-size(like radius). the left is pos.x - size.x. please set to only positive value.
		bool exist;				// Is enable collision ?
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( pos ),
				CEREAL_NVP( size ),
				CEREAL_NVP( exist )
			);
			if ( 1 <= version )
			{
				// archive();
			}
		}
	public:
		/// <summary>
		/// AABB vs Point, assumes that these belong in world-space when collision checking.<para></para>
		/// The "ignoreExistFlag" is specify disable of exist flag.
		/// </summary>
		static bool IsHitPoint( const AABB &worldSpaceBox, const Donya::Vector3 &worldSpacePoint, bool ignoreExistFlag = false );
		/// <summary>
		/// AABB vs AABB, assumes that these belong in world-space when collision checking.<para></para>
		/// The "ignoreExistFlag" is specify disable of exist flag.
		/// </summary>
		static bool IsHitAABB( const AABB &worldSpaceBoxL, const AABB &worldSpaceBoxR, bool ignoreExistFlag = false );
		/// <summary>
		/// AABB vs Sphere, assumes that these belong in world-space when collision checking.<para></para>
		/// The "ignoreExistFlag" is specify disable of exist flag.
		/// </summary>
		static bool IsHitSphere( const AABB &worldSpaceBox, const Sphere &worldSpaceSphere, bool ignoreExistFlag = false );
	public:
		static AABB Nil()
		{
			return AABB
			{
				Donya::Vector3{ 0.0f, 0.0f, 0.0f },
				Donya::Vector3{ 0.0f, 0.0f, 0.0f },
				false
			};
		}
	};

	/// <summary>
	/// Hit-Box of Sphere.<para></para>
	/// Vector3 pos : Center-position. the belong space is world-space when checking collision method.<para></para>
	/// float radius : Radius of sphere in world-space. please set to only positive value.<para></para>
	/// bool exist : Is enable collision ?
	/// </summary>
	class Sphere
	{
	public:
		Donya::Vector3	pos;		// Center-position. the belong space is world-space when checking collision method.
		float			radius;		// Radius of sphere in world-space. please set to only positive value.
		bool			exist;		// Is enable collision ?
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( pos ),
				CEREAL_NVP( radius ),
				CEREAL_NVP( exist )
			);
			if ( 1 <= version )
			{
				// archive();
			}
		}
	public:
		/// <summary>
		/// Sphere vs Point, assumes that these belong in world-space when collision checking.<para></para>
		/// The "ignoreExistFlag" is specify disable of exist flag.
		/// </summary>
		static bool IsHitPoint( const Sphere &worldSpaceSphere, const Donya::Vector3 &worldSpacePoint, bool ignoreExistFlag = false );
		/// <summary>
		/// Sphere vs Sphere, assumes that these belong in world-space when collision checking.<para></para>
		/// The "ignoreExistFlag" is specify disable of exist flag.
		/// </summary>
		static bool IsHitSphere( const Sphere &worldSpaceSphereL, const Sphere &worldSpaceSphereR, bool ignoreExistFlag = false );
		/// <summary>
		/// Sphere vs AABB, assumes that these belong in world-space when collision checking.<para></para>
		/// The "ignoreExistFlag" is specify disable of exist flag.
		/// </summary>
		static bool IsHitAABB( const Sphere &worldSpaceSphere, const AABB &worldSpaceBox, bool ignoreExistFlag = false );
	};

	/// <summary>
	/// Copy from OBB.h
	/// </summary>
	class OBB
	{
	public:
		Donya::Vector3		pos;
		Donya::Vector3		size;		// Half-size.
		Donya::Quaternion	orientation;
		bool exist{ true };
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( pos ),
				CEREAL_NVP( size ),
				CEREAL_NVP( orientation ),
				CEREAL_NVP( exist )
			);
			if ( 1 <= version )
			{
				// archive();
			}
		}
	public:
		bool JudgeOBB( const OBB *obb, bool ignoreExistFlag = false ) const;
	private:
		float LenSegOnSepAxis( DirectX::XMFLOAT3 *sep, DirectX::XMFLOAT3 *v1, DirectX::XMFLOAT3 *v2, DirectX::XMFLOAT3 *v3 = 0 ) const
		{
			// 3‚Â‚Ì“àÏ‚Ìâ‘Î’l‚Ì˜a‚Å“Š‰eü•ª’·‚ðŒvŽZ
			// •ª—£Ž²Sep‚Í•W€‰»‚³‚ê‚Ä‚¢‚é‚±‚Æ
			float dot1 = ( ( sep->x * v1->x ) + ( sep->y * v1->y ) + ( sep->z * v1->z ) );
			float dot2 = ( ( sep->x * v2->x ) + ( sep->y * v2->y ) + ( sep->z * v2->z ) );
			float dot3 = 0.0f;
			if ( !v3 )
			{
				dot3 = 0.0f;
			}
			else
			{
				dot3 = ( ( sep->x * v3->x ) + ( sep->y * v3->y ) + ( sep->z * v3->z ) );
			}

			float r1 = fabs( dot1 );
			float r2 = fabs( dot2 );
			float r3 = v3 ? ( fabs( dot3 ) ) : 0;

			return r1 + r2 + r3;
		}
	public:
		static OBB Nil()
		{
			return OBB{ {}, {}, {}, false };
		}
	public:
		static bool IsHitOBB( const OBB &L, const OBB &R, bool ignoreExistFlag = false )
		{
			return L.JudgeOBB( &R, ignoreExistFlag );
		}
	};

	struct OBBFrame
	{
		int currentFrame{};
		int enableFrameStart{};	// WIll be serialize. Contain start frame.
		int enableFrameLast{};	// WIll be serialize. Contain last frame.
		Donya::OBB OBB{};
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( enableFrameStart ),
				CEREAL_NVP( enableFrameLast ),
				CEREAL_NVP( OBB )
			);

			if ( 1 <= version )
			{
				// archive( CEREAL_NVP( x ) );
			}
		}
	public:
		void Update( int elapsedTime = 1 )
		{
			currentFrame += elapsedTime;

			OBB.exist = WithinEnableFrame() ? true : false;
		}

		bool WithinEnableFrame() const
		{
			return ( enableFrameStart <= currentFrame && currentFrame <= enableFrameLast ) ? true : false;
		}
	};

	bool		operator == ( const Box &L, const Box &R );
	static bool	operator != ( const Box &L, const Box &R ) { return !( L == R ); }

	bool		operator == ( const Circle &L, const Circle &R );
	static bool	operator != ( const Circle &L, const Circle &R ) { return !( L == R ); }

	bool		operator == ( const AABB &L, const AABB &R );
	static bool	operator != ( const AABB &L, const AABB &R ) { return !( L == R ); }

	bool		operator == ( const Sphere &L, const Sphere &R );
	static bool	operator != ( const Sphere &L, const Sphere &R ) { return !( L == R ); }
	
	bool		operator == ( const OBB &L, const OBB &R );
	static bool	operator != ( const OBB &L, const OBB &R ) { return !( L == R ); }
}


CEREAL_CLASS_VERSION( Donya::Box,		0 );
CEREAL_CLASS_VERSION( Donya::Circle,	0 );
CEREAL_CLASS_VERSION( Donya::AABB,		0 );
CEREAL_CLASS_VERSION( Donya::Sphere,	0 );
CEREAL_CLASS_VERSION( Donya::OBB,		0 );
CEREAL_CLASS_VERSION( Donya::OBBFrame,	0 );

#endif // INCLUDED_COLLISION_H_
