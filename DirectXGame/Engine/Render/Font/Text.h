#pragma once
#include <Assets/Fonts/FontLoader.h>
#include "../RenderObject.h"

namespace SHEngine {

	class Text {
	public:

		static void SetFontLoader(FontLoader* fontLoader) { fontLoader_ = fontLoader; }

		void Initialize(const std::string& fontPath, int fontSize = 64);
		void Initialize(int fontID);

		void SetText(const std::wstring& text);

	private:

		static inline FontLoader* fontLoader_ = nullptr;

		int textureIndex_ = -1;
		int fontSize_ = 0;

		std::unique_ptr<RenderObject> render_ = nullptr;
		std::vector<CharPosition> charPositions_{};

		Matrix4x4 charSizeMat_ = Matrix4x4::Identity();
		Matrix4x4 wvpMat_ = Matrix4x4::Identity();

	};

}
