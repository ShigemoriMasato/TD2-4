#pragma once
#include <Render/RenderObject.h>
#include <Screen/DualDisplay.h>
#include "PostEffectData.h"

enum class PostEffectJob : uint32_t {
	None = 0,
	Blur = 1 << 0,
	Fade = 1 << 1,
	Glitch = 1 << 2,
	GrayScale = 1 << 3,
	GridTransition = 1 << 4,
	SlowMotion = 1 << 5,
};

uint32_t operator|(PostEffectJob a, PostEffectJob b);
uint32_t operator|(uint32_t a, PostEffectJob b);
uint32_t operator&(uint32_t a, PostEffectJob b);
uint32_t operator~(PostEffectJob a);
bool operator<(PostEffectJob a, PostEffectJob b);

struct PostEffectConfig {
	Window* window = nullptr;
	std::pair<IDisplay*, bool> origin = { nullptr, false };
	std::pair<IDisplay*, bool> output = { nullptr, false };	//nullptrの場合はoriginに描画する

	uint32_t jobs_ = 0;

};

class PostEffect {
public:

	void Initialize(TextureManager* textureManager, DrawData& drawData);
	template<typename T>
	void CopyBuffer(PostEffectJob job, const T& data);
	void Draw(PostEffectConfig config);

private:

	std::unique_ptr<DualDisplay> intermediateDisplay_ = nullptr;
	std::map<PostEffectJob, std::unique_ptr<RenderObject>> postEffectObjects_{};

};

template<typename T>
inline void PostEffect::CopyBuffer(PostEffectJob job, const T& data) {
	auto& postEffectObject = postEffectObjects_.at(job);
	postEffectObject->CopyBufferData(1, &data, sizeof(T));
}
