#pragma once
#include <Core/DXDevice.h>
#include <Assets/Texture/TextureManager.h>
#include "IDisplay.h"

namespace SHEngine::Screen {

	class SingleDisplay : public IDisplay {
	public:

		/// @brief OffScreen用初期化関数
		/// @param textureManager テクスチャマネージャーへのポインタ
		/// @param width 幅
		/// @param height 高さ
		/// @param clearColor クリアカラー
		void Initialize(TextureManager* textureManager, int width, int height, uint32_t clearColor);

		/// @brief スワップチェーン用初期化関数
		/// @param textureManager テクスチャマネージャーへのポインタ
		/// @param resource スワップチェーンのリソース
		void Initialize(TextureManager* textureManager, ID3D12Resource* resource, uint32_t clearColor);

		void PreDraw(Command::Object* cmdObject, bool isClear = true) override;
		void PostDraw(Command::Object* cmdObject) override;
		void ToTexture(Command::Object* cmdObject) override;

		DXGI_FORMAT D3D12GetRTVFormat() override { return rtvFormat_; }

	private:

		void PrivateInitialize();

		void TransitionBarrier(Command::Object* cmdObject, D3D12_RESOURCE_STATES after);

		TextureData* textureData_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

		RTVHandle rtvHandle_{};
		DSVHandle dsvHandle_{};

		DXGI_FORMAT rtvFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		D3D12_RESOURCE_STATES currentBarrier_ = D3D12_RESOURCE_STATE_COMMON;

		int width_ = 0;
		int height_ = 0;

	};

}
