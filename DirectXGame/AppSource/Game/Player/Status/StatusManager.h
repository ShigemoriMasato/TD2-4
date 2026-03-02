#pragma once
#include "Status.h"
#include "StatusModifier.h"
#include <../Engine/Tool/Json/JsonManager.h>
#include <unordered_map>

class StatusManager {
public:
	// コンストラクタ
	StatusManager();

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

private:
	// StatusTypeをキーにしたハッシュマップ
	std::unordered_map<StatusType, Status> statuses_;

	// JsonManager
	std::unique_ptr<JsonManager> jsonManager_ = nullptr;
};
