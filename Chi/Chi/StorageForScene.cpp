#include "StorageForScene.h"

namespace StorageForScene
{
	static Storage instance{};

	void SetStorage( const Storage &newData )
	{
		instance = newData;
	}
	const Storage *GetStorage()
	{
		return &instance;
	}
}
