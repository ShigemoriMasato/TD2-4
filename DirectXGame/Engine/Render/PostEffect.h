#pragma once
#include <Render/RenderObject.h>
#include <Screen/DualDisplay.h>

enum class PostEffectJob : uint32_t {
	None = 0,
	Bloom = 1 << 0,
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
	DualDisplay* origin = nullptr;
	DualDisplay* output = nullptr;	//nullptrの場合はoriginに描画する

	uint32_t jobs_ = 0;
};

class PostEffect {
public:

	void Initialize(TextureManager* textureManager);
	void Draw(const PostEffectConfig& config);

private:

	std::unique_ptr<Display> intermediateDisplay_ = nullptr;
	std::map<PostEffectJob, std::unique_ptr<RenderObject>> postEffectObjects_{};

};
