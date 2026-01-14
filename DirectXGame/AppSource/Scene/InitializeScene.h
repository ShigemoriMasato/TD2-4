#pragma once
#include "IScene.h"
class InitializeScene : public IScene {
public:

	InitializeScene() = default;
	~InitializeScene() = default;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	//普段描画するメインのDisplayやWindowの作成
	void CreateDisplay();

	//InputにWindowを割り当てたり、目標FPSを設定したりする
	void EngineSetup();

	//KeyConfigの設定
	void SetupKeyManager();
};
