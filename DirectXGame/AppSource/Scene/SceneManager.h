#pragma once
#include "IScene.h"

class SceneManager {
public:

	SceneManager() = default;
	~SceneManager();

	void Initialize(SHEngine* engine);
	bool IsHasNextScene() const { return nextScene_ != nullptr; };
	void SwapScene();
	void Update();
	void Draw();

	CommonData* GetCommonData() const { return commonData_.get(); }

private:

	SHEngine* engine_;

	std::unique_ptr<CommonData> commonData_;

	std::unique_ptr<IScene> currentScene_;
	std::unique_ptr<IScene> nextScene_;
};
