#pragma once
#include "TextureData.h"
#include <Core/DXDevice.h>
#include <Core/CmdListManager.h>
#include <map>
#include <unordered_map>
#include <memory>

/**
 * @class TextureManager
 * @brief テクスチャの読み込みと管理を行うクラス
 * 
 * ファイルからのテクスチャ読み込み、ウィンドウ用テクスチャ、
 * スワップチェーン用テクスチャの作成をサポート。
 * GPUへのアップロードと中間リソースの管理も行う。
 */
class TextureManager {
public:

	/// @brief デフォルトコンストラクタ
	TextureManager() = default;
	/// @brief デストラクタ
	~TextureManager() = default;

	/**
	 * @brief テクスチャマネージャーを初期化
	 * @param device DirectX12デバイス
	 * @param CmdListManager コマンドリストマネージャー
	 */
	void Initialize(DXDevice* device, CmdListManager* CmdListManager);
	
	/**
	 * @brief 全てのテクスチャをクリア
	 */
	void Clear();

	/**
	 * @brief すべてのテクスチャを読み込む
	 */
	void LoadAllTextures();

	/**
	 * @brief ファイルからテクスチャを取得（未読込のときerrorテクスチャを返す）
	 * @param filePath テクスチャファイルのパス
	 * @return テクスチャハンドル
	 */
	int GetTexture(std::string filePath) const;

	/**
	 * @brief ファイルからテクスチャを読み込む
	 * @param filePath テクスチャファイルのパス
	 * @return テクスチャハンドル
	 */
	int LoadTexture(const std::string& filePath);

	/**
	 * @brief エラーテクスチャのハンドルを取得
	 * @return エラーテクスチャハンドル
	 */
	int GetErrorTextureHandle() const { return 2; }
	
	/**
	 * @brief ウィンドウ用テクスチャを作成
	 * @param width 幅
	 * @param height 高さ
	 * @param clearColor クリアカラー
	 * @return テクスチャハンドル
	 */
	int CreateWindowTexture(uint32_t width, uint32_t height, uint32_t clearColor);
	
	/**
	 * @brief スワップチェーン用テクスチャを作成
	 * @param resource スワップチェーンのリソース
	 * @return テクスチャハンドル
	 */
	int CreateSwapChainTexture(ID3D12Resource* resource);

	/**
	 * @brief ビットマップテクスチャを作成
	 * @param width 幅
	 * @param height 高さ
	 * @param colorMap カラーマップ（RGBA形式の32ビットカラーの配列）
	 * @return テクスチャハンドル
	 */
	int CreateBitmapTexture(uint32_t width, uint32_t height, std::vector<uint32_t> colorMap);
	
	/**
	 * @brief テクスチャを削除
	 * @param handle テクスチャハンドル
	 */
	void DeleteTexture(int handle);
	
	/**
	 * @brief テクスチャを削除
	 * @param textureData テクスチャデータへのポインタ
	 */
	void DeleteTexture(TextureData* textureData);

	/**
	 * @brief テクスチャデータを取得
	 * @param handle テクスチャハンドル
	 * @return TextureDataへのポインタ
	 */
	TextureData* GetTextureData(int handle);
	
	/**
	 * @brief テクスチャリソースを取得
	 * @param handle テクスチャハンドル
	 * @return ID3D12ResourceのComポインタ
	 */
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTextureResource(int handle) {
		return textureDataList_[handle]->textureResource_;
	}

	/**
	 * @brief テクスチャをGPUにアップロード
	 * @param cmdList コマンドリスト
	 */
	void UploadTextures(ID3D12GraphicsCommandList* cmdList);
	
	/**
	 * @brief アップロード済みリソースをクリア
	 */
	void ClearUploadedResources();

private:

	/**
	 * @brief テクスチャ数の上限をチェック
	 * @param offset オフセット
	 */
	void CheckMaxCount(int offset);

	/// @brief DirectX12デバイスへのポインタ
	DXDevice* device_ = nullptr;
	/// @brief SRVマネージャーへのポインタ
	SRVManager* srvManager_ = nullptr;
	/// @brief コマンドオブジェクト
	std::shared_ptr<CommandObject> cmdObject_ = nullptr;

	/// @brief 最大テクスチャ数
	const int maxTextureCount = 1024;

	/// @brief テクスチャデータのマップ（ハンドル → TextureData）
	std::map<int, std::unique_ptr<TextureData>> textureDataList_;

	/// @brief アップロード待ちリソース
	std::vector< std::pair<ID3D12Resource*, DirectX::ScratchImage>> uploadResources_;
	/// @brief 中間リソース
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;
	/// @brief ミップマップアップロードデータ
	std::vector<TextureData::MipMapUploadData> mipUploadData_;
	/// @brief ミップマップアップロードデータ（アップロード中）
	std::vector<TextureData::MipMapUploadData> mipUploadingData_;
	/// @brief 読み込んだテクスチャパスのマップ（ファイルパス → ハンドル）
	std::unordered_map<std::string, int> loadedTexturePaths_;

	/// @brief エラーテクスチャハンドル
	int errorTextureHandle_ = -1;

	/// @brief ロガー
	Logger logger_ = nullptr;
};

