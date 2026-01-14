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

// 入力変数
static char nameBuffer[256] = "";
static std::string statusMessage = "";

// ImGuiで表示する用のパラメータを管理する
struct DebugParameterVisitor {
	const std::string& itemName;
	explicit DebugParameterVisitor(const std::string& name) : itemName(name) {}

	void operator()(int32_t& value) const {
		ImGui::DragInt(itemName.c_str(), &value, 1);
	}

	void operator()(uint32_t& value) const {
		ImGui::DragScalar(itemName.c_str(), ImGuiDataType_U32, &value, 1.0f);
	}

	void operator()(float& value) const {
		ImGui::DragFloat(itemName.c_str(), &value, 0.01f);
	}

	void operator()(Vector2& value) const {
		ImGui::DragFloat2(itemName.c_str(), reinterpret_cast<float*>(&value), 0.01f);
	}

	void operator()(Vector3& value) const {
		ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(&value), 0.01f);
	}

	void operator()(Vector4& value) const {
		ImGui::ColorEdit4(itemName.c_str(), reinterpret_cast<float*>(&value));
	}

	void operator()(bool& value) const {
		ImGui::Checkbox(itemName.c_str(), &value);
	}

	void operator()(std::string& value) const {
		//ImGui::InputText(itemName.c_str(), value.data());
		ImGui::Text(itemName.c_str(), &value);
	}

	void operator()(std::map<std::string, uint32_t>& value) const {
		if (ImGui::TreeNode(itemName.c_str())) {
			InspectorWindow::EditTexture(value);
			ImGui::TreePop();
		}
	}

	// 対応出来ない型がきた場合の処理
	template<typename T>
	void operator()(T& value) const {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[%s] は未対応の型です", itemName.c_str());
	}
};

