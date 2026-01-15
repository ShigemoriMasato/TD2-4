#pragma once
#include <Core/DXDevice.h>
#include <Core/FenceManager.h>
#include <Core/ImGuiforEngine.h>
#include <Utility/DirectUtilFuncs.h>
#include <Screen/Window.h>
#include <Screen/WindowMaker.h>
#include <Render/DrawDataManager.h>
#include <Input/Input.h>
#include <Tool/FPS/FPSObserver.h>
#include <Assets/Model/ModelManager.h>
#include <memory>

/**
 * @class SHEngine
 * @brief エンジンの中核となるクラス。全体の初期化、更新、描画フローを管理する
 * 
 * DirectX12を使用したゲームエンジンのメインクラス。
 * デバイス、コマンドリスト、ウィンドウ、入力、FPS管理などの各サブシステムを統括し、
 * ゲームループの実行とリソースのライフサイクル管理を担当する。
 */
class SHEngine {
public:

	/// @brief コンストラクタ。各サブシステムの初期化を行う
	SHEngine();
	
	/// @brief デストラクタ。GPU処理の完了を待機し、リソースを解放する
	~SHEngine();

	/**
	 * @brief エンジンの初期化処理
	 * @param hInstance アプリケーションのインスタンスハンドル
	 */
	void Initialize(HINSTANCE hInstance);

	/**
	 * @brief ゲームループを継続するかどうかを判定
	 * @return true: ループ継続、false: ループ終了
	 */
	bool IsLoop();
	
	/// @brief フレーム更新処理
	void Update();
	
	/**
	 * @brief 描画前処理。GPUの同期待ち、コマンドリストのリセット、テクスチャのアップロードを行う
	 * @return true: 描画可能、false: 描画不可
	 */
	bool PreDraw();
	
	/// @brief フレーム終了処理。描画コマンドの実行、Present、メッセージ処理を行う
	void EndFrame();

	/// @brief GPUの処理完了を待機する
	void WaitForGPU();

	/**
	 * @brief ImGuiを有効化する
	 * @param window ImGuiを描画するウィンドウ
	 */
	void ImGuiActivate(Window* window);
	
	/// @brief ImGuiの描画処理
	void ImGuiDraw();

	/// @brief テクスチャマネージャーを取得
	/// @return TextureManagerへのポインタ
	TextureManager* GetTextureManager() { return textureManager_.get(); }
	
	/// @brief モデルマネージャーを取得
	/// @return ModelManagerへのポインタ
	ModelManager* GetModelManager() { return modelManager_.get(); }
	
	/// @brief ウィンドウメーカーを取得
	/// @return WindowMakerへのポインタ
	WindowMaker* GetWindowMaker() { return windowMaker_.get(); }
	
	/// @brief 描画データマネージャーを取得
	/// @return DrawDataManagerへのポインタ
	DrawDataManager* GetDrawDataManager() { return drawDataManager_.get(); }
	
	/// @brief 入力マネージャーを取得
	/// @return Inputへのポインタ
	Input* GetInput() { return input_.get(); }
	
	/// @brief FPS監視クラスを取得
	/// @return FPSObserverへのポインタ
	FPSObserver* GetFPSObserver() { return fpsObserver_.get(); }


private:

	/// @brief Windowsメッセージを処理する
	void ExecuteMessage();
	
	/// @brief FPS情報をImGuiに描画する
	void FPSDraw();

private:

	//LeakChecker
	/// @brief DirectX12リソースのリーク検出用
	D3DResourceLeakChecker leakChecker_{};

	//最後に削除する
	/// @brief DirectX12デバイス（最後に削除する必要がある）
	std::unique_ptr<DXDevice> dxDevice_ = nullptr;

private:

	//Engine側ツール
	/// @brief コマンドリスト管理
	std::unique_ptr<CmdListManager> cmdListManager_ = nullptr;
	/// @brief フェンス管理（GPU同期）
	std::unique_ptr<FenceManager> fenceManager_ = nullptr;
	/// @brief ImGui管理
	std::unique_ptr<ImGuiforEngine> imGuiForEngine_ = nullptr;
	/// @brief 入力が有効かどうか
	bool inputActive_ = false;

	//App側ツール
	/// @brief テクスチャ管理
	std::unique_ptr<TextureManager> textureManager_ = nullptr;
	/// @brief モデル管理
	std::unique_ptr<ModelManager> modelManager_ = nullptr;
	/// @brief 描画データ管理
	std::unique_ptr<DrawDataManager> drawDataManager_ = nullptr;
	/// @brief ウィンドウ生成管理
	std::unique_ptr<WindowMaker> windowMaker_ = nullptr;
	/// @brief 入力管理
	std::unique_ptr<Input> input_ = nullptr;
	/// @brief FPS監視
	std::unique_ptr<FPSObserver> fpsObserver_ = nullptr;

private:

	/// @brief アプリケーション終了フラグ
	bool exit_ = false;
	/// @brief ウィンドウの×ボタンが押されたかどうか
	bool pushedCrossButton_ = false;

	/// @brief ImGuiが有効かどうか
	bool imGuiActive_ = false;
	/// @brief ImGuiが描画済みかどうか
	bool imguiDrawed_ = false;

	/// @brief アプリケーションインスタンスハンドル
	HINSTANCE hInstance_ = nullptr;
	/// @brief Windowsメッセージ
	MSG msg_{};

	/// @brief ロガー
	Logger logger_ = nullptr;
};
