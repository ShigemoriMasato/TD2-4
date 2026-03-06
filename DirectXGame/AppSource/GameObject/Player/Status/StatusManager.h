#pragma once
#include "Status.h"
#include "StatusModifier.h"
#include <../Engine/Tool/Json/JsonManager.h>
#include <unordered_map>
#include <string>

// キャラクターID型
using CharacterID = int;

class StatusManager {
public:
	// コンストラクタ
	StatusManager();

	// 指定したキャラクターIDで初期化
	void Initialize(CharacterID characterID);

	// ステータスの現在値を取得
	float GetStatusValue(StatusType type) {
		auto it = statuses_.find(type);
		if (it != statuses_.end()) {
			return it->second.GetValue();
		}
		return 0.0f;
	}

	// 特定のステータスにModifierを追加
	void AddModifier(StatusType type, const StatusModifier& modifier);

	// アイテムを外したとき、全ステータスからそのアイテム由来のModifierを消す
	void RemoveModifierFromItem(const void* itemSource);

	// 現在のキャラクターIDを取得
	CharacterID GetCurrentCharacterID() const { return currentCharacterID_; }

private:
	// キャラクターのステータスをJSONから読み込む
	void LoadCharacterStatus(CharacterID characterID);

	// StatusTypeをキーにしたハッシュマップ
	std::unordered_map<StatusType, Status> statuses_;

	// JsonManager
	std::unique_ptr<JsonManager> jsonManager_ = nullptr;

	// 現在のキャラクターID
	CharacterID currentCharacterID_ = -1;
};
