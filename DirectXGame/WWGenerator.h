#pragma once
#include <Render/RenderObject.h>
#include <vector>
#include <Game/NewMap/NewMap.h>

class WWGenerator {
public:

	void Initialize();
	void Update();
	void Draw();

	void Generate(std::vector<std::vector<TileType>> mapData);

private:

};
