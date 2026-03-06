#pragma once
#include "IAttackObject.h"
#include <GameObject/DrawInfo.h>

class AttackManager {
public:

	void Initialize();
	void Update(float deltaTime);
	void Draw(CmdObj* cmdObj);

	void AddObj(std::unique_ptr<IAttackObject> attackObject);

	std::vector<DrawInfo> GetAttackDrawInfos() const;

private:

	std::vector<std::unique_ptr<IAttackObject>> attackObjects_;

};
