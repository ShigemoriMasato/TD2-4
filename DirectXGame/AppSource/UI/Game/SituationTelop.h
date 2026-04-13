#pragma once
#include <Render/RenderObject.h>
#include <assets/Model/ModelManager.h>
#include <Render/Font/Text.h>
#include <Common/KeyConfig/KeyManager.h>
#include <GameObject/EasingAnimation/AnimationBundle.h>

class SituationTelop {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, int textureIndex);
	void Update(Matrix4x4 vpMatrix, std::unordered_map<Key, bool> key, float deltaTime);
	void Draw(CmdObj* cmdObj);

	void StartAnimation(const std::wstring& telopName);

private:
	enum class State{
		None,
		FadeIn,
		Hold,
		FadeOut
	};

private:
	std::unique_ptr<SHEngine::Text> telopText_;

	Transform transform_;
	int textureIndex_ = 0;

	AnimationBundle<float> alphaAnime_;
	AnimationBundle<float> scaleAnime_;

	std::unordered_map<std::wstring, Vector4> colors_; 
	Vector4 color_;

	State state_ = State::None;
	float holdTimer_ = 0.0f;

	Vector3 basePosition_;
	float nameSize_;
};
