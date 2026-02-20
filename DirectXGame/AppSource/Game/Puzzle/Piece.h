#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Game/Item/Item.h>

class Piece {
public:

	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager);
	void SetItem(const Item& item);

private:

	Item item_;
	Vector3 position_;


};
