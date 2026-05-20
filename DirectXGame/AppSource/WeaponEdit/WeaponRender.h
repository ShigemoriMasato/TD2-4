#pragma once
#include <Render/RenderObject.h>
#include "Weapon.h"

class WeaponRender {
public:

	WeaponRender() = default;
	~WeaponRender() = default;

	void Initialize(const Weapon::RenderData& renderData);

private:

	std::unique_ptr<SHEngine::RenderObject> renderObject_ = std::make_unique<SHEngine::RenderObject>();
	
};
