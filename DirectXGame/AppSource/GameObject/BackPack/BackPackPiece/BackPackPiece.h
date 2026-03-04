#pragma once
#include "GameObject/BackPack/GameConstants.h"

class Item;

// バックパックの1マス
class BackPackPiece
{
public:
	BackPackPiece();
	~BackPackPiece();
	void Initialize(GridState state);
	void SetPosition(const Vector3& pos)
	{
		transform_.position = pos; 
	}
	void ChangeState(GridState newState);

	GridState GetState() const { return state_; }
	const Vector4& GetColor() const { return color_; }

	Matrix4x4 GetWorldMatrix() const
	{
		return Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	}

	// アイテムを設置する関数
	bool SetItem(Item* item, const int localIndex);

private:
	Transform transform_{};
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	GridState state_ = GridState::LockedUnavailable;

	Item* item_ = nullptr;
	// 設置されているアイテムのローカルインデックス(Item::mapDataのインデックス)
	int index_;
};

// バックパックの全マスの描画
class DrawBackPack
{
public:
	DrawBackPack();
	~DrawBackPack();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(const Matrix4x4& viewProj, const std::vector<std::vector<std::unique_ptr<BackPackPiece>>>& grids);
	void Draw(SHEngine::Command::Object* cmdObject);
	void DrawImGui();

private:
	int textureIndex_ = 0;
	DirectionalLight light_{};


	// ロック中・解放不可
	std::unique_ptr<SHEngine::RenderObject> lockedUnavailableGrid_;
	std::vector<Matrix4x4> lockedUnavailableWorlds_;
	Vector4 lockedUnavailableColor_ = { 0.25f, 0.25f, 0.25f, 1.0f };
	InstanceBinding bindLockedUnavailable_{};

	// ロック中・解放可能
	std::unique_ptr<SHEngine::RenderObject> lockedAvailableGrid_;
	std::vector<Matrix4x4> lockedAvailableWorlds_;
	Vector4 lockedAvailableColor_ = { 0.5f, 0.5f, 0.5f, 1.0f };
	InstanceBinding bindLockedAvailable_{};

	// 解放済み
	std::unique_ptr<SHEngine::RenderObject> unlockedEmptyGrid_;
	std::vector<Matrix4x4> unlockedEmptyWorlds_;
	Vector4 unlockedEmptyColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	InstanceBinding bindUnlockedEmpty_{};

	// 毎フレーム転送（3重バッファ対策）
	void UpdateRenderObject(
		SHEngine::RenderObject& ro,
		const InstanceBinding& bind,
		const Matrix4x4& viewProj,
		const std::vector<Matrix4x4>& worlds,
		const Vector4& color);
};