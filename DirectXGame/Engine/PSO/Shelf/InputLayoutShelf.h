#pragma once
#include <vector>
#include <cstdint>
#include <d3d12.h>

/**
 * @enum InputLayoutID
 * @brief 入力レイアウトのID
 */
enum class InputLayoutID : uint8_t {
	Default = 0,    ///< デフォルト(位置、法線、UV)
	Textured,       ///< テクスチャ付き
	Vector3,        ///< 位置のみ
	Skinning,       ///< スキニング用(ボーン情報含む)

	Count           ///< 入力レイアウトの総数
};

/**
 * @class InputLayoutShelf
 * @brief 入力レイアウトを管理するクラス
 * 
 * 複数の入力レイアウトを事前に定義し、IDで取得できるようにする。
 */
class InputLayoutShelf {
public:

	InputLayoutShelf();
	~InputLayoutShelf();

	/**
	 * @brief 入力レイアウト記述の取得
	 * 
	 * @param id 入力レイアウトID
	 * @return 入力レイアウト記述
	 */
	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc(InputLayoutID id) const;

private:

	/// @brief 入力要素記述の配列(各レイアウト用)
	std::vector<std::vector<D3D12_INPUT_ELEMENT_DESC>> inputElementsList_;
	/// @brief 入力レイアウト記述の配列
	std::vector<D3D12_INPUT_LAYOUT_DESC> inputLayouts_;

};

