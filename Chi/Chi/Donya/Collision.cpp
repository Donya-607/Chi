#include "Collision.h"

#include <array>

#include "Constant.h"
#include "Useful.h"	// Use ZeroEqual().

#undef max
#undef min

namespace Donya
{
	void Box::Set			( float centerX, float centerY, float halfWidth, float halfHeight, bool isExist )
	{
		cx		= centerX;
		cy		= centerY;
		w		= halfWidth;
		h		= halfHeight;
		exist	= isExist;
	}
	bool Box::IsHitPoint	( const Box &L, const float &RX, const float &RY, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && !L.exist )
		{
			return false;
		}
		// else

		if	(
				L.cx - L.w	<= RX			&&	// X1 - W1	<= X2
				RX			<= L.cx + L.w	&&	// X2		<= X1 + W1
				L.cy - L.h	<= RY			&&	// Y1 - H1	<= Y2
				RY			<= L.cy + L.h		// Y2		<= Y1 + H2
			)
		{
			return true;
		}
		//else
		return false;
	}
	bool Box::IsHitPoint	( const Box &L, const float &LBoxScreenPosX, const float &LBoxScreenPosY, const float &RX, const float &RY, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && !L.exist )
		{
			return false;
		}
		// else

		Box tmp =
		{
			L.cx + LBoxScreenPosX,
			L.cy + LBoxScreenPosY,
			L.w,
			L.h,
			true
		};
		return Box::IsHitPoint( tmp, RX, RY );
	}
	bool Box::IsHitBox		( const Box &L, const Box &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) )
		{
			return false;
		}
		// else

		if	(
				L.cx - L.w <= R.cx + R.w	&&	// X1 - W1 < X2 + W2
				R.cx - R.w <= L.cx + L.w	&&	// X2 - W2 < X1 + W1
				L.cy - L.h <= R.cy + R.h	&&	// Y1 - H1 < Y2 + H2
				R.cy - R.h <= L.cy + L.h		// Y2 - H2 < Y1 + H1
			)
		{
			return true;
		}
		//else
		return false;
	}
	bool Box::IsHitBox		( const Box &L, const float &LBoxScreenPosX, const float &LBoxScreenPosY, const Box &R, const float &RBoxScreenPosX, const float &RBoxScreenPosY, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) )
		{
			return false;
		}
		// else

		Box tmpL =
		{
			L.cx + LBoxScreenPosX,
			L.cy + LBoxScreenPosY,
			L.w,
			L.h,
			true
		};
		Box tmpR =
		{
			R.cx + RBoxScreenPosX,
			R.cy + RBoxScreenPosY,
			R.w,
			R.h,
			true
		};
		return Box::IsHitBox( tmpL, tmpR );
	}
	bool Box::IsHitCircle	( const Box &L, const Circle &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) )
		{
			return false;
		}
		// else

		// ˆ—‚Ì“à—e
		/*
			Œ³‚Ì’·•ûŒ`‚ð
			‰~‚Ì”¼Œa‚Ô‚ñ‘å‚«‚­‚µC
			Šp‚ðŠÛ‚­‚µ‚½’·•ûŒ`	‚ðì‚èC
			‚±‚ê‚Æ‰~‚Ì’†S“_‚Æ‚ÌÚG‚ðŒ©‚é
		*/

		// c’·‚É‚µ‚½’·•ûŒ`‚Æ‚ÌÚG
		{
			Box tmp =
			{
				L.cx, 
				L.cy, 
				L.w, 
				L.h + R.radius, 
				true
			};
			if ( Box::IsHitPoint( tmp, R.cx, R.cy ) )
			{
				return true;
			}
		}
		// ‰¡’·‚É‚µ‚½’·•ûŒ`‚Æ‚ÌÚG
		{
			Box tmp =
			{
				L.cx,
				L.cy,
				L.w + R.radius,
				L.h,
				true
			};
			if ( Box::IsHitPoint( tmp, R.cx, R.cy ) )
			{
				return true;
			}
		}
		// Žl‹÷‚©‚ç‚Ì‹——£‚ðŒ©‚é
		{
			if	(
					Circle::IsHitPoint( R, L.cx - L.w,	L.cy - L.h )	||	// ¶ã
					Circle::IsHitPoint( R, L.cx - L.w,	L.cy + L.h )	||	// ¶‰º
					Circle::IsHitPoint( R, L.cx + L.w,	L.cy - L.h )	||	// ‰Eã
					Circle::IsHitPoint( R, L.cx + L.w,	L.cy + L.h )		// ‰E‰º
				)
			{
				return true;
			}
		}
		return false;
	}
	bool Box::IsHitCircle	( const Box &L, const float &LBoxScreenPosX, const float &LBoxScreenPosY, const Circle &R, const float &RCircleScreenPosX, const float &RCircleScreenPosY, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) )
		{
			return false;
		}
		// else

		Box tmpL =
		{
			L.cx + LBoxScreenPosX,
			L.cy + LBoxScreenPosY,
			L.w,
			L.h,
			true
		};
		Circle tmpR =
		{
			R.cx + RCircleScreenPosX,
			R.cy + RCircleScreenPosY,
			R.radius,
			true
		};
		return Box::IsHitCircle( tmpL, tmpR );
	}

	void Circle::Set		( float centerX, float centerY, float rad, bool isExist )
	{
		cx		= centerX;
		cy		= centerY;
		radius	= rad;
		exist	= isExist;
	}
	bool Circle::IsHitPoint	( const Circle &L, const float &RX, const float &RY, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && !L.exist )
		{
			return false;
		}
		// else

		return ( ( ( RX - L.cx ) * ( RX - L.cx ) ) + ( ( RY - L.cy ) * ( RY - L.cy ) ) < ( L.radius * L.radius ) );
	}
	bool Circle::IsHitPoint	( const Circle &L, const float &LCircleScreenPosX, const float &LCircleScreenPosY, const float &RX, const float &RY, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && !L.exist )
		{
			return false;
		}
		// else

		Circle tmp =
		{
			L.cx + LCircleScreenPosX,
			L.cy + LCircleScreenPosY,
			L.radius,
			true
		};
		return Circle::IsHitPoint( tmp, RX, RY );
	}
	bool Circle::IsHitCircle( const Circle &L, const Circle &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) )
		{
			return false;
		}
		// else

		float dx = R.cx - L.cx;
		float dy = R.cy - L.cy;
		float rad = R.radius + L.radius;

		return ( ( dx * dx ) + ( dy * dy ) < rad * rad );
	}
	bool Circle::IsHitCircle( const Circle &L, const float &LCircleScreenPosX, const float &LCircleScreenPosY, const Circle &R, const float &RCircleScreenPosX, const float &RCircleScreenPosY, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) )
		{
			return false;
		}
		// else

		Circle tmpL =
		{
			L.cx + LCircleScreenPosX,
			L.cy + LCircleScreenPosY,
			L.radius,
			true
		};
		Circle tmpR =
		{
			R.cx + RCircleScreenPosX,
			R.cy + RCircleScreenPosY,
			R.radius,
			true
		};
		return Circle::IsHitCircle( tmpL, tmpR );
	}
	bool Circle::IsHitBox	( const Circle &L, const Box &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) )
		{
			return false;
		}
		// else

		return Box::IsHitCircle( R, L );
	}
	bool Circle::IsHitBox	( const Circle &L, const float &LCircleScreenPosX, const float &LCircleScreenPosY, const Box &R, const float &RBoxScreenPosX, const float &RBoxScreenPosY, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) )
		{
			return false;
		}
		// else

		return Box::IsHitCircle( R, RBoxScreenPosX, RBoxScreenPosY, L, LCircleScreenPosX, LCircleScreenPosY );
	}

	bool AABB::IsHitPoint	( const AABB &L, const Donya::Vector3 &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && !L.exist ) { return false; }
		// else

		constexpr size_t AXIS_COUNT = 3;

		const std::array<float, AXIS_COUNT> point{ R.x,			R.y,		R.z			};	// [0:X][1:Y][2:Z]
		const std::array<float, AXIS_COUNT> AABB { L.pos.x,		L.pos.y,	L.pos.z		};	// [0:X][1:Y][2:Z]
		const std::array<float, AXIS_COUNT> size { L.size.x,	L.size.y,	L.size.z	};	// [0:X][1:Y][2:Z]

		for ( size_t i = 0; i < AXIS_COUNT; ++i )
		{
			// If isn't [Min <= Point <= Max], judge to false.
			if ( point[i] < AABB[i] - size[i] ) { return false; }
			if ( point[i] > AABB[i] + size[i] ) { return false; }
		}

		return true;
	}
	bool AABB::IsHitAABB	( const AABB &L, const AABB &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) ) { return false; }
		// else

		// Judge by "AABB of extended by size of other" vs "position of other".
	
		AABB extAABB = L;
		extAABB.size += R.size;	// If "R.size" is negative, maybe this method does not work.

		const Donya::Vector3 &point = R.pos;

		return IsHitPoint( extAABB, point );
	
	}
	bool AABB::IsHitSphere	( const AABB &L, const Sphere &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) ) { return false; }
		// else

		auto CalcShortestDistanceSq = []( const AABB &L, const Donya::Vector3 &R )->float
		{
			// from http://marupeke296.com/COL_3D_No11_AABBvsPoint.html

			constexpr size_t AXIS_COUNT = 3;

			const std::array<float, AXIS_COUNT> point{ R.x,			R.y,		R.z			};	// [0:X][1:Y][2:Z]
			const std::array<float, AXIS_COUNT> AABB { L.pos.x,		L.pos.y,	L.pos.z		};	// [0:X][1:Y][2:Z]
			const std::array<float, AXIS_COUNT> size { L.size.x,	L.size.y,	L.size.z	};	// [0:X][1:Y][2:Z]

			std::array<float, AXIS_COUNT> distance{};

			float max{}, min{};
			for ( size_t i = 0; i < AXIS_COUNT; ++i )
			{
				max = AABB[i] + size[i];
				min = AABB[i] - size[i];

				if ( point[i] > max ) { distance[i] = point[i] - max; }
				if ( point[i] < min ) { distance[i] = point[i] - min; }
			}

			Donya::Vector3 v{ distance[0], distance[1], distance[2] };

			return v.LengthSq();
		};

		float distanceSq = CalcShortestDistanceSq( L, R.pos );
		return ( distanceSq < ( R.radius * R.radius ) );
	}

	bool Sphere::IsHitPoint	( const Sphere &L, const Donya::Vector3 &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && !L.exist ) { return false; }
		// else

		Donya::Vector3 d = R - L.pos;
		return ( d.LengthSq() < L.radius * L.radius );
	}
	bool Sphere::IsHitSphere( const Sphere &L, const Sphere &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) ) { return false; }
		// else

		float r = L.radius + R.radius;
		Donya::Vector3 d = R.pos - L.pos;

		return ( d.LengthSq() < ( r * r ) );
	}
	bool Sphere::IsHitAABB	( const Sphere &L, const AABB &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) ) { return false; }
		// else

		return AABB::IsHitSphere( R, L );
	}

	bool OBB::JudgeOBB( const OBB *obb, bool ignoreExistFlag ) const
	{
		if ( !ignoreExistFlag && ( !exist || !obb->exist ) ) { return false; }
		// else

		DirectX::XMFLOAT3 Nv1_1;
		DirectX::XMFLOAT3 Nv2_1;
		DirectX::XMFLOAT3 Nv3_1;

		DirectX::XMFLOAT3 Nv1_2;
		DirectX::XMFLOAT3 Nv2_2;
		DirectX::XMFLOAT3 Nv3_2;


		DirectX::XMFLOAT3 v1_1;
		DirectX::XMFLOAT3 v2_1;
		DirectX::XMFLOAT3 v3_1;

		DirectX::XMFLOAT3 v1_2;
		DirectX::XMFLOAT3 v2_2;
		DirectX::XMFLOAT3 v3_2;


		//	‰ñ“]
		DirectX::XMFLOAT4X4 rota, rota2;

		rota = orientation.RequireRotationMatrix();

		Nv1_1.x = rota._11;
		Nv1_1.y = rota._12;
		Nv1_1.z = rota._13;

		Nv2_1.x = rota._21;
		Nv2_1.y = rota._22;
		Nv2_1.z = rota._23;

		Nv3_1.x = rota._31;
		Nv3_1.y = rota._32;
		Nv3_1.z = rota._33;

		rota2 = obb->orientation.RequireRotationMatrix();

		Nv1_2.x = rota2._11;
		Nv1_2.y = rota2._12;
		Nv1_2.z = rota2._13;

		Nv2_2.x = rota2._21;
		Nv2_2.y = rota2._22;
		Nv2_2.z = rota2._23;

		Nv3_2.x = rota2._31;
		Nv3_2.y = rota2._32;
		Nv3_2.z = rota2._33;

		v1_1.x = Nv1_1.x * size.x;
		v1_1.y = Nv1_1.y * size.x;
		v1_1.z = Nv1_1.z * size.x;
								
		v2_1.x = Nv2_1.x * size.y;
		v2_1.y = Nv2_1.y * size.y;
		v2_1.z = Nv2_1.z * size.y;
								
		v3_1.x = Nv3_1.x * size.z;
		v3_1.y = Nv3_1.y * size.z;
		v3_1.z = Nv3_1.z * size.z;

		v1_2.x = Nv1_2.x * obb->size.x;
		v1_2.y = Nv1_2.y * obb->size.x;
		v1_2.z = Nv1_2.z * obb->size.x;

		v2_2.x = Nv2_2.x * obb->size.y;
		v2_2.y = Nv2_2.y * obb->size.y;
		v2_2.z = Nv2_2.z * obb->size.y;

		v3_2.x = Nv3_2.x * obb->size.z;
		v3_2.y = Nv3_2.y * obb->size.z;
		v3_2.z = Nv3_2.z * obb->size.z;


		DirectX::XMFLOAT3 interval;

		interval.x = pos.x - obb->pos.x;
		interval.y = pos.y - obb->pos.y;
		interval.z = pos.z - obb->pos.z;

		float r1 = 0.0f;
		float r2 = 0.0f;
		float l = 0.0f;
		float dot = 0.0f;


		// •ª—£Ž² v1_1
		r1 = sqrtf( ( v1_1.x * v1_1.x ) + ( v1_1.y * v1_1.y ) + ( v1_1.z * v1_1.z ) );
		r2 = LenSegOnSepAxis( &Nv1_1, &v1_2, &v2_2, &v3_2 );
		dot = ( ( interval.x * Nv1_1.x ) + ( interval.y * Nv1_1.y ) + ( interval.z * Nv1_1.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² v2_1
		r1 = sqrtf( ( v2_1.x * v2_1.x ) + ( v2_1.y * v2_1.y ) + ( v2_1.z * v2_1.z ) );
		r2 = LenSegOnSepAxis( &Nv2_1, &v1_2, &v2_2, &v3_2 );
		dot = ( ( interval.x * Nv2_1.x ) + ( interval.y * Nv2_1.y ) + ( interval.z * Nv2_1.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² v3_1
		r1 = sqrtf( ( v3_1.x * v3_1.x ) + ( v3_1.y * v3_1.y ) + ( v3_1.z * v3_1.z ) );
		r2 = LenSegOnSepAxis( &Nv3_1, &v1_2, &v2_2, &v3_2 );
		dot = ( ( interval.x * Nv3_1.x ) + ( interval.y * Nv3_1.y ) + ( interval.z * Nv3_1.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}


		// •ª—£Ž² v1_2
		r1 = LenSegOnSepAxis( &Nv1_2, &v1_1, &v2_1, &v3_1 );
		r2 = sqrtf( ( v1_2.x * v1_2.x ) + ( v1_2.y * v1_2.y ) + ( v1_2.z * v1_2.z ) );
		dot = ( ( interval.x * Nv1_2.x ) + ( interval.y * Nv1_2.y ) + ( interval.z * Nv1_2.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² v2_2
		r1 = LenSegOnSepAxis( &Nv2_2, &v1_1, &v2_1, &v3_1 );
		r2 = sqrtf( ( v2_2.x * v2_2.x ) + ( v2_2.y * v2_2.y ) + ( v2_2.z * v2_2.z ) );
		dot = ( ( interval.x * Nv2_2.x ) + ( interval.y * Nv2_2.y ) + ( interval.z * Nv2_2.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² v3_2
		r1 = LenSegOnSepAxis( &Nv3_2, &v1_1, &v2_1, &v3_1 );
		r2 = sqrtf( ( v3_2.x * v3_2.x ) + ( v3_2.y * v3_2.y ) + ( v3_2.z * v3_2.z ) );
		dot = ( ( interval.x * Nv3_2.x ) + ( interval.y * Nv3_2.y ) + ( interval.z * Nv3_2.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}


		DirectX::XMFLOAT3 cross;

		// •ª—£Ž² cross v1_1 + v1_2
		cross.x = v1_1.y * v1_2.z - v1_1.z * v1_2.y;
		cross.y = v1_1.z * v1_2.x - v1_1.x * v1_2.z;
		cross.z = v1_1.x * v1_2.y - v1_1.y * v1_2.x;

		r1 = LenSegOnSepAxis( &cross, &v2_1, &v3_1 );
		r2 = LenSegOnSepAxis( &cross, &v2_2, &v3_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² cross v1_1 + v2_2
		cross.x = v1_1.y * v2_2.z - v1_1.z * v2_2.y;
		cross.y = v1_1.z * v2_2.x - v1_1.x * v2_2.z;
		cross.z = v1_1.x * v2_2.y - v1_1.y * v2_2.x;

		r1 = LenSegOnSepAxis( &cross, &v2_1, &v3_1 );
		r2 = LenSegOnSepAxis( &cross, &v1_2, &v3_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² cross v1_1 + v3_2
		cross.x = v1_1.y * v3_2.z - v1_1.z * v3_2.y;
		cross.y = v1_1.z * v3_2.x - v1_1.x * v3_2.z;
		cross.z = v1_1.x * v3_2.y - v1_1.y * v3_2.x;

		r1 = LenSegOnSepAxis( &cross, &v2_1, &v3_1 );
		r2 = LenSegOnSepAxis( &cross, &v1_2, &v2_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² cross v2_1 + v1_2
		cross.x = v2_1.y * v1_2.z - v2_1.z * v1_2.y;
		cross.y = v2_1.z * v1_2.x - v2_1.x * v1_2.z;
		cross.z = v2_1.x * v1_2.y - v2_1.y * v1_2.x;

		r1 = LenSegOnSepAxis( &cross, &v1_1, &v3_1 );
		r2 = LenSegOnSepAxis( &cross, &v2_2, &v3_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² cross v2_1 + v2_2
		cross.x = v2_1.y * v2_2.z - v2_1.z * v2_2.y;
		cross.y = v2_1.z * v2_2.x - v2_1.x * v2_2.z;
		cross.z = v2_1.x * v2_2.y - v2_1.y * v2_2.x;

		r1 = LenSegOnSepAxis( &cross, &v1_1, &v3_1 );
		r2 = LenSegOnSepAxis( &cross, &v1_2, &v3_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² cross v2_1 + v3_2
		cross.x = v2_1.y * v3_2.z - v2_1.z * v3_2.y;
		cross.y = v2_1.z * v3_2.x - v2_1.x * v3_2.z;
		cross.z = v2_1.x * v3_2.y - v2_1.y * v3_2.x;

		r1 = LenSegOnSepAxis( &cross, &v1_1, &v3_1 );
		r2 = LenSegOnSepAxis( &cross, &v1_2, &v2_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² cross v3_1 + v1_2
		cross.x = v3_1.y * v1_2.z - v3_1.z * v1_2.y;
		cross.y = v3_1.z * v1_2.x - v3_1.x * v1_2.z;
		cross.z = v3_1.x * v1_2.y - v3_1.y * v1_2.x;

		r1 = LenSegOnSepAxis( &cross, &v1_1, &v2_1 );
		r2 = LenSegOnSepAxis( &cross, &v2_2, &v3_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² cross v3_1 + v2_2
		cross.x = v3_1.y * v2_2.z - v3_1.z * v2_2.y;
		cross.y = v3_1.z * v2_2.x - v3_1.x * v2_2.z;
		cross.z = v3_1.x * v2_2.y - v3_1.y * v2_2.x;

		r1 = LenSegOnSepAxis( &cross, &v1_1, &v2_1 );
		r2 = LenSegOnSepAxis( &cross, &v1_2, &v3_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}

		// •ª—£Ž² cross v3_1 + v3_2
		cross.x = v3_1.y * v3_2.z - v3_1.z * v3_2.y;
		cross.y = v3_1.z * v3_2.x - v3_1.x * v3_2.z;
		cross.z = v3_1.x * v3_2.y - v3_1.y * v3_2.x;

		r1 = LenSegOnSepAxis( &cross, &v1_1, &v2_1 );
		r2 = LenSegOnSepAxis( &cross, &v1_2, &v2_2 );
		dot = ( ( interval.x * cross.x ) + ( interval.y * cross.y ) + ( interval.z * cross.z ) );
		l = fabs( dot );
		if ( l > r1 + r2 )
		{
			return false;
		}


		return true;
	}

	float CalcShortestDistance( const OBB &L, const Donya::Vector3 &point )
	{
		// see http://marupeke296.com/COL_3D_No12_OBBvsPoint.html

		enum IntAxis{ X = 0, Y, Z, END };
		auto RotatedAxis = []( const OBB &OBB, IntAxis axisIndex )->Donya::Vector3
		{
			_ASSERT_EXPR( 0 <= axisIndex && axisIndex < END, L"Error : Passed index out of range !" );

			Donya::Vector3 axis{};
			switch ( axisIndex )
			{
			case X: axis = Donya::Vector3{ 1.0f, 0.0f, 0.0f }; break;
			case Y: axis = Donya::Vector3{ 0.0f, 1.0f, 0.0f }; break;
			case Z: axis = Donya::Vector3{ 0.0f, 0.0f, 1.0f }; break;
			default: return {};
			}

			return OBB.orientation.RotateVector( axis );
		};

		Donya::Vector3 protrudedSum{};
		Donya::Vector3 axis{};
		for ( int i = 0; i < END; ++i )
		{
			axis = RotatedAxis( L, static_cast<IntAxis>( i ) );

			float halfLength = axis.Length();
			if ( halfLength <= 0 ) { continue; } // This case can not calculate.
			// else

			float magni = Donya::Vector3::Dot( ( point - L.pos ), axis ) / halfLength;
			magni = fabsf( magni );

			if ( 1.0f < magni )
			{
				protrudedSum += ( 1.0f - magni ) * halfLength * axis;
			}
		}

		return protrudedSum.Length();
	}

	bool OBB::IsHitSphere( const OBB &L, const Sphere &R, bool ignoreExistFlag )
	{
		if ( !ignoreExistFlag && ( !L.exist || !R.exist ) ) { return false; }
		// else

		float distance = CalcShortestDistance( L, R.pos );
		return ( distance <= R.radius ) ? true : false;
	}

	bool operator == ( const Box	&L, const Box		&R )
	{
		if ( !ZeroEqual( L.cx - R.cx ) )	{ return false; }
		if ( !ZeroEqual( L.cy - R.cy ) )	{ return false; }
		if ( !ZeroEqual( L.w  - R.w  ) )	{ return false; }
		if ( !ZeroEqual( L.h  - R.h  ) )	{ return false; }
		if ( L.exist != R.exist )			{ return false; }
		// else
		return true;
	}
	bool operator == ( const Circle	&L, const Circle	&R )
	{
		if ( !ZeroEqual( L.cx - R.cx ) )			{ return false; }
		if ( !ZeroEqual( L.cy - R.cy ) )			{ return false; }
		if ( !ZeroEqual( L.radius - R.radius ) )	{ return false; }
		if ( L.exist != R.exist )					{ return false; }
		// else
		return true;
	}
	bool operator == ( const AABB	&L, const AABB		&R )
	{
		if ( ! ( L.pos  - R.pos  ).IsZero() )		{ return false; }
		if ( ! ( L.size - R.size ).IsZero() )		{ return false; }
		if ( L.exist != R.exist )					{ return false; }
		// else
		return true;
	}
	bool operator == ( const Sphere	&L, const Sphere	&R )
	{
		if ( ! ( L.pos  - R.pos  ).IsZero() )		{ return false; }
		if ( !ZeroEqual( L.radius - R.radius ) )	{ return false; }
		if ( L.exist != R.exist )					{ return false; }
		// else
		return true;
	}
	bool operator == ( const OBB	&L, const OBB		&R )
	{
		if ( !( L.pos   - R.pos   ).IsZero() ) { return false; }
		if ( !( L.size - R.size ).IsZero() ) { return false; }
		if ( !L.orientation.IsSameRotation( R.orientation ) ) { return false; }
		if ( L.exist != R.exist ) { return false; }
		// else
		return true;
	}
}
