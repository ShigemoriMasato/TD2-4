#pragma once
#include <Core/DXDevice.h>
#include <Core/Command/CommandManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Fonts/FontLoader.h>
#include <Assets/Model/ModelManager.h>
#include <Input/Input.h>
#include <Tool/FPS/FPSObserver.h>

namespace SHEngine {

	class Engine {
	public:

		void Initialize(HINSTANCE hInstance);

		// エンジン側の終了命令
		bool IsLoop();

		// Inputとコマンドの更新
		void BeginFrame();
		// コマンドのクローズ
		void PostDraw();
		//コマンドの実行
		void EndFrame();

	public://Getter

		TextureManager* GetTextureManager() { return textureManager_.get(); }
		FontLoader* GetFontLoader() { return fontLoader_.get(); }
		ModelManager* GetModelManager() { return modelManager_.get(); }
		DrawDataManager* GetDrawDataManager() { return drawDataManager_.get(); }
		Input* GetInput() { return input_.get(); }

	private:// Engine内で完結するクラス

		std::unique_ptr<DXDevice> device_;
		std::unique_ptr<Command::Manager> cmdManager_;

	private:// Engine外部からアクセスするクラス

		std::unique_ptr<TextureManager> textureManager_;
		std::unique_ptr<FontLoader> fontLoader_;
		std::unique_ptr<ModelManager> modelManager_;
		std::unique_ptr<DrawDataManager> drawDataManager_;
		
		std::unique_ptr<Input> input_;
		
	private: // その他系
		MSG msg_{};
		Logger logger_;
	};

}
