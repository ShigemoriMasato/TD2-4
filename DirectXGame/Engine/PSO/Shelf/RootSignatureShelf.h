#pragma once
#include <vector>
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <Tool/Logger/Logger.h>
#include "ShaderShelf.h"

/**
 * @enum SamplerID
 * @brief サンプラーID(ビットフラグで複数指定可能)
 */
enum class SamplerID : uint32_t {
	Non = 0,             ///< サンプラーなし
	Static = 1 << 0,     ///< リピートサンプラー
	Clamp = 1 << 1,      ///< クランプサンプラー

	Count                ///< サンプラーIDの総数
};

/**
 * @brief SamplerIDのビットOR演算子
 * @return ビットORの結果
 */
uint32_t operator|(SamplerID a, SamplerID b);

/**
 * @brief uint32_tとSamplerIDのビットOR演算子
 * @return ビットORの結果
 */
uint32_t operator|(uint32_t a, SamplerID b);

/**
 * @brief SamplerIDの比較演算子
 * @return a < bの結果
 */
bool operator<(SamplerID a, SamplerID b);

/**
 * @struct RootSignatureConfig
 * @brief ルートシグネチャの設定情報
 * 
 * ルートシグネチャに必要な定数バッファ、シェーダーリソース、
 * テクスチャ、サンプラーの設定をまとめた構造体。
 */
struct RootSignatureConfig {
	std::pair<int, int> cbvNums{};                         ///< 定数バッファ数<Vertex, Pixel>
	std::pair<int, int> srvNums{};                         ///< シェーダーリソース数<Vertex, Pixel>（上限8）
	bool useTexture = false;                                ///< テクスチャを使用するか
	uint32_t samplers = uint32_t(SamplerID::Static);        ///< サンプラーIDのビットマスク

	bool operator<(const RootSignatureConfig& other) const;
	bool operator==(const RootSignatureConfig& other) const;
};

namespace std {
	template<>
	struct hash<RootSignatureConfig> {
		size_t operator()(const RootSignatureConfig& cfg) const {
			size_t h = 0;
			hash_combine(h, hash<int>()(cfg.cbvNums.first));
			hash_combine(h, hash<int>()(cfg.cbvNums.second));
			hash_combine(h, hash<int>()(cfg.srvNums.first));
			hash_combine(h, hash<int>()(cfg.srvNums.second));
			hash_combine(h, hash<bool>()(cfg.useTexture));
			hash_combine(h, hash<uint32_t>()(cfg.samplers));
			return h;
		}
	private:
		static void hash_combine(size_t& seed, size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}

class RootSignatureShelf {
public:

	RootSignatureShelf(ID3D12Device* device);
	~RootSignatureShelf();

	ID3D12RootSignature* GetRootSignature(const RootSignatureConfig& config);

private:

	ID3D12RootSignature* CreateRootSignature(const RootSignatureConfig& config);

	ID3D12Device* device_ = nullptr;

	std::map<RootSignatureConfig, ID3D12RootSignature*> rootSignatureMap_;
	std::map<SamplerID, D3D12_STATIC_SAMPLER_DESC> samplers_;

	std::shared_ptr<spdlog::logger> logger_ = nullptr;

};
