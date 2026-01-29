#pragma once
#include <Scene/IScene.h>
#include <GameCamera/CameraController.h>
#include "Render/DebugMCRender.h"
#include "Render/MapRender.h"
#include "Render/StaticObjectRender.h"
#include "Editor/Type/MapTypeEditor.h"
#include "Editor/Texture/MapTextureEditor.h"
#include "Editor/Decoration/MapDecorationEditor.h"
#include "Editor/Stage/StageEditor.h"

class ModelEditScene : public IScene {
public:

	~ModelEditScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	int currentStage_ = 0;

	std::unique_ptr<CameraController> cameraController_ = nullptr;
	std::unique_ptr<DebugMCRender> mcRender_ = nullptr;
	std::unique_ptr<MapRender> mapRender_ = nullptr;
	std::unique_ptr<StaticObjectRender> staticObjectRender_ = nullptr;

	std::unique_ptr<MapTypeEditor> typeEditor_ = nullptr;
	std::unique_ptr<MapTextureEditor> textureEditor_ = nullptr;
	std::unique_ptr<MapDecorationEditor> decorationEditor_ = nullptr;
	std::unique_ptr<StageEditor> stageEditor_ = nullptr;

	//マップチップorオブジェクト
	bool whichEditMode_ = true;
	//Stageを切り替えた瞬間
	bool stageChanged_ = false;
};
