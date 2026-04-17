#pragma once
#include "GPUBuffer.h"

namespace SHEngine {

	// @brief GPUBuffer用のクラス。作成と寿命管理を行う。GPUBufferの作成関数やemplace_back等で1行が長くなるので、その解決用として存在している。
	class BufferContainer {
	public:

		BufferContainer() = default;

		GPUBuffer* Create(BufferType bufferType, size_t size, uint32_t num = 1, uint32_t bufferNum = 3) {
			buffers_.emplace_back(std::make_unique<GPUBuffer>(bufferType, size, num, bufferNum));
			return buffers_.back().get();
		}

		void Copy(uint32_t index, const void* data, size_t dataSize) {
			assert(index < buffers_.size());
			buffers_[index]->CopyBuffer(data, dataSize);
		}

	private:

		std::vector<std::unique_ptr<GPUBuffer>> buffers_;

	};

}
