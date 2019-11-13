#pragma once

namespace StorageForScene
{
	struct Storage
	{
	public:
		int stageNo{};
	};

	void ResetStorage();
	void SetStorage( const Storage &newData );
	const Storage *GetStorage();
}
