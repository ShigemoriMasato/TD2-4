#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <Utility/Vector.h>
#include <assets/Model/ModelManager.h>
#include <memory>
#include <vector>

/// <summary>
/// グリッド生成用の構造体
/// </summary>
struct GridConfig {
	float cellSize = 1.0f;      // グリッド1マスの大きさ
	int width = 20;             // 横方向のマス数
	int height = 20;            // 縦方向のマス数
	Vector3 origin = {-10.0f, 0.0f, -10.0f}; // グリッドの開始地点
};

namespace Player {
class HeatmapManager {
public:
	// 初期化関数
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, GridConfig config);

	// グリッドのインデックスに滞在時間を記録する関数
	void Record(Vector3 position, float deltaTime);

	// 更新関数
	void Update(const Matrix4x4& viewProj);

	// 描画関数
	void Draw(CmdObj* cmdObj);

private:
	// グリッド生成用の設定パラメータ
	GridConfig config_;

	// グリッド内の重みを保持する変数
	std::vector<float> data_;

	// 描画用変数
	std::unique_ptr<SHEngine::RenderObject> render_ = nullptr;

	// インスタンスデータ
	std::vector<Matrix4x4> worldMatrices_;
	std::vector<Vector4> colors_;

	// 最大閾値
	float maxStayTime_ = 5.0f;
	
	// インスタンス数の最大値
	static constexpr int maxInstanceNum_ = 512;
};
} // namespace Player
