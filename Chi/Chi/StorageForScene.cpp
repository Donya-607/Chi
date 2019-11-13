#include "StorageForScene.h"

static StorageForScene instance{};

void SetStorage( const StorageForScene &newData )
{
	instance = newData;
}
const StorageForScene *GetStorage()
{
	return &instance;
}
