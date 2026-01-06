#pragma once
#include <Scene/IScene.h>
#include <Game/Tetris/Tetris.h>
#include <Camera/DebugCamera.h>
#include <Render/PostEffect.h>
#include <Game/Effect/BackGround.h>
#include <Game/Camera/GameCamera.h>
#include <Game/Effect/FallLight.h>

class GameScene : public IScene {
public:

	GameScene();
	~GameScene() override = default;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	Camera* worldCamera_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
	std::unique_ptr<Camera> manualCamera_ = nullptr;
	std::unique_ptr<GameCamera> gameCamera_ = nullptr;

	std::unique_ptr<KeyCoating> keyCoating_;
	std::unique_ptr<Tetris> tetris_;
	std::unique_ptr<PostEffect> postEffect_;
	std::unique_ptr<BackGround> backGround_;
	std::unique_ptr<FallLight> fallLight_;

	std::unique_ptr<RenderObject> gameOverText = nullptr;
	std::unique_ptr<RenderObject> gameMonitor_ = nullptr;

	BackGround::Wave weakWave_;

	PostEffectConfig postEffectConfig_{};

};