#pragma once
#include <Assets/Fonts/FontLoader.h>
#include <Assets/Model/ModelManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Core/Command/CommandManager.h>
#include <Core/DXDevice.h>
#include <Core/ImGuiWrapper.h>
#include <Input/Input.h>
#include <Render/Screen/SwapChain.h>
#include <Render/Screen/WindowsAPI.h>
#include <Tool/FPS/FPSObserver.h>
#include <Utility/DirectUtilFuncs.h>
#include <Render/PSO/PSOEditor.h>
#include <Compute/PSO/CSPSOManager.h>

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
	// コマンドの実行
	void EndFrame();

	// コマンドの実行(Signalも送る)
	void ExecuteCommand(Command::Type type, int index = 0) {
		cmdManager_->Execute(type, index);
		cmdManager_->SendSignal(type, index);
	}

	// ImGuiの有効化
	void ImGuiActivate(Screen::WindowsAPI* window, Command::Object* cmdObj) {
		imGuiWrapper_ = std::make_unique<ImGuiWrapper>();
		imGuiWrapper_->Initialize(device_.get(), cmdManager_.get(), window, cmdObj);
		imGuiWrapper_->NewFrame();
	}

	// ImGuiの描画
	void DrawImGui() {
		if (imGuiWrapper_) {
			imGuiWrapper_->Render();
		}
	}

public: // Getter
	TextureManager* GetTextureManager() { return textureManager_.get(); }
	FontLoader* GetFontLoader() { return fontLoader_.get(); }
	ModelManager* GetModelManager() { return modelManager_.get(); }
	DrawDataManager* GetDrawDataManager() { return drawDataManager_.get(); }
	Input* GetInput() { return input_.get(); }
	FPSObserver* GetFPSObserver() { return fpsObserver_.get(); }
	float GetDeltaTime() { return fpsObserver_->GetDeltatime(); }

	std::unique_ptr<Command::Object> CreateCommandObject(Command::Type type, int index = 0, int listNum = 3) {
		auto cmdObj = cmdManager_->CreateCommandObject(type, index, listNum);
		return cmdObj;
	}

	std::unique_ptr<Screen::SwapChain> MakeWindow(Screen::WindowsAPI* windowsApi, uint32_t clearColor);

	HINSTANCE GetHInstance() const { return hInstance_; }

private: // Engine内で完結するクラス
	std::unique_ptr<DXDevice> device_;
	std::unique_ptr<Command::Manager> cmdManager_;
	std::unique_ptr<ImGuiWrapper> imGuiWrapper_;
	std::unique_ptr<PSO::Editor> psoEditor_;
	std::unique_ptr<PSO::CSPSOManager> csPsoManager_;

private: // Engine外部からアクセスするクラス
	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<FontLoader> fontLoader_;
	std::unique_ptr<ModelManager> modelManager_;
	std::unique_ptr<DrawDataManager> drawDataManager_;

	std::unique_ptr<Input> input_;
	std::unique_ptr<FPSObserver> fpsObserver_;

private: // その他系
	HINSTANCE hInstance_;
	MSG msg_{};
	bool imguiDrawed_ = true;
	Logger logger_;
};

} // namespace SHEngine

struct AABB
{
	Vector3 min;
	Vector3 max;


	// ローカル AABB → ワールド AABB
	AABB Transform(const Matrix4x4& m) const
	{
		Vector3 corners[8] = {
			{ min.x, min.y, min.z },
			{ max.x, min.y, min.z },
			{ min.x, max.y, min.z },
			{ max.x, max.y, min.z },
			{ min.x, min.y, max.z },
			{ max.x, min.y, max.z },
			{ min.x, max.y, max.z },
			{ max.x, max.y, max.z }
		};

		AABB transformedAABB;
		transformedAABB.min = { FLT_MAX,  FLT_MAX,  FLT_MAX };
		transformedAABB.max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (int i = 0; i < 8; ++i)
		{
			Vector3 p = m * corners[i];

			transformedAABB.min.x = std::min(transformedAABB.min.x, p.x);
			transformedAABB.min.y = std::min(transformedAABB.min.y, p.y);
			transformedAABB.min.z = std::min(transformedAABB.min.z, p.z);

			transformedAABB.max.x = std::max(transformedAABB.max.x, p.x);
			transformedAABB.max.y = std::max(transformedAABB.max.y, p.y);
			transformedAABB.max.z = std::max(transformedAABB.max.z, p.z);
		}

		return transformedAABB;
	}
};

