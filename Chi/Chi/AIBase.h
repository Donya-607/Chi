#pragma once

#include <string>
#include <typeinfo>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

#include "Donya/Serializer.h"
#include "Donya/UseImGui.h"

struct ActionStorage
{
	int waitNo{};		// -1:random.
	int nextAttackNo{};
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
			CEREAL_NVP( waitNo ),
			CEREAL_NVP( nextAttackNo )
		);

		if ( 1 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
#if USE_IMGUI

public:
	/// <summary>
	/// Showing all members to ImGui::TreeNode().
	/// </summary>
	void ShowImGuiNode( const std::string &prefix );
#endif // USE_IMGUI

public:
	static ActionStorage Zero()
	{
		return ActionStorage{ 0, 0 };
	}
};
CEREAL_CLASS_VERSION( ActionStorage, 0 )

class LotteryBase
{
public:
	virtual void Init() = 0;
	virtual void Uninit() = 0;
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{

		// No op.
	}
public:
	virtual ActionStorage Lottery( float normalizedDistance ) = 0;

	virtual void LoadParameter( bool isBinary = 0 ) = 0;

#if USE_IMGUI

	virtual void SaveParameter() = 0;

	/// <summary>
	/// Showing all members to ImGui::TreeNode().
	/// </summary>
	virtual void ShowImGuiNode( const std::string &prefix, bool isAllowUnitIO = false ) = 0;

#endif // USE_IMGUI

};

#if USE_IMGUI

void ShowImGuiChooserNode( const std::string &prefix, std::unique_ptr<LotteryBase> *ppChooserBase, int *pNowKind );

#endif // USE_IMGUI

enum class ChooserKind
{
	Distance = 0,
	Fixed,

	CHOOSERS_COUNT
};
void ResetLotteryKind( std::unique_ptr<LotteryBase> *ppChooserBase, int chooserKind );
void ResetLotteryKind( std::unique_ptr<LotteryBase> *ppChooserBase, ChooserKind chooserKind );

/// <summary>
/// Chooser number 0.
/// </summary>
class DistanceLottery : public LotteryBase
{
public:
	struct Content
	{
		float			distance{};	// 0.0f ~ 1.0f. Enable this when the passed normalized distance is less-equal than "distance".
		ActionStorage	action{};
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( distance ),
				CEREAL_NVP( action )
			);

			if ( 1 <= version )
			{
				// archive( CEREAL_NVP( x ) );
			}
		}
	};
private:
	std::vector<Content> data;
public:
	DistanceLottery();
	~DistanceLottery();
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive( CEREAL_NVP( data ) );

		if ( 1 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "DistanceLottery";
public:
	void Init() override;
	void Uninit() override;

	ActionStorage Lottery( float normalizedDistance ) override;

	void LoadParameter( bool isBinary = 0 ) override;

#if USE_IMGUI

	void SaveParameter() override;

	/// <summary>
	/// Showing all members to ImGui::TreeNode().
	/// </summary>
	void ShowImGuiNode( const std::string &prefix, bool isAllowUnitIO = false ) override;

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( DistanceLottery, 0 )
CEREAL_CLASS_VERSION( DistanceLottery::Content, 0 )
CEREAL_REGISTER_TYPE( DistanceLottery )
CEREAL_REGISTER_POLYMORPHIC_RELATION( LotteryBase, DistanceLottery )

/// <summary>
/// Chooser number 1.
/// </summary>
class FixedLottery : public LotteryBase
{
private:
	ActionStorage action;
public:
	FixedLottery();
	~FixedLottery();
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive( CEREAL_NVP( action ) );

		if ( 1 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "FixedLottery";
public:
	void Init() override;
	void Uninit() override;

	ActionStorage Lottery( float normalizedDistance ) override;

	void LoadParameter( bool isBinary = 0 ) override;

#if USE_IMGUI

	void SaveParameter() override;

	/// <summary>
	/// Showing all members to ImGui::TreeNode().
	/// </summary>
	void ShowImGuiNode( const std::string &prefix, bool isAllowUnitIO = false ) override;

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( FixedLottery, 0 )
CEREAL_REGISTER_TYPE( FixedLottery )
CEREAL_REGISTER_POLYMORPHIC_RELATION( LotteryBase, FixedLottery )
