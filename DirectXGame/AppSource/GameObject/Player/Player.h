#pragma once
#include "../Map/MapInfo.h"
#include "../Player/Parameter/ParameterData.h"
#include "../Player/Parameter/ParameterList.h"
#include "Controller/IController.h"
#include "GameObject/EasingAnimation/AnimationBundle.h"
#include "State/IPlayerState.h"
#include "State/PlayerStateDash.h"
#include "State/PlayerStateNormal.h"
#include <Collision/Collider.h>
#include <Common/KeyConfig/KeyManager.h>
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

/// <summary>
/// プレイヤー
/// </summary>
namespace Player {
	enum class PartIndex { RightArm = 0, LeftArm = 1, RightLeg = 2, LeftLeg = 3, Body = 4, Count = 5 };

	class Base : public Collider {
	public:
		// 初期化（デフォルトキャラクターID: 0）
		void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, CharacterID characterID, ItemManager* itemManager);

		// 更新
		void Update(Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool>& key);
		// パラメータの更新
		void UpdateParameter(const std::vector<Piece*>& items);

		// 描画
		void Draw(CmdObj* cmdObj);

		// 状態管理関数
		void ChangeState(std::unique_ptr<IPlayerState> newState);

		// StateからアクセスするためのGetter等
		SHEngine::Input* GetInput() const { return input_; }
		Transform& GetTransform() { return transform_; }
		Vector3* GetPositionPtr() { return &transform_.position; }
		void SetTransform(Transform t) { transform_ = t; }
		float GetVelocity() const {
			// マーカーに移動している場合はmarkerSpeed_、AI移動の場合はaiSpeed_を返す
			if (controller_ && controller_->HasTarget()) {
				return markerSpeed_;
			}
			return aiSpeed_;
		}
		MapInfo GetMapInfo() const { return mapInfo_; }

		void SetPosition(Vector3 position) { transform_.position = position; }
		void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }

		float GetRotationSpeed() const { return rotationSpeed_; }

		void SetMapInfo(const MapInfo& mapInfo) { mapInfo_ = mapInfo; }

		// パラメータ取得関数。スペルミスを防ぐため、こちらを推奨
		float GetParameter(const std::string& paramName) const;
		// 全てのパラメータを取得する関数
		std::unordered_map<std::string, float> GetParameters() const { return parameterList_->GetAllParameters(); }

		// コントローラーの取得
		IController* GetController() const { return controller_; }

		// コントローラーの設定
		void SetController(IController* controller) { controller_ = controller; }

		// HPのアクセッサ
		float GetCurrentHP() const { return currentHP_; }
		float GetMaxHP() const { return maxHP_; }

		// ダメージ
		void Damage(float amount);

		// 回復
		void Heal(float amount);

		// Stateから呼び出す用のアニメーション更新関数
		void UpdateWalkAnimation(float deltaTime, bool isMoving);

		// ライトのパラメータSetter
		void SetDirectionalLight(DirectionalLight dirLight) { dirLight_ = dirLight; }

	private:
		// プレイヤーの移動制限
		void ClampPosition();

		// 接触時処理
		void OnCollision(Collider* other) override;

	private:
		// 描画用変数
		std::array<std::unique_ptr<SHEngine::RenderObject>, static_cast<int>(PartIndex::Count)> render_;

		// WVP行列
		Matrix4x4 wvp_;

		// 入力
		SHEngine::Input* input_ = nullptr;

		// Transform
		Transform transform_{};

		// 速度
		float aiSpeed_ = 5.0f;
		float markerSpeed_ = 6.5f;

		// 現在の状態
		std::unique_ptr<IPlayerState> currentState_ = nullptr;

		// VP行列
		Matrix4x4 vpMatrix_;

		// テクスチャのインデックス
		int textureIndex_;

		// 回転の滑らかさ
		float rotationSpeed_ = 10.0f;

		// マップの移動制限
		MapInfo mapInfo_;

		// パラメータ
		std::unique_ptr<ParameterList> parameterList_ = nullptr;

		// コントローラー
		IController* controller_ = nullptr;

		// HP
		float maxHP_ = 0.0f;
		float currentHP_ = 0.0f;
		float upperLimitHP_ = 999.0f; // HPの上限（バグ防止用）
		float lowerLimitHP_ = 1.0f;   // HPの下限（バグ防止用）

	// 無敵フラグ
	bool isInvincible_ = false;
	float invincibleTimer_ = 0.0f;
	float invincibleDuration_ = 0.1f;

		// アニメーション用変数
		AnimationBundle<Vector3> rotateAnimationRightArm_; // 右腕
		AnimationBundle<Vector3> rotateAnimationLeftArm_;  // 左腕
		AnimationBundle<Vector3> rotateAnimationRightLeg_; // 右脚
		AnimationBundle<Vector3> rotateAnimationLeftLeg_;  // 左脚

		// 各パーツのローカルTransform
		Transform partTransforms_[static_cast<int>(PartIndex::Count)];

		// 前回移動していたかどうかのフラグ
		bool wasMoving_ = false;

		std::unique_ptr<Circle> collCircle_ = nullptr;
		Logger logger_;

		DirectionalLight dirLight_;

#ifdef _DEBUG
		bool isDebugInvincible_ = false;
#endif

	};
} // namespace Player
