#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <Core/View/SRVManager.h>

class CmdListManager;

/**
 * @class CommandObject
 * @brief コマンドリストを管理するクラス
 * 
 * 複数のコマンドアロケータとコマンドリストを管理し、
 * ダブルバッファリングを実現するために切り替え機能を提供する。
 */
class CommandObject {
public:

	CommandObject() = default;
	~CommandObject();

	/**
	 * @brief CommandObjectの初期化
	 * 
	 * コマンドアロケータとコマンドリストを作成し、管理用のデータ構造を初期化する。
	 * 
	 * @param device DirectX12デバイス
	 * @param manager コマンドリストマネージャー
	 * @param srvManager SRVマネージャー
	 * @return 初期化成功時のオブジェクトID
	 */
	int Initialize(ID3D12Device* device, CmdListManager* manager, SRVManager* srvManager);

	/**
	 * @brief コマンドリストの取得
	 * 
	 * @return 現在使用中のコマンドリスト
	 */
	ID3D12GraphicsCommandList* GetCommandList() const;

	/**
	 * @brief コマンドリストのリセット
	 * 
	 * 現在のコマンドアロケータをリセットし、コマンドリストを再利用可能にする。
	 */
	void Reset();

	/**
	 * @brief 使用するコマンドリストを切り替え
	 * 
	 * ダブルバッファリングのために次のコマンドリストに切り替える。
	 */
	void Swap();

private:

	/**
	 * @struct Command
	 * @brief コマンドアロケータとコマンドリストのペア
	 */
	struct Command {
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;   ///< コマンドアロケータ
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;     ///< コマンドリスト
	};
	/// @brief コマンドリストの配列(ダブルバッファリング用)
	std::vector<Command> cmdLists_;

	/// @brief 次に使用するコマンドリストのインデックス
	int index_ = 0;

	/// @brief SetHeap用のSRVマネージャー
	SRVManager* srvManager_;

	/// @brief 次に割り当てるコマンドオブジェクトID(静的メンバ)
	static inline int nextCmdObjId_ = 0;
	/// @brief このコマンドオブジェクトのID
	int cmdObjId_ = nextCmdObjId_++;

	/// @brief コマンドリストマネージャーへのポインタ
	CmdListManager* manager_ = nullptr;
};
