#pragma once
#include <Render/RenderObject.h>
#include <Assets/Texture/TextureManager.h>
#include <Render/DrawDataManager.h>
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <deque>
#include <memory>
#include <string>
#include <GameObject/Effect/Trail/Preset/TrailPreset.h>

class ITrail
{
public:
	static constexpr uint32_t kMaxVertices_ = 128;

	struct GpuVertex
	{
		Vector4 position{ 0, 0, 0, 0 };
		Vector2 uv;
		Vector4 color;
		uint32_t textureIndex = 0;
	};

private:
	// 外部
	SHEngine::TextureManager* textureManager_ = nullptr;


protected:

	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool isActive_ = true;
	TrailPresetVariant config_;

	// GPU転送用
	std::vector<GpuVertex> gpuVertices_;

	// リソースハンドル
	int textureHandle_ = -1;

public:
	ITrail() = default;
	~ITrail() = default;

	void Initialize(SHEngine::TextureManager* textureManager);
	virtual void Update(float deltaTime) = 0;

	TrailPresetVariant& GetUniqueConfig() { return config_; }

	// 制御
	virtual void Clear() = 0;

	// テクスチャセット
	void SetTexture(const std::string& texturePath) { textureHandle_ = textureManager_->LoadTexture(texturePath); }
	// Configセット
	virtual void SetConfig(const TrailPresetVariant& config) = 0;
	// 追従行列セット
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	// アクティブフラグセット
	void SetIsActive(bool isActive) { isActive_ = isActive; }


	// Drawer用
	int GetTextureHandle() const { return textureHandle_; }

	const std::vector<GpuVertex>& GetGpuVertices() const { return gpuVertices_; }

};