#pragma once
#include <Core/DXDevice.h>
#include <Core/Command/CommandManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Fonts/FontLoader.h>
#include <Assets/Model/ModelManager.h>
#include <Input/Input.h>
#include <Render/Screen/SwapChain.h>
#include <Render/Screen/WindowsAPI.h>
#include <Core/ImGuiWrapper.h>

namespace SHEngine {

	class Engine {
	public:

		~Engine();

		void Initialize(HINSTANCE hInstance);

		// エンジン側の終了命令
		bool IsLoop();

		// Inputとコマンドの更新
		void BeginFrame();
		// コマンドのクローズ
		void PostDraw();
		//コマンドの実行
		void EndFrame();

		// ImGuiの有効化
		void ImGuiActivate(Screen::WindowsAPI* window) {
			imGuiWrapper_->Initialize(device_.get(), cmdManager_.get(), window);
		}

	public://Getter

		TextureManager* GetTextureManager() { return textureManager_.get(); }
		FontLoader* GetFontLoader() { return fontLoader_.get(); }
		ModelManager* GetModelManager() { return modelManager_.get(); }
		DrawDataManager* GetDrawDataManager() { return drawDataManager_.get(); }
		Input* GetInput() { return input_.get(); }

		std::unique_ptr<Command::Object> CreateCommandObject(Command::Type type, int index = 0, int listNum = 3) {
			auto cmdObj = cmdManager_->CreateCommandObject(type, index, listNum);
			return cmdObj;
		}

		std::unique_ptr<Screen::SwapChain> MakeWindow(Screen::WindowsAPI* windowsApi, uint32_t clearColor);

		HINSTANCE GetHInstance() const { return hInstance_; }

	private:// Engine内で完結するクラス

		std::unique_ptr<DXDevice> device_;
		std::unique_ptr<Command::Manager> cmdManager_;
		std::unique_ptr<ImGuiWrapper> imGuiWrapper_;

	private:// Engine外部からアクセスするクラス

		std::unique_ptr<TextureManager> textureManager_;
		std::unique_ptr<FontLoader> fontLoader_;
		std::unique_ptr<ModelManager> modelManager_;
		std::unique_ptr<DrawDataManager> drawDataManager_;
		
		std::unique_ptr<Input> input_;
		
	private: // その他系
		HINSTANCE hInstance_;
		MSG msg_{};
		Logger logger_;
	};

}
