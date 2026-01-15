#pragma once
#include <Scene/IScene.h>
#include "Editor/MapEditor.h"

class MapEditScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<MapEditor::Editor> editor_ = nullptr;

};
