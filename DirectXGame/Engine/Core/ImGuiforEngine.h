#pragma once
#include <Core/DXDevice.h>
#include <Core/CmdListManager.h>
#include <Screen/Window.h>

/**
 * @class ImGuiforEngine
 * @brief ImGuiのエンジン統合を管理するクラス
 * 
 * DirectX12バックエンドを使用したImGuiの初期化、フレーム管理、描画処理を担当する。
 * エンジン全体で使用するデバッグUIやエディタUIの基盤を提供する。
 */
class ImGuiforEngine {
public:

	/**
	 * @brief ImGuiを初期化する
	 * @param device DirectX12デバイス
	 * @param cmdManager コマンドリストマネージャー
	 * @param window ImGuiを描画するウィンドウ
	 */
	void Initialize(DXDevice* device, CmdListManager* cmdManager, Window* window);

	/**
	 * @brief ImGuiのフレーム開始処理
	 * 
	 * ImGuiの新しいフレームを開始し、ドッキングスペースをセットアップする。
	 */
	void BeginFrame();
	
	/**
	 * @brief ImGuiのフレーム終了処理
	 * 
	 * ImGuiの描画データをレンダリングし、コマンドリストに記録する。
	 */
	void EndFrame();

	/**
	 * @brief ImGuiリソースを解放する
	 */
	void Finalize();

private:

	/// @brief ロガー
	Logger logger_ = nullptr;

	/// @brief SRVマネージャーへのポインタ
	SRVManager* srv_;

	/// @brief ImGui用SRVハンドル
	SRVHandle srvHandle_;
	/// @brief ImGui用SRVハンドル2
	SRVHandle srvHandle2_;
	/// @brief コマンドオブジェクトへのポインタ
	CommandObject* commandObject_ = nullptr;

};

