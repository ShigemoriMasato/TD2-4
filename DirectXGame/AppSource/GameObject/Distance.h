#pragma once
#include <cmath>
#include <type_traits>

// ベクトルの距離を求めるテンプレート
template<typename T> float Distance(const T& a, const T& b) {
	float dx = static_cast<float>(a.x - b.x);
	float dy = static_cast<float>(a.y - b.y);

	// z成分があるかどうかのチェック
	if constexpr (requires {
		              a.z;
		              b.z;
	              }) {
		// zが存在する場合
		float dz = static_cast<float>(a.z - b.z);
		return std::sqrtf(dx * dx + dy * dy + dz * dz);
	} else {
		return std::sqrtf(dx * dx + dy * dy);
	}
}