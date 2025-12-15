#include "../IScene.h"

void IScene::SwapScene(CommonData* commonData, SHEngine* engine) {
	commonData_ = commonData;
	engine_ = engine;
}
