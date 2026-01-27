#pragma once
#include <Scene/IScene.h>
#include <GameCamera/CameraController.h>
#include "Render/DebugMCRender.h"
#include "Render/MapRender.h"
#include "Editor/Type/MapTypeEditor.h"
#include "Editor/Texture/MapTextureEditor.h"

class ModelEditScene : public IScene {
public:

	~ModelEditScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	MapChipData mcData_;

	std::unique_ptr<CameraController> cameraController_ = nullptr;
	std::unique_ptr<DebugMCRender> mcRender_ = nullptr;
	std::unique_ptr<MapRender> mapRender_ = nullptr;

	std::unique_ptr<MapTypeEditor> typeEditor_ = nullptr;
	std::unique_ptr<MapTextureEditor> textureEditor_ = nullptr;
};
