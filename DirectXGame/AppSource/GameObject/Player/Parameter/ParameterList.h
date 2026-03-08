#pragma once
#include "ParameterData.h"
#include <vector>
#include <../Engine/Tool/Json/JsonManager.h>

enum class CharacterID {
	Warrior,
	Mage,
	Archer,

	Size // キャラクターの種類の数を表すための値
};

class ParameterList {
public:
	// 初期化処理
	void Initialize(JsonManager* jsonManager);

	// 指定したキャラクターIDに基づいてパラメータを取得する
	const ParameterData& GetParameterData(CharacterID characterID) const { return parameterData_[static_cast<int>(characterID)]; }

private:
	// パラメータの値を保存するリスト
	std::vector<ParameterData> parameterData_;

	// JsonManager
	JsonManager* jsonManager_;
};
