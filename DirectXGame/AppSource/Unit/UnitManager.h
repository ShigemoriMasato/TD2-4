#pragma once
#include<unordered_map>
#include <deque>
#include"Common/KeyConfig/KeyManager.h"

#include"Unit/Player/PlayerUnit.h"
#include"Unit/Ore/OreUnit.h"

#include"UI/OreUnitHPUI.h"
#include"UnitEffectManager.h"

#include"Item/HomeManager.h"

class OreItem;

class UnitManager {
public:

	// 出現させるためのデータ
	struct SpawnData {
		uint32_t spawnNum = 1;  // 出現させる数
		uint32_t currentNum = 0; // 現在の出現数
		Vector3 pos = {}; // 出現位置
		OreItem* oreItem_ = nullptr;
	};

public:

	void Initalize(MapChipField* mapChipField, DrawData playerDrawData,int pIndex, DrawData oreDrawData,int oIndex, KeyManager* keyManager);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void DrawUI();

	/// <summary>
	/// ユニットを出動させる
	/// </summary>
	/// <param name="targetPos">移動する目標位置</param>
	/// <param name="excessNum">余剰の数</param>
	void RegisterUnit(const Vector3& targetPos,const int32_t& excessNum, OreItem* oreItem);

	// 出撃させるユニットの数
	int32_t GetUnitSpawnNum() const { return unitSpawnNum_; }

	// 最大の数
	int32_t GetMaxOreCount() const { return maxCurrentOreCount_; }

	// 現在出撃している数
	int32_t GetOreCount() const { return activeCount_; }

	// UIシステムを取得
	void SetUnitHp(OreUnitHPUI* oreUnitHPUI) {
		oreUnitHPUI_ = oreUnitHPUI;
	}

	// 演出システムを取得
	void SetUnitEffect(UnitEffectManager* unitEffectManager) {
		unitEffectManager_ = unitEffectManager;
	}

	// 拠点管理クラスを取得
	void SetHomeManager(HomeManager* home) {
		HomeManager_ = home;
	}

private:

	// マップ
	MapChipField* mapChipField_ = nullptr;

	OreUnitHPUI* oreUnitHPUI_ = nullptr;
	UnitEffectManager* unitEffectManager_ = nullptr;

	// 拠点管理クラス
	HomeManager* HomeManager_ = nullptr;

	// おれの描画データ
	DrawData oreDrawData_;
	int oreTexIndex_ = 0;

	// プレイヤー
	std::unique_ptr<PlayerUnit> playerUnit_;

	// おれ
	// <id, インスタンス>
	std::unordered_map<int32_t, std::unique_ptr<OreUnit>> oreUnits_;
	// 最大のおれの数
	int32_t maxOreCount_ = 100;
	// 現在出せる最大のおれの数
	int32_t maxCurrentOreCount_ = 20;
	// 現在のid
	int32_t currentId_ = 0;
	// 再利用可能id
	std::deque<int32_t> freeIndices_;

	// おれの出現位置
	std::vector<Vector3> homePosList_;

	int32_t activeCount_ = 0;

	// 出現するユニットをを登録
	std::deque<SpawnData> spawnList_;
	bool isSpawn_ = false;
	SpawnData spawnData_;
	float spawnTimer_ = 0.0f;

	// デバック用
	std::string kGroupName_ = "UnitManager";

private: // 調整項目

	// 出現させるユニットの数
	uint32_t unitSpawnNum_ = 1;

	// 出現させる時間間隔
	float spawnTime_ = 0.2f;

private:

	/// <summary>
	/// 登録されたユニットを出現させる
	/// </summary>
	void UnitSpawn();

	/// <summary>
	/// ユニットを追加
	/// </summary>
	/// <param name="targetPos">移動する位置</param>
	void AddOreUnit(const Vector3& targetPos, OreItem* oreItem);

	// 一番近い出現位置を求める
	Vector3 GetNearHomePos(const Vector3& targetPos);

private:

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

	// XZ上での距離を求める
	float DistanceXZ(const Vector3& v1, const Vector3& v2) {
		float dx = v1.x - v2.x;
		float dz = v1.z - v2.z;
		return dx * dx + dz * dz;
	}
}