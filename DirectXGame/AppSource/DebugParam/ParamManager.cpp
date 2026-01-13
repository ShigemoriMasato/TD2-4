#include"ParamManager.h"

ParamManager::ParamManager() {
	hierarchyWindow_ = std::make_unique<HierarchyWindow>();
	inspectorWindow_ = std::make_unique<InspectorWindow>();
}

void ParamManager::Draw() {
	hierarchyWindow_->Draw();
	inspectorWindow_->Draw();
}