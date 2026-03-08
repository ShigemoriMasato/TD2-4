#pragma once
#include "IAttackObject.h"

class Swing : public IAttackObject {
public:

	void Initialize(const Config& config) override;
	void Update(float deltaTime) override;
	DrawInfo GetDrawInfo() override;

private:

	std::unique_ptr<DirCircle> collDirCircle_;
	static constexpr float lifeTime_ = 0.4f;
	static constexpr float radius_ = 10.0f; // 攻撃の半径
	float timer_ = 0.0f;
	float direction_ = 0.0f;
	const static inline std::string modelPath_ = "Swing";
};
