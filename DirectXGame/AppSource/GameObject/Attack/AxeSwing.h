#pragma once
#include "IAttackObject.h"

class AxeSwing : public IAttackObject {
public:
	void Initialize(const Config& config) override;
	void Update(float deltaTime) override;
	DrawInfo GetDrawInfo() override;

private:
	std::unique_ptr<DirCircle> collDirCircle_;
	static constexpr float lifeTime_ = 0.5f;
	static constexpr float radius_ = 12.0f;
	float timer_ = 0.0f;
	float direction_ = 0.0f;
	const static inline std::string modelPath_ = "Swing";
};