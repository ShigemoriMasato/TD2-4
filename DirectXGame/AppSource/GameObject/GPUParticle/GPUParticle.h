#pragma once
#include <Compute/ComputeObject.h>
#include <SHEngine.h>
#include "GPUPRender.h"

namespace GPUP {

	class Controller {
	public:

		void Initialize(uint32_t maxNum, SHEngine::Engine* engine);
		void Draw();

	private:

		std::unique_ptr<CmdObj> compute_;
		std::unique_ptr<CmdObj> direct_;

		std::unique_ptr<Render> render_;

	};

}
