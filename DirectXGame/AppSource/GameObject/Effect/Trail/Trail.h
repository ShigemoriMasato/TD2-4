#pragma once
#include <Render/RenderObject.h>
#include <Assets/Texture/TextureManager.h>
#include <Render/DrawDataManager.h>
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <deque>
#include <memory>
#include <string>

class Trail
{
public:
	struct Config
	{
		// 履歴
		int maxSegments = 32;      // セグメント数。頂点数は maxSegments*2
		float lifeTime = 0.20f;    // 寿命
		float minDistance = 0.1f; // minDistance以上動いてないときは追加しない

		// 見た目
		Vector4 color = { 0.6f, 0.9f, 1.0f, 0.75f };

		// テクスチャ
		std::string texturePath = "Assets/.EngineResource/Texture/white1x1.png";
	};

	struct GpuVertex
	{
		Vector4 position;
		Vector2 uv;
		Vector3 normal;
		Vector4 color;
		uint32_t textureIndex = 0;
	};

public:
	Trail() = default;
	~Trail() = default;

	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const Config& config = {});
	void Update(float deltaTime, const Matrix4x4& vpMatrix);

	// ワールド座標で2点を追加
	void PushSegment(const Vector3& baseWS, const Vector3& tipWS);

	// 制御
	void Clear();
	void SetEnabled(bool enabled) { enabled_ = enabled; }
	bool IsEnabled() const { return enabled_; }

	// テクスチャ差し替え
	void SetTexture(const std::string& texturePath);

	// 設定
	Config& GetConfig() { return config_; }
	const Config& GetConfig() const { return config_; }

	// 今フレーム作られた頂点を返す（最大固定長vectorの先頭から activeVertexCount_ までが有効）
	const std::vector<GpuVertex>& GetGpuVertices() const { return gpuVertices_; }
	int GetActiveVertexCount() const { return activeVertexCount_; }

	// テクスチャ
	int GetTextureHandle() const { return textureHandle_; }

private:
	struct Sample
	{
		Vector3 base;
		Vector3 tip;
		float age = 0.0f; // 秒
		float u = 0.0f;   // 0..1（長さ方向）
	};

private:
	void RebuildVertices(const Matrix4x4& vpMatrix);

private:
	// 外部
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;

	// 設定
	Config config_{};
	bool enabled_ = true;

	// 履歴
	std::deque<Sample> samples_;
	Vector3 lastBase_{};
	Vector3 lastTip_{};
	bool hasLast_ = false;	// 一点でも追加されたらtrue

	// GPU転送用
	std::vector<GpuVertex> gpuVertices_; // 最大: (maxSegments+1)*2
	int maxVertexCount_ = 0;
	int activeVertexCount_ = 0;

	Matrix4x4 vpMatrix_{ Matrix4x4::Identity() };

	int textureHandle_ = -1;
};