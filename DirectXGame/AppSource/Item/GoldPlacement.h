#pragma once
#include "OreItemManager.h"
#include <Game/MapData/MapDataManager.h>

class GoldPlacement {
public:

	void Initialize(OreItemManager* oreItemManager, MapDataManager);
	void Execute(const StageData& stage);

private:

};
