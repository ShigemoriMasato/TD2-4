#pragma once
#include <vector>
#include <cstdint>
#include <d3d12.h>

/**
 * @enum RasterizerID
 * @brief ラスタライザーモードのID
 */
enum class RasterizerID : uint8_t {
	Fill,       ///< 塗りつぶしモード(通常)
	Wireframe,  ///< ワイヤーフレームモード(デバッグ用)
	Back,       ///< 背面を描画する

	Count       ///< ラスタライザーモードの総数
};

/**
 * @class RasterizerShelf
 * @brief ラスタライザーステートを管理するクラス
 * 
 * 複数のラスタライザーモードを事前に定義し、IDで取得できるようにする。
 */
class RasterizerShelf {
public:

	RasterizerShelf();
	~RasterizerShelf();

	/**
	 * @brief ラスタライザー設定の取得
	 * 
	 * @param id ラスタライザーID
	 * @return ラスタライザーの設定
	 */
	D3D12_RASTERIZER_DESC GetRasterizerDesc(RasterizerID id) const;

private:

	/// @brief 事前定義されたラスタライザー設定の配列
	std::vector<D3D12_RASTERIZER_DESC> rasterizerDescs_;

};
