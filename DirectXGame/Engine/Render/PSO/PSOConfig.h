#pragma once
#include "Shelf/ShaderShelf.h"
#include "Shelf/BlendStateShelf.h"
#include "Shelf/DepthStencilShelf.h"
#include "Shelf/RasterizerShelf.h"
#include "Shelf/RootSignatureShelf.h"
#include "Shelf/InputLayoutShelf.h"

namespace SHEngine::PSO {

	/**
	 * @struct PSOConfig
	 * @brief パイプラインステートオブジェクト（PSO）の設定をまとめた構造体
	 *
	 * シェーダー、ブレンドステート、深度ステンシル、ラスタライザー、
	 * ルートシグネチャ、入力レイアウトなどの描画パイプライン設定を保持する。
	 */
	struct Config {
		/// @brief 頂点シェーダーファイル名
		std::string vs = "Object3d.VS.hlsl";
		/// @brief ピクセルシェーダーファイル名
		std::string ps = "Object3d.PS.hlsl";
		/// @brief ルートシグネチャ設定
		RootSignatureConfig rootConfig = {};
		/// @brief 入力レイアウトID
		InputLayoutID inputLayoutID = InputLayoutID::Default;
		/// @brief ブレンドステートID
		BlendStateID blendID = BlendStateID::Normal;
		/// @brief 深度ステンシルID
		DepthStencilID depthStencilID = DepthStencilID::Default;
		/// @brief ラスタライザーID
		RasterizerID rasterizerID = RasterizerID::Fill;
		/// @brief プリミティブトポロジー
		D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		/// @brief スワップチェーン用かどうか
		bool isSwapChain = false;

		/**
		* @brief 等価演算子
		* @param other 比較対象
		* @return 全てのメンバが等しい場合true
		*/
		bool operator==(const Config& other) const {
			return vs == other.vs &&
				ps == other.ps &&
				blendID == other.blendID &&
				depthStencilID == other.depthStencilID &&
				rasterizerID == other.rasterizerID &&
				rootConfig == other.rootConfig &&
				inputLayoutID == other.inputLayoutID &&
				topology == other.topology &&
				isSwapChain == other.isSwapChain;
		}

		/**
		 * @brief 不等価演算子
		 * @param other 比較対象
		 * @return メンバが異なる場合true
		 */
		bool operator!=(const Config& other) const {
			return !(*this == other);
		}
	};

} // namespace SHEngine

/**
 * @brief std::hash特殊化（PSOConfigをunordered_mapのキーとして使用可能にする）
 */
namespace std {
	template<>
	struct hash<SHEngine::PSO::Config> {
		/**
		 * @brief ハッシュ値計算演算子
		 * @param cfg PSOConfig設定
		 * @return ハッシュ値
		 */
		size_t operator()(const SHEngine::PSO::Config& cfg) const {
			size_t h = 0;
			hash_combine(h, hash<string>()(cfg.vs));
			hash_combine(h, hash<string>()(cfg.ps));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.blendID)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.depthStencilID)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.rasterizerID)));
			hash_combine(h, hash<SHEngine::PSO::RootSignatureConfig>()((cfg.rootConfig)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.inputLayoutID)));
			hash_combine(h, hash<int>()(static_cast<int>(cfg.topology)));
			hash_combine(h, hash<bool>()(cfg.isSwapChain));
			return h;
		}

	private:
		/**
		 * @brief ハッシュ値を結合する（Boost風実装）
		 * @param seed 元のハッシュ値（結果もここに格納）
		 * @param value 結合するハッシュ値
		 */
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}
