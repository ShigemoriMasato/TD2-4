#pragma once
#include "GameObject/BackPack/GameConstants.h"

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

private:
	Transform transform_{};
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	GridState state_ = GridState::LockedUnavailable;

};

