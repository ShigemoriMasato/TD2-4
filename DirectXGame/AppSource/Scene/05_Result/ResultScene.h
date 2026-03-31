#pragma once
#include <Scene/IScene.h>
#include <Render/Font/Text.h>
#include <Render/PostEffect.h>

class ResultScene : public IScene {
public:

	ResultScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	std::unique_ptr<SHEngine::Text> clearText_;
	std::unique_ptr<SHEngine::Text> gameOverText_;
	std::unique_ptr<SHEngine::Text> CorrectText_;

	std::unique_ptr<Camera> orthoCamera_;

	Transform clearTextTransform_{};
	Transform gameOverTextTransform_{};
	Transform correctTextTransform_{};

	bool isWin_ = false;

	std::unique_ptr<PostEffect> postEffect_;
	PostEffectConfig postEffectConfig_;

};