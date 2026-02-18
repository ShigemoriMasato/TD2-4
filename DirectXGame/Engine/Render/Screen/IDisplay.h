#pragma once
#include <Core/Command/CommandManager.h>
#include <Assets/Texture/TextureData.h>

namespace SHEngine::Screen {

	class IDisplay {
	public:

		virtual ~IDisplay() = default;

		static void SetDevice(DXDevice* device) { device_ = device; }

		/// @brief レンダーターゲットの切り替えや、画面のクリア
		/// @param cmdObject コマンドオブジェクト
		/// @param isClear 画面をクリアするかどうか
		virtual void PreDraw(Command::Object* cmdObject, bool isClear) = 0;

		/// @brief 描画後の処理（Presentや、OffScreen用テクスチャへのコピーなど）
		/// @param cmdObject コマンドオブジェト
		virtual void PostDraw(Command::Object* cmdObject) = 0;

		/// @brief 描画結果をテクスチャにコピーする
		/// @param cmdObject コマンドオブジェクト
		virtual void ToTexture(Command::Object* cmdObject) = 0;

		virtual TextureData* GetTextureData() const = 0;

		virtual DXGI_FORMAT D3D12GetRTVFormat() = 0;

	protected:

		static inline DXDevice* device_ = nullptr;

	};

}
