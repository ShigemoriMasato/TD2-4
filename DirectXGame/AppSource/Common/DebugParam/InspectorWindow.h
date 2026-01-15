#pragma once
#include <imgui/imgui.h>
#include "GameParamEditor.h"

class InspectorWindow {
public:
	InspectorWindow();

	void Draw();
	std::string GetName() const { return "ParameterInspector"; }

	static void EditTexture(std::map<std::string, uint32_t>& value);
};
