#pragma once
#include <Utility/Vector.h>
#include <vector>

namespace MapEditor {

	class Pallet {
	public:

		void Initialize();
		void DrawImGui();

	private:

		std::vector<Vector4> colors_;
		int currentColorIndex_ = 0;

	};

}
