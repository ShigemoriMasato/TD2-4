#pragma once
#include <memory>
#include <Utility/Matrix.h>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

// SetConfigとUpdateは継承先で実装すること
class IParticle
{
public:
	static constexpr uint32_t kMaxParticles_ = 4096;

	// GPUに渡す情報
	struct InstanceGpu
	{
		Matrix4x4 world{ Matrix4x4::Identity() };
		Vector4 color{ 1,1,1,1 };
		uint32_t textureIndex = 0;
		uint32_t modelIndex = 0;
	};

private:
	// 外部
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;

	// 履歴
	size_t aliveCount_ = 0;

	// GPU転送用
	std::vector<InstanceGpu> gpuInstances_;

	// リソースハンドル
	int modelHandle_ = -1;
	int textureHandle_ = -1;

protected:

	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool isActive_ = false;
	float emitTimer_ = 0.0f;
	Vector3 cameraPos_ = { 0.0f,0.0f,0.0f };
	ParticlePresetVariant config_;
	int32_t emitCount_ = 0;

public:
	IParticle() = default;
	virtual ~IParticle() = default;

	void Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager);
	virtual void Update(float dt) = 0;
	void pushInstance(const Matrix4x4& world, const Vector4& color = { 1,1,1,1 });

	ParticlePresetVariant& GetUniqueConfig() { return config_; }

	// 制御
	void Clear();

	// テクスチャセット
	void SetTexture(const std::string& texturePath) { textureHandle_ = textureManager_->LoadTexture(texturePath); }
	// モデルセット
	void SetModel(const std::string& modelPath) { modelHandle_ = modelManager_->LoadModel(modelPath); }
	// Configセット
	virtual void SetConfig(const ParticlePresetVariant& config) = 0;
	// 追従行列セット
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	// アクティブフラグセット
	void SetEnabled(bool isActive);
	// カメラ位置セット
	void SetCameraPos(const Vector3& cameraPos) { cameraPos_ = cameraPos; }
	// 発生回数を取得
	int32_t GetEmitCount() const { return emitCount_; }


	// Drawer用
	int GetModelHandle() const { return modelHandle_; }
	int GetTextureHandle() const { return textureHandle_; }
	const InstanceGpu* GetGpuInstanceData() const { return gpuInstances_.data(); }
	uint32_t GetGpuInstanceCount() const { return static_cast<uint32_t>(aliveCount_); }

	std::vector<Matrix4x4> GetParticleWorlds() const;
	size_t GetAliveCount() const { return aliveCount_; }

};