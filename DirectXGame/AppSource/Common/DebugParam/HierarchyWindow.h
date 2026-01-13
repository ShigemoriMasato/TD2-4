#pragma once
#include <imgui/imgui.h>
#include<string>

class HierarchyWindow {
public:

    void Draw();
    std::string GetName() const { return "ParameterHierarchy"; }
};

