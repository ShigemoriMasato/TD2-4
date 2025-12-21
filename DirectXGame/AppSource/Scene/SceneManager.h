#pragma once
#include "IScene.h"

class SceneManager {
public:

	SceneManager() = default;
	~SceneManager() = default;

	void Initialize(SHEngine* engine);
	void Update();
	void Draw();

	CommonData* GetCommonData() const { return commonData_.get(); }

private:

	void SwapScene();

private:

	SHEngine* engine_;

	std::unique_ptr<CommonData> commonData_;

	std::unique_ptr<IScene> currentScene_;
	std::unique_ptr<IScene> nextScene_;
};
