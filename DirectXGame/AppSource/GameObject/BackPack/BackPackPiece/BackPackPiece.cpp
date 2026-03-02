#include "BackPackPiece.h"

BackPackPiece::BackPackPiece()
{}

BackPackPiece::~BackPackPiece()
{}

void BackPackPiece::Initialize(GridState state)
{
	ChangeState(state);
}

void BackPackPiece::ChangeState(GridState newState)
{
	switch (newState)
	{
	// ロック中・解放不可
	case GridState::LockedUnavailable:
		transform_.scale = { 0.0f, 0.0f, 0.0f };
		color_ = { 0.25f, 0.25f, 0.25f, 1.0f };
		break;
	// ロック中・解放可能
	case GridState::LockedAvailable:
		transform_.scale = { 0.9f, 0.1f, 0.9f };
		transform_.position.y = -0.1f;
		color_ = { 0.5f, 0.5f, 0.5f, 1.0f };
		break;
	// 解放済み・空
	case GridState::UnlockedEmpty:
		transform_.scale = { 0.9f, 0.1f, 0.9f };
		transform_.position.y = 0.0f;
		color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		break;
	// 解放済み・アイテムあり
	case GridState::UnlockedOccupied:
		transform_.scale = { 0.9f, 0.1f, 0.9f };
		transform_.position.y = 0.0f;
		color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		break;
	default:
		break;
	}
	state_ = newState;
}