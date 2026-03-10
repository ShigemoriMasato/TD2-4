#include "ParameterList.h"

void ParameterList::Initialize(ItemManager* itemManager) {
	auto param = itemManager->GetBaseParamsForEdit();
	for (const auto& [key, value] : param) {
		parameters_[key] = value;
	}
}

void ParameterList::Update(const std::vector<Piece*>& pieces) {
	parameters_ = baseParams; // 基礎値でリセット
	for (const auto& piece : pieces) {
		auto item = piece->GetItem();
		int rank = piece->GetRank();
		auto params = item.ranks[rank].params;

		for (const auto& [key, value] : params) {
			parameters_[key] += value; // 例: maxHPを加算
		}
	}
}
