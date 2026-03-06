#include "AttackManager.h"

void AttackManager::Initialize() {
	attackObjects_.clear();
}

void AttackManager::Update(float deltaTime) {
	// 攻撃オブジェクトの更新
	for (auto& obj : attackObjects_) {
		if (obj->IsActive()) {
			obj->Update(deltaTime);
		}
	}

	// 非アクティブな攻撃オブジェクトを削除
	attackObjects_.erase(
		std::remove_if(attackObjects_.begin(), attackObjects_.end(),
			[](const std::unique_ptr<IAttackObject>& obj) { return !obj->IsActive(); }),
		attackObjects_.end());
}

void AttackManager::Draw(CmdObj* cmdObj) {
	for (const auto& obj : attackObjects_) {
		if (obj->IsActive()) {
			obj->Draw(cmdObj);
		}
	}
}

void AttackManager::AddObj(std::unique_ptr<IAttackObject> attackObject) {
	attackObjects_.emplace_back(std::move(attackObject));
}

std::vector<DrawInfo> AttackManager::GetAttackDrawInfos() const {
	std::vector<DrawInfo> di;
	for (const auto& obj : attackObjects_) {
		if (obj->IsActive()) {
			di.push_back(obj->GetDrawInfo());
		}
	}
	return di;
}
