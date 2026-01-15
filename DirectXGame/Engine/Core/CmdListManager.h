#pragma once
#include "Data/CommandObject.h"
#include <Core/DXDevice.h>
#include <Tool/Logger/Logger.h>
#include <map>

/**
 * @class CmdListManager
 * @brief DirectX12のコマンドリストとコマンドキューを管理するクラス
 * 
 * コマンドオブジェクトの作成と管理、コマンドキューへのコマンドリスト実行、
 * コマンドリストのリセット処理を担当する。
 * 複数のコマンドオブジェクトを管理し、効率的なGPUコマンドの実行を可能にする。
 */
class CmdListManager {
public:

	/// @brief デフォルトコンストラクタ
	CmdListManager() = default;
	
	/// @brief デストラクタ
	~CmdListManager() = default;

	/**
	 * @brief コマンドリストマネージャーを初期化する
	 * @param device DirectX12デバイス
	 */
	void Initialize(DXDevice* device);
	
	/**
	 * @brief 全てのコマンドリストをコマンドキューに実行する
	 */
	void Execute();
	
	/**
	 * @brief 全てのコマンドオブジェクトをリセットする
	 */
	void Reset();

	/**
	 * @brief 新しいコマンドオブジェクトを作成する
	 * @return 作成されたコマンドオブジェクトの共有ポインタ
	 */
	std::shared_ptr<CommandObject> CreateCommandObject();
	
	/**
	 * @brief コマンドキューを取得する
	 * @return ID3D12CommandQueue*
	 */
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

private:

	/// @brief DirectX12デバイスへのポインタ
	DXDevice* device_ = nullptr;
	/// @brief SRVマネージャーへのポインタ
	SRVManager* srvManager_ = nullptr;
	/// @brief ロガー
	Logger logger_ = nullptr;

	/// @brief コマンドオブジェクトのマップ（ID -> CommandObject）
	std::map<int, std::shared_ptr<CommandObject>> commandObjects_;
	/// @brief 使用中のコマンドオブジェクトIDのリスト
	std::vector<int> cmdObjectCounter_;

	/// @brief コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

};
