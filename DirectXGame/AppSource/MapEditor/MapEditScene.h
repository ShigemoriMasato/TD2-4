#pragma once
#include <Scene/IScene.h>
#include "Editor/MapEditor.h"
#include "MapModelEdit/MapModelEditor.h"

class MapEditScene : public IScene {
public:

	~MapEditScene() override;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<MapEditor::Editor> mapEditor_ = nullptr;
	std::unique_ptr<MapModelEditor> mapModelEditor_ = nullptr;

	int type_ = 1;

};
