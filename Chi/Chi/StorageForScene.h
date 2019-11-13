#pragma once

namespace StorageForScene
{
	struct Storage
	{
	public:
		int stageNo{};
	};

	void SetStorage( const Storage &newData );
	const Storage *GetStorage();
}
