#pragma once
#include"HierarchyWindow.h"
#include"InspectorWindow.h"

class ParamManager {
public:
	ParamManager();

	// 描画処理
	void Draw();

private:

	std::unique_ptr<HierarchyWindow> hierarchyWindow_;

	std::unique_ptr<InspectorWindow> inspectorWindow_;
};