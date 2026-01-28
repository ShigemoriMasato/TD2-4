#include "MapDecorationEditor.h"
#include <Utility/Color.h>
#include <Input/Input.h>

void MapDecorationEditor::Initialize(ModelManager* modelManager) {
	assert(modelManager);
	modelManager_ = modelManager;

	DevoDataLoad();
	ModelListLoad();
}

void MapDecorationEditor::Update() {
	someSelected_ = false;

	if (!editing_) {
		return;
	}

	if (!Input::GetMouseButtonState()[0]) {
		isHold_ = false;
	}

	isHovered_ = false;
	for (size_t i = 0; i < decorations_.size(); ++i) {
		if (isHold_) {
			isHovered_ = true;
			break;
		}

		float distance = (Vector3(cursorPos_.x, 0.0f, cursorPos_.y) - decorations_[i].first.position).Length();

		//カーソルと物体が重なっている
		if (distance <= radius_) {
			isHovered_ = true;

			//クリックしたら
			if (Input::GetMouseButtonState()[0] && !isHold_) {
				isHold_ = true;
				cursorOffset_ = Vector2(
					cursorPos_.x - decorations_[i].first.position.x,
					cursorPos_.y - decorations_[i].first.position.z
				);

				//編集中のモデル変更
				currentModelID_ = decorations_[i].second;
				editingModelIndex_ = static_cast<int>(i);
			}
			break;
		}
	}

	if (!isHovered_ && Input::GetMouseButtonState()[0]) {
		
		//モデルの新規追加
		std::pair<Transform, int>& newDecoration = decorations_.emplace_back();
		newDecoration.first.position = Vector3(
			cursorPos_.x,
			0.0f,
			cursorPos_.y
		);
		newDecoration.second = currentModelID_;

		editingModelIndex_ = static_cast<int>(decorations_.size() - 1);

		
	}

	if (isHold_) {
		Transform& transform = decorations_[editingModelIndex_].first;
		transform.position.x = cursorPos_.x - cursorOffset_.x;
		transform.position.z = cursorPos_.y - cursorOffset_.y;
	}
}

void MapDecorationEditor::PreviewDraw() {
	if (!editing_ || isHovered_ || currentModelID_ == -1) {
		return;
	}
}

void MapDecorationEditor::DrawImGui() {
#ifdef USE_IMGUI

	if (ImGui::Begin("Decoration")) {
		//編集モード開始
		if (!editing_) {
			editing_ = true;
			someSelected_ = true;
			editingModelIndex_ = -1;
		}
	}

	//Model一覧表示
	ImGui::BeginChild("ModelList", ImVec2(600, 0), true);

	int buttonNum = 0;
	for (const auto& [id, modelInfo] : modelList_) {
		++buttonNum;
		ImGui::PushID(id);

		//ボタンの設定
		ImVec4 color = { modelInfo.second.x, modelInfo.second.y, modelInfo.second.z, 1.0f };
		ImVec2 size = ImVec2(50, 50);
		if (currentModelID_ == id) {
			size = ImVec2(60, 60);
		}

		//ボタンの描画
		if (ImGui::ColorButton(modelInfo.first.c_str(), color, 0, size)) {
			currentModelID_ = id;
		}

		ImGui::PopID();

		if (buttonNum % 8 != 0) {
			ImGui::SameLine();
		}
	}

	ImGui::EndChild();

	ImGui::End();


	if (!editing_ || editingModelIndex_ < 0) {
		return;
	}
	//現在選択中のモデル表示
	ImGui::Begin("Current Decoration Model");

	int modelID = decorations_[editingModelIndex_].second;
	Transform& transform = decorations_[editingModelIndex_].first;
	const std::string& modelPath = modelList_[modelID].first;

	ImGui::Text("Model: %s", modelPath.c_str());
	ImGui::Separator();
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
	transform.position.y = 0.0f; // Y軸固定
	ImGui::Separator();
	ImGui::Text("Model Config");
	ImGui::ColorEdit3("Color", &modelList_[modelID].second.x);

	ImGui::End();

#endif // USE_IMGUI

}

void MapDecorationEditor::Save() {
}

void MapDecorationEditor::DevoDataLoad() {
}

void MapDecorationEditor::ModelListLoad() {
}
