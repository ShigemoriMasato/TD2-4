#include "WeaponRender.h"

void WeaponRender::Initialize(const Weapon::RenderData& renderData) {
	renderObject_ = ;
	renderObject_->Initialize();
	renderObject_->SetDrawData(renderData.drawData);
	renderObject_->psoConfig_.vs = "";
}
