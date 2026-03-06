#pragma once
#include "StatusModifier.h"
#include <vector>

class Status {
public:
	// デフォルトコンストラクタ
	Status() : baseValue_(0.0f), isDirty_(true), lastCalculatedValue_(0.0f) {}

	// コンストラクタ
	explicit Status(float baseValue) : baseValue_(baseValue), isDirty_(true), lastCalculatedValue_(0.0f) {}

	// 現在のステータス値を取得する
	float GetValue() {
		if (isDirty_) {
			lastCalculatedValue_ = CalculateFinalValue();
			isDirty_ = false;
		}
		return lastCalculatedValue_;
	}

	// Modifierを追加
	void AddModifier(const StatusModifier& mod);

	// 指定したソース（アイテムなど）由来のModifierをすべて削除する関数
	void RemoveModifierFromSource(const void* source);

public:
	float baseValue_;

private:
	// 最終的な数値を計算する関数
	float CalculateFinalValue() const {
		float finalValue = baseValue_;
		float percentAddSum = 0.0f;

		for (const auto& mod : modifiers_) {
			if (mod.type_ == ModifierType::Flat) {
				finalValue += mod.value_;
			} else if (mod.type_ == ModifierType::PercentAdd) {
				percentAddSum += mod.value_;
			} else if (mod.type_ == ModifierType::PercentMult) {
				finalValue *= (1.0f + mod.value_);
			}
		}

		finalValue *= (1.0f + percentAddSum);

		// 計算結果を返す
		return finalValue;
	}

private:
	// モディファイアーのリスト
	std::vector<StatusModifier> modifiers_;
	bool isDirty_;
	float lastCalculatedValue_;
};
