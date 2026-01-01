#pragma once
#include "CommonData.h"
#include <memory>

class IScene {
public:

	IScene() = default;
	virtual ~IScene() = default;

	virtual void Initialize() = 0;
	virtual std::unique_ptr<IScene> Update() = 0;
	virtual void Draw() = 0;

	void SwapScene(CommonData* commonData, SHEngine* engine);

protected:

	CommonData* commonData_;
	SHEngine* engine_ = nullptr;

	TextureManager* textureManager_ = nullptr;
	WindowMaker* windowMaker_ = nullptr;
	DrawDataManager* drawDataManager_ = nullptr;
	Input* input_ = nullptr;

};
