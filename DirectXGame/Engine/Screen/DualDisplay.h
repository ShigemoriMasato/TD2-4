#pragma once
#include "Display/IDisplay.h"

/**
 * @class DualDisplay
 * @brief ダブルバッファリングを実装したディスプレイクラス
 * 
 * 2つのディスプレイバッファを交互に使用し、スムーズな描画を実現する。
 */
class DualDisplay : public IDisplay {
public:

	/**
	 * @brief 静的初期化
	 * @param device DirectX12デバイス
	 */
	static void StaticInitialize(DXDevice* device);

	/// @brief デフォルトコンストラクタ
	DualDisplay(std::string debugName = "UnknownName");

	/// @brief デストラクタ
	~DualDisplay();

	/**
	 * @brief デュアルディスプレイを初期化する
	 * @param data 最初のテクスチャデータ
	 * @param data2 2番目のテクスチャデータ
	 */
	void Initialize(TextureData* data, TextureData* data2);

	/**
	 * @brief 描画前処理
	 * @param commandList コマンドリスト
	 * @param isClear 画面をクリアするかどうか
	 */
	void PreDraw(CommandObject* cmdObject, bool isClear) override;

	/**
	 * @brief テクスチャとして使用可能な状態にする
	 * @param commandList コマンドリスト
	 */
	void ToTexture(CommandObject* cmdObject) override;

	/**
	 * @brief 描画後処理
	 * @param commandList コマンドリスト
	 */
	void PostDraw(CommandObject* cmdObject) override;

	/**
	 * @brief テクスチャリソースを取得
	 * @return ID3D12Resource*
	 */
	ID3D12Resource* GetTextureResource() const override { return Displays_[index_].textureData_->GetResource(); }

	/**
	 * @brief RTVハンドルを取得
	 * @return RTVHandle
	 */
	RTVHandle GetRTVHandle() const override { return Displays_[index_].rtvHandle_; }

	/**
	 * @brief テクスチャデータを取得
	 * @return TextureData*
	 */
	TextureData* GetTextureData() const override { return Displays_[index_].textureData_; }

private:

	/**
	 * @brief リソースバリアを編集する
	 * @param commandList コマンドリスト
	 * @param afterState 変更後のリソースステート
	 */
	void EditBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES afterState);

private:

	/// @brief DirectX12デバイス（静的メンバ）
	static DXDevice* device_;

private:

	friend class Display;

	Logger logger = nullptr;
	std::string debugName_;

	/**
	 * @struct DisplayData
	 * @brief ディスプレイバッファのデータ
	 */
	struct DisplayData {
		RTVHandle rtvHandle_;                                                  ///< レンダーターゲットビューハンドル
		DSVHandle dsvHandle_;                                                  ///< 深度ステンシルビューハンドル

		D3D12_RESOURCE_STATES resourceState_;                                  ///< 現在のリソースステート

		TextureData* textureData_ = nullptr;                                   ///< テクスチャデータ
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr; ///< 深度ステンシルリソース
	};

	/// @brief ディスプレイバッファ（2つ）
	DisplayData Displays_[2]{};
	int index_ = 0;

	/// @brief クリアカラー
	Vector4 clearColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	/// @brief ディスプレイの幅
	int width_{};
	/// @brief ディスプレイの高さ
	int height_{};

};
