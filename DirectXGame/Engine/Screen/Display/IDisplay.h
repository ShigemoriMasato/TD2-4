#pragma once
#include <Core/DXDevice.h>
#include <Utility/Vector.h>
#include <Assets/Texture/TextureData.h>

/**
 * @class IDisplay
 * @brief ディスプレイの抽象基底クラス
 * 
 * Display、DualDisplayなどの基底インターフェース。
 */
class IDisplay {
public:
	/// @brief 仮想デストラクタ
	virtual ~IDisplay() = default;

	/**
	 * @brief 描画前処理
	 * @param commandList コマンドリスト
	 * @param isClear 画面をクリアするかどうか
	 */
	virtual void PreDraw(ID3D12GraphicsCommandList* commandList, bool isClear) = 0;

	/**
	 * @brief テクスチャとして使用可能な状態にする
	 * @param commandList コマンドリスト
	 */
	virtual void ToTexture(ID3D12GraphicsCommandList* commandList) = 0;

	/**
	 * @brief 描画後処理
	 * @param commandList コマンドリスト
	 */
	virtual void PostDraw(ID3D12GraphicsCommandList* commandList) = 0;

	/**
	 * @brief テクスチャリソースを取得
	 * @return ID3D12Resource*
	 */
	virtual ID3D12Resource* GetTextureResource() const = 0;

	/**
	 * @brief RTVハンドルを取得
	 * @return RTVHandle
	 */
	virtual RTVHandle GetRTVHandle() const = 0;

	/**
	 * @brief テクスチャデータを取得
	 * @return TextureData*
	 */
	virtual TextureData* GetTextureData() const = 0;

};
