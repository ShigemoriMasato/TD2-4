#pragma once
#include "Object/FontObject.h"

class Number {
public:

	void Initialize(const std::string& fontName, DrawData drawData, FontLoader* fontLoader);
	void Update(float deltaTime);
	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void SetNumber(int number);
	void SetTransform(const Transform& transform) { transform_ = transform; }

	void SetColor(uint32_t defaultColor, uint32_t highColor) { defaultColor_ = defaultColor, highColor_ = highColor; };

	Transform transform_{};
private:

	std::unique_ptr<FontObject> fontObject_ = nullptr;
	int currentNumber_ = 0;

	float scale_ = 1.0f;

	struct ScalingOrder {
		float scaleT_ = 0.0f;
	};
	std::vector<ScalingOrder> scalingOrders_;

	uint32_t defaultColor_ = 0xffffffff;
	uint32_t highColor_ = 0xffffffff;
};
