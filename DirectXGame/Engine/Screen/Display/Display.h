#pragma once
#include "IDisplay.h"

/**
 * @class Display
 * @brief スワップチェーン専用のディスプレイクラス
 * 
 * スワップチェーンと連携して画面表示を行う。
 */
class Display : public IDisplay {
public:

	/**
	 * @brief 静的初期化
	 * @param device DirectX12デバイス
	 */
	static void StaticInitialize(DXDevice* device);

	/// @brief デフォルトコンストラクタ
	Display() = default;

	/// @brief デストラクタ
	~Display();

	/**
	 * @brief ディスプレイを初期化する
	 * @param data テクスチャデータ
	 * @param color クリアカラー
	 */
	void Initialize(TextureData* data, uint32_t color);

	/**
	 * @brief 描画前処理
	 * @param commandList コマンドリスト
	 * @param isClear 画面をクリアするかどうか
	 */
	void PreDraw(ID3D12GraphicsCommandList* commandList, bool isClear) override;

	/**
	 * @brief テクスチャとして使用可能な状態にする
	 * @param commandList コマンドリスト
	 */
	void ToTexture(ID3D12GraphicsCommandList* commandList) override;

	/**
	 * @brief 描画後処理
	 * @param commandList コマンドリスト
	 */
	void PostDraw(ID3D12GraphicsCommandList* commandList) override;

	/**
	 * @brief テクスチャリソースを取得
	 * @return ID3D12Resource*
	 */
	ID3D12Resource* GetTextureResource() const override { return textureData->GetResource(); }

	/**
	 * @brief RTVハンドルを取得
	 * @return RTVHandle
	 */
	RTVHandle GetRTVHandle() const override { return rtvHandle_; }

	/**
	 * @brief テクスチャデータを取得
	 * @return TextureData*
	 */
	TextureData* GetTextureData() const override { return textureData; }

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

	/// @brief レンダーターゲットビューハンドル
	RTVHandle rtvHandle_;
	/// @brief 深度ステンシルビューハンドル
	DSVHandle dsvHandle_;

	/// @brief 現在のリソースステート
	D3D12_RESOURCE_STATES resourceState_;
	
	/// @brief テクスチャデータ
	TextureData* textureData = nullptr;
	/// @brief 深度ステンシルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	/// @brief クリアカラー
	Vector4 clearColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	/// @brief ディスプレイの幅
	int width_;
	/// @brief ディスプレイの高さ
	int height_;
};
