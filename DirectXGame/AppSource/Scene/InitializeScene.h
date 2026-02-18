#pragma once
#include "IScene.h"

class InitializeScene final : public IScene {
public:

	InitializeScene() = default;
	~InitializeScene() = default;
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:



};
