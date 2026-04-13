#pragma once
#include <Scene/IScene.h>
#include <UI/Title/TitleUI.h>
#include <Render/PostEffect.h>
#include <Camera/Camera.h>
#include <memory>

class TitleScene : public IScene {
public:

	TitleScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<TitleUI> titleUI_;
	std::unique_ptr<Camera> camera_;

	std::unique_ptr<PostEffect> postEffect_;
	PostEffectConfig postEffectConfig_;

	//@brief オプションで使用する音量の変数(min 0.0, max 1.0)
	// CommonDataから読み込み、変更をCommonDataに反映する
	float* masterVolume_ = nullptr;
	float* bgmVolume_ = nullptr;
	float* seVolume_ = nullptr;

	//@brief 計算済みの音量（再生時に使用）
	float calculatedBgmVolume_ = 0.5f;
	float calculatedSeVolume_ = 0.5f;

	//@brief オプション選択中かどうか
	bool isOptionMode_ = false;

	//@brief 音量を再計算する
	void UpdateCalculatedVolumes();

	Option::Select currentOptionSelect_ = Option::Select::Master;
};