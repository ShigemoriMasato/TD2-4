#pragma once
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Render/RenderObject.h>


enum class GridState
{
	// ロック中・解放不可
	LockedUnavailable,
	// ロック中・解放可能
	LockedAvailable,
	// 解放済み・空
	UnlockedEmpty,
	// 解放済み・アイテムあり
	UnlockedOccupied,
};


/// <summary>
/// バックパックの1マス
/// </summary>
class BackPackGrid
{
public:
	BackPackGrid();
	~BackPackGrid();
	void Initialize(GridState state);
	void Update();

	Transform transform_{};
private:
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	GridState state_ = GridState::LockedUnavailable;
};

/// <summary>
/// BackPackGridの集合体
/// </summary>
class BackPack
{
public:
	BackPack();
	~BackPack();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(const Matrix4x4& viewProj);
	void Draw(SHEngine::Command::Object* cmdObject);

private:
	std::vector<Matrix4x4> lockedWorlds_;
	int lockedTextureIndex_ = 0;
	Vector4 lockedColor_ = { 0.25f, 0.25f, 0.25f, 1.0f };
	DirectionalLight lockedLight_{};

	// BackPackGridの2次元配列
	std::vector<std::vector<std::unique_ptr<BackPackGrid>>> grids_;

	/// インスタンシング描画
	// ロック中・解放不可
	std::unique_ptr<SHEngine::RenderObject> lockedUnavailableGrid_;
	// ロック中・解放可能
	std::unique_ptr<SHEngine::RenderObject> lockedAvailableGrid_;
	// 解放済み
	std::unique_ptr<SHEngine::RenderObject> unlockedEmptyGrid_;

	int lockedMatricesSrvIndex_ = -1;   // SRV(Vt0)
	int lockedVpCbvIndex_ = -1;         // CBV(Vb0)
	int lockedTexIndexCbvIndex_ = -1;   // CBV(Pb0)
	int lockedColorCbvIndex_ = -1;      // CBV(Pb1)
	int lockedLightCbvIndex_ = -1;      // CBV(Pb2)
};


namespace GameConstants
{
	// バックパックの行数
	inline constexpr size_t kBackPackRowNum = 8;
	// バックパックの列数
	inline constexpr size_t kBackPackColNum = 12;
}