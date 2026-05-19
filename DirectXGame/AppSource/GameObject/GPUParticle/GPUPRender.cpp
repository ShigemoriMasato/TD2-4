#include "GPUPRender.h"

GPUP::RenderBufferData GPUP::Render::Initialize(uint32_t num, const SHEngine::DrawData& dd) {
	render_ = std::make_unique<SHEngine::Renderer>(dd);
	container_ = std::make_unique<SHEngine::BufferContainer>();

	return {};
}
