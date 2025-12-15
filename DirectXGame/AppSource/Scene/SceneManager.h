#pragma once
#include "IScene.h"

class SceneManager {
public:

	SceneManager() = default;
	~SceneManager() = default;

	void Initialize();
	void Update();
	void Draw();

	CommonData* GetCommonData() const { return commonData_.get(); }

private:

	std::unique_ptr<CommonData> commonData_;

	std::unique_ptr<IScene> currentScene_;
	std::unique_ptr<IScene> nextScene_;

};
