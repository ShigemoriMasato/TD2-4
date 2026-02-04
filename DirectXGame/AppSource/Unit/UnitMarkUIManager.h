#pragma once
#include<vector>
#include"GameCamera/CameraController.h"
#include"UI/Object/SpriteObject.h"
#include"Game/MiniMap/MiniMap.h"

struct MarkUI {
	std::unique_ptr<SpriteObject> iconObject_;
	std::unique_ptr<SpriteObject> arrowObject_;
};

class UnitMarkUIManager {
public:
	struct Cluster {
		Vector3 positionSum; // 位置の合計
		int count;           // まとめた数
	};

public:


	void Initialize(DrawData spriteDrawData, int texture,int iconTex,int playerTex,int outLineIconTex);

	void Update(bool isCamera = true);

	void DrawUI(Window* window, const Matrix4x4& vpMatrix);

	void Reset();

	// カメラクラスを設定
	void SetCamera(CameraController* cameraController) {
		cameraController_ = cameraController;
	}

	// 衝突位置を取得
	void AddConflict(const Vector3& pos);

	// UI位置を設定
	void AddConflictMarkUI(const Vector3& pos);

	// プレイヤーの位置を設定
	void SetPlayerPos(const Vector3& pos);

	// ミニマップを作成
	void SetMinMap(MiniMap* miniMap) {
		miniMap_ = miniMap;
	}

private:
	// カメラクラス
	CameraController* cameraController_ = nullptr;

	// ミニマップ
	MiniMap* miniMap_ = nullptr;

	bool isCamera_ = true;

	// 描画データ
	DrawData spriteDrawData_;

	int arrowTex_ = 0;
	int iconTex_ = 0;
	int outLineIconTex_ = 0;

	// 衝突位置を取得
	std::vector<Vector3> conflictPosList_;

	// マーク画像
	std::vector<MarkUI> markUIList_;

	// プレイヤー用のマーク
	MarkUI playerMarkUI_;

	// アクティブ数
	int32_t activeIndex_ = 0;

	// マークUIの最大数
	int32_t maxNum_ = 10;

	// 一つにまとめる距離
	float mergeThreshold_ = 5.0f;

	float offsetRot_ = 0.0f;

	bool isPlayerDraw_ = false;

	float playerSize_ = 128.0f;
	float conflictSize_ = 200.0f;

	float playerMargin_ = 64.0f;
	float conflictMargin_ = 64.0f;

	float conflictOffsetY_ = -150.0f;

private:

	// 位置をまとめる
	void ProcessClusters();

	/// <summary>
	/// 値を登録する
	/// </summary>
	void RegisterDebugParam();

	/// <summary>
	/// 値を適応する
	/// </summary>
	void ApplyDebugParam();
};

// ヘルプ関数
namespace {
	float LengthSq(const Vector3& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}
}