#pragma once
#include <Render/Renderer.h>
#include <Camera/Camera.h>

namespace GPUP {

	struct RenderBufferData {
		SHEngine::GPUBuffer* positions_;
		SHEngine::GPUBuffer* colors_;
	};

	class Render {
	public:

		RenderBufferData Initialize(uint32_t num, const SHEngine::DrawData& dd);
		void Update(Camera* camera);
		void Draw(CmdObj* cmdObj);

		RenderBufferData GetRenderBufferData() const { return data_; }

	private:

		std::unique_ptr<SHEngine::BufferContainer> container_;

		std::unique_ptr<SHEngine::Renderer> render_;
		RenderBufferData data_;

	};

}
