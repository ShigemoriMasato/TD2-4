#pragma once
#include "IAttackObject.h"
#include <GameObject/DrawInfo.h>

class AttackManager {
public:
	void Initialize(SHEngine::ModelManager* modelManager);
	void Update(float deltaTime);
	void Draw(CmdObj* cmdObj);

	void AddObj(std::unique_ptr<IAttackObject> attackObject);

	std::vector<DrawInfo> GetAttackDrawInfos() const;

	float GetAttackObjCount() {
		if (!attackObjects_.empty()) {
			return static_cast<float>(attackObjects_.size());
		}
		return 0.0f;
	}

private:
	std::vector<std::unique_ptr<IAttackObject>> attackObjects_;

	int nextID_ = 0;
};
