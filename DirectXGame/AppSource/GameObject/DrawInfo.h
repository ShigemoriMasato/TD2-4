#pragma once
#include <Utility/DataStructures.h>

struct DrawInfo {
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 scale = { 1.0f ,1.0f, 1.0f };
	float timer = 0.0f;
	int modelIndex = 0;
	uint32_t color = 0xffffffff;
};
