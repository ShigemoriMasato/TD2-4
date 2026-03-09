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
		int maxSegments = 32;      // セグメント数（quad数）。頂点数は maxSegments*2
		float lifeTime = 0.20f;    // 寿命
		float minDistance = 0.1f; // minDistance以上動いてないときは追加しない

		// 見た目
		Vector4 colorNormal = { 1.0f, 1.0f, 1.0f, 0.65f };
		Vector4 colorAdd = { 0.6f, 0.9f, 1.0f, 0.75f };

		// 描画パス
		bool drawNormal = true;
		bool drawAdd = true;

		// テクスチャ
		std::string defaultTexturePath = "Assets/.EngineResource/Texture/white1x1.png";
	};

public:
	Trail() = default;
	~Trail() = default;

	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const Config& config = {});
	void Update(float deltaTime);
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix);

	// ワールド座標で2点を追加
	void PushSegment(const Vector3& baseWS, const Vector3& tipWS);

	// 制御
	void Clear();
	void SetEnabled(bool enabled) { enabled_ = enabled; }
	bool IsEnabled() const { return enabled_; }

	// テクスチャ差し替え
	void SetTexturePath(const std::string& texturePath); // LoadTextureを内部で呼ぶ
	void SetTextureHandle(int textureHandle);            // 既にLoad済みならこちら

	// 設定
	Config& GetConfig() { return config_; }
	const Config& GetConfig() const { return config_; }

private:
	struct Sample
	{
		Vector3 base;
		Vector3 tip;
		float age = 0.0f; // 秒
		float u = 0.0f;   // 0..1（長さ方向）
	};

	struct GpuVertex
	{
		Vector4 position; // input layoutに合わせてfloat4
		Vector2 uv;
		Vector3 normal;
		Vector4 color;
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
	bool hasLast_ = false;

	// GPU転送用（最大固定サイズ）
	std::vector<GpuVertex> gpuVertices_; // 最大: (maxSegments+1)*2
	int maxVertexCount_ = 0;

	// 描画用
	std::unique_ptr<SHEngine::RenderObject> renderNormal_;
	std::unique_ptr<SHEngine::RenderObject> renderAdd_;

	// ダミーのDrawData（Plane相当ではなく、固定数の頂点・インデックス）
	int dummyDrawDataIndex_ = -1;

	// SRV/CBVのindex（RenderObject側のバッファインデックス）
	int srvVertexIndex_ = -1;

	int cbvVpIndex_ = -1;
	int cbvColorIndex_ = -1;
	int cbvTextureIndex_ = -1;

	int textureHandle_ = -1;
};