#pragma once

struct StorageForScene
{
public:
	int stageNo{};
};

void SetStorage( const StorageForScene &newData );
const StorageForScene *GetStorage();
