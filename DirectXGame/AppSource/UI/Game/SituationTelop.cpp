#include "SituationTelop.h"
#include <imgui/imgui.h>
#include <numbers>

using namespace SHEngine;

void SituationTelop::Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, int textureIndex) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);

	colors_[L"ピンチ"] = {1.0f, 0.0f, 0.0f, 1.0f};
	colors_[L"アドバンテージ"] = {1.0f, 1.0f, 0.0f, 1.0f};

	telopText_ = std::make_unique<Text>();
	telopText_->Initialize(data, "YDWbananaslipplus.otf", 64, "Telop");
}

void SituationTelop::Update(Matrix4x4 vpMatrix, std::unordered_map<Key, bool> key, float deltaTime) {
#ifdef USE_IMGUI
	ImGui::Begin("Telop");
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &transform_.position.x, 0.01f);
	ImGui::End();
#endif

	bool playingAlpha = alphaAnime_.anim.Update(deltaTime, alphaAnime_.temp);
	bool playingScale = scaleAnime_.anim.Update(deltaTime, scaleAnime_.temp);

	switch (state_) {
	case State::FadeIn:
		telopText_->SetColor({color_.x, color_.y, color_.z, alphaAnime_.temp});
		transform_.scale.x = scaleAnime_.temp;

		{
			float baseScale = 3.0f;
			float currentScale = scaleAnime_.temp;

			Vector2 size = {(64.0f * nameSize_) / 2.0f, 64.0f};
			float diff = currentScale - baseScale;

			Vector3 offset = {-size.x * diff * 0.5f, -size.y * diff * 0.5f, 0.0f};

			transform_.position = basePosition_ + offset;
		}

		if (!playingAlpha && !playingScale) {
			state_ = State::Hold;
			holdTimer_ = 0.0f;
		}
		break;

	case State::Hold:
		holdTimer_ += deltaTime;
		if (holdTimer_ >= 1.0f) {
			alphaAnime_.anim.Start(1.0f, 0.0f, 0.5f, EaseType::EaseOutCubic);
			scaleAnime_.anim.Start(3.0f, 4.0f, 0.5f, EaseType::EaseOutCubic);
			state_ = State::FadeOut;
		}
		break;

	case State::FadeOut:
		telopText_->SetColor({color_.x, color_.y, color_.z, alphaAnime_.temp});
		transform_.scale.x = scaleAnime_.temp;

		{
			float baseScale = 3.0f;
			float currentScale = scaleAnime_.temp;

			Vector2 size = {(64.0f * nameSize_) / 2.0f, 64.0f};
			float diff = currentScale - baseScale;

			Vector3 offset = {-size.x * diff * 0.5f, -size.y * diff * 0.5f, 0.0f};

			transform_.position = basePosition_ + offset;
		}

		if (!playingAlpha && !playingScale) {
			state_ = State::None;
			transform_.scale = {3.0f, 3.0f, 1.0f};
			telopText_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		}
		break;

	case State::None:
	default:
		break;
	}

	telopText_->Update(vpMatrix);
	telopText_->SetTransform(transform_);
}

void SituationTelop::Draw(CmdObj* cmdObj) {
	if (state_ != State::None) {
		telopText_->Draw(cmdObj);
	}
}

void SituationTelop::StartAnimation(const std::wstring& telopName) {
	telopText_->SetText(telopName + L"!");
	color_ = colors_[telopName];

	transform_.scale = {4.0f, 4.0f, 1.0f};
	telopText_->SetColor({color_.x, color_.y, color_.z, 1.0f});

	alphaAnime_.anim.Start(0.0f, 1.0f, 0.2f, EaseType::EaseOutCubic);
	scaleAnime_.anim.Start(4.0f, 3.0f, 0.2f, EaseType::EaseOutCubic);

	state_ = State::FadeIn;
	holdTimer_ = 0.0f;

	if (telopName == L"ピンチ") {
		basePosition_ = Vector3(490.0f, -380.0f, 0.0f);
	} else {
		basePosition_ = Vector3(340.0f, -380.0f, 0.0f);
	}

	transform_.position = basePosition_;
	nameSize_ = static_cast<float>(telopName.size());
}
