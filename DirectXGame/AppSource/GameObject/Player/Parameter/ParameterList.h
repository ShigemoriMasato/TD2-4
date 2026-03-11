#pragma once
#include "ParameterData.h"
#include <vector>
#include <Shop/Piece.h>
#include <GameObject/Item/ItemManager.h>
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
	void Initialize(ItemManager* itemManager);

	void Update(const std::vector<Piece*>& items);
	// パラメータの値を取得する関数
	float GetParameter(const std::string& paramName) const {
		auto it = parameters_.find(paramName);
		if (it != parameters_.end()) {
			return it->second;
		}
		return 0.0f; // パラメータが見つからない場合は0を返す
	}
	std::unordered_map<std::string, float> GetAllParameters() const { return parameters_; }
	
private:

	std::unordered_map<std::string, float> baseParams;
	std::unordered_map<std::string, float> parameters_;

};
