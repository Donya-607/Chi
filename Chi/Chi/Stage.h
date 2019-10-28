#pragma once

#include <memory>

#include "Donya/Vector.h"

class skinned_mesh;	// With pointer. because I'm not want include this at header.
class Stage
{
private:
	int stageNo;	// 1-based.
	std::unique_ptr<skinned_mesh> pModel;
public:
	Stage();
	~Stage();
public:
	void Init( int stageNo );
	void Uninit();

	void Update();

	void Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
private:
	void LoadModel();
};
