#include "../IScene.h"

void IScene::SwapScene(CommonData* commonData, SHEngine* engine) {
	commonData_ = commonData;
	engine_ = engine;

	textureManager_ = engine_->GetTextureManager();
	modelManager_ = engine_->GetModelManager();
	windowMaker_ = engine_->GetWindowMaker();
	drawDataManager_ = engine_->GetDrawDataManager();
	input_ = engine_->GetInput();
}
