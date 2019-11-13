#include "StorageForScene.h"

namespace StorageForScene
{
	static Storage instance{};

	void ResetStorage()
	{
		instance.stageNo = 0;
	}
	void SetStorage( const Storage &newData )
	{
		instance = newData;
	}
	const Storage *GetStorage()
	{
		return &instance;
	}
}
