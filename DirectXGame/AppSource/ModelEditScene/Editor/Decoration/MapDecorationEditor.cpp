#include "MapDecorationEditor.h"
#include <Utility/Color.h>
#include <Input/Input.h>
#include <Utility/SearchFile.h>
#include <GameCamera/DebugMousePos.h>

void MapDecorationEditor::Initialize(ModelManager* modelManager) {
	assert(modelManager);
	modelManager_ = modelManager;

	ModelListLoad();
	DecoDataLoad();
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
	//カーソルとデコレーションの当たり判定
	for (const auto& [modelIndex, transforms] : decorations_[currentMapID_]) {
		if (isHold_) {
			isHovered_ = true;
			break;
		}

		for (size_t i = 0; i < transforms.size(); ++i) {
			float distance = (Vector3(cursorPos_.x, 0.0f, cursorPos_.y) - transforms[i].position).Length();

			//カーソルと物体が重なっている
			if (distance <= radius_) {
				isHovered_ = true;

				//クリックしたら
				if (Input::GetMouseButtonState()[0] && !isHold_ && DebugMousePos::isHovered) {
					isHold_ = true;
					cursorOffset_ = Vector2(
						cursorPos_.x - transforms[i].position.x,
						cursorPos_.y - transforms[i].position.z
					);

					//編集中のモデル変更
					currentModelIndex_ = modelIndex;
					editingTransformIndex_ = static_cast<int>(i);
					break;
				}
			}
		}
	}

	if (!isHovered_ && Input::GetMouseButtonState()[0] && DebugMousePos::isHovered) {
		//モデルの新規追加
		editingTransformIndex_ = static_cast<int>(decorations_[currentMapID_][currentModelIndex_].size());
		decorations_[currentMapID_][currentModelIndex_].emplace_back();
		isHold_ = true;
	}

	if (isHold_ && editingTransformIndex_ != -1) {
		Transform& transform = decorations_[currentMapID_][currentModelIndex_][editingTransformIndex_];
		transform.position.x = cursorPos_.x - cursorOffset_.x;
		transform.position.z = cursorPos_.y - cursorOffset_.y;
	}
}

void MapDecorationEditor::PreviewDraw() {
	if (!editing_ || isHovered_ || currentModelIndex_ == -1) {
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
			editingTransformIndex_ = -1;
		}
	}

	int buttonNum = 0;
	for (const auto& [id, modelInfo] : modelList_) {
		++buttonNum;

		//Index取得
		int modelIndex = modelManager_->LoadModel(".Deco/" + modelInfo.first);

		ImGui::PushID(id);

		//ボタンの設定
		ImVec4 color = { modelInfo.second.x, modelInfo.second.y, modelInfo.second.z, 1.0f };
		ImVec2 size = ImVec2(50, 50);
		if (currentModelIndex_ == modelIndex) {
			size = ImVec2(60, 60);
		}

		//ボタンの描画
		if (ImGui::ColorButton(modelInfo.first.c_str(), color, 0, size)) {
			currentModelIndex_ = modelIndex;
			editingTransformIndex_ = -1;
		}

		ImGui::PopID();

		if (buttonNum % 8 != 0) {
			ImGui::SameLine();
		}
	}

	ImGui::End();

	ImGui::Begin("Debug");
	ImGui::Text("CursorPos: (%.2f, %.2f)", cursorPos_.x, cursorPos_.y);
	ImGui::End();


	//編集中じゃなかったり、編集対象がいなかったら描画しない
	if (!editing_ || editingTransformIndex_ < 0) {
		return;
	}
	//現在選択中のモデル表示
	ImGui::Begin("Current Decoration Model");

	int currentModelID = modelIDMap_[currentModelIndex_];
	Transform& transform = decorations_[currentMapID_][currentModelIndex_][editingTransformIndex_];
	const std::string& modelPath = modelList_[currentModelID].first;

	ImGui::Text("Model: %s", modelPath.c_str());
	ImGui::Separator();
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Position", &transform.position.x, 0.1f);

	if (ImGui::Button("Delete")) {
		decorations_[currentMapID_][currentModelIndex_].erase(decorations_[currentMapID_][currentModelIndex_].begin() + editingTransformIndex_);
		editingTransformIndex_ = -1;
	}

	transform.position.y = 0.0f; // Y軸固定
	ImGui::Separator();
	ImGui::Text("Model Config");
	ImGui::ColorEdit3("Color", &modelList_[currentModelID].second.x);

	ImGui::End();

	ImGui::Begin("Wall");

	static char buffer[256] = "";
	ImGui::InputText("File Name", buffer, sizeof(buffer));
	if (ImGui::Button("Load")) {
		int modelIndex = modelManager_->LoadModel(".Deco/" + std::string(buffer));
		if (modelIndex != 1) {
			modelIDMap_[modelIndex] = 0;
			modelList_[0] = { std::string(buffer), modelList_[0].second };
		}
	}

	ImGui::End();

#endif // USE_IMGUI

}

void MapDecorationEditor::Save() {
	for (const auto& [modelID, config] : modelList_) {
		binaryManager_.RegisterOutput(modelID);
		binaryManager_.RegisterOutput(config.first);
		binaryManager_.RegisterOutput(config.second);
	}
	binaryManager_.Write(saveFileNameModelList_);

	binaryManager_.RegisterOutput(static_cast<int>(decorations_.size()));
	for (const auto& mapDecorations : decorations_) {
		binaryManager_.RegisterOutput(static_cast<int>(mapDecorations.size()));
		for (const auto& [modelIndex, transforms] : mapDecorations) {
			int modelID = modelIDMap_[modelIndex];
			binaryManager_.RegisterOutput(modelID);
			binaryManager_.RegisterOutput(static_cast<int>(transforms.size()));
			for (const auto& transform : transforms) {
				binaryManager_.RegisterOutput(transform.position);
				binaryManager_.RegisterOutput(transform.rotate);
				binaryManager_.RegisterOutput(transform.scale);
			}
		}
	}
	binaryManager_.Write(saveFileNameDecorationData_);
}

void MapDecorationEditor::SetCurrentMap(int mapID) {
	currentMapID_ = mapID;
	if (currentMapID_ >= static_cast<int>(decorations_.size())) {
		decorations_.resize(currentMapID_ + 1);
	}
	editingTransformIndex_ = -1;
}

void MapDecorationEditor::GenerateWW(std::vector<std::vector<TileType>> mapData) {
	int height = static_cast<int>(mapData.size());
	int width = static_cast<int>(mapData[0].size());

	int index = 0;
	for (const auto& [modelID, modelInfo] : modelList_) {
		if (modelID == 0) {
			index = modelManager_->LoadModel(".Deco/" + modelInfo.first);
			break;
		}
	}

	decorations_[currentMapID_][index].clear();

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (mapData[i][j] == TileType::Wall) {
				Transform transform;
				transform.position = Vector3(static_cast<float>(j), 0.0f, static_cast<float>(i));
				transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
				transform.scale = Vector3(1.0f, 1.0f, 1.0f);
				decorations_[currentMapID_][index].push_back(transform);
			}
		}
	}
}

void MapDecorationEditor::DecoDataLoad() {
	auto values = binaryManager_.Read(saveFileNameDecorationData_);

	if (values.empty()) {
		return;
	}

	int index = 0;

	int mapCount = BinaryManager::Reverse<int>(values[index++].get());
	decorations_.resize(mapCount);

	for (int j = 0; j < mapCount; ++j) { //マップの数だけfor

		int decorationCount = BinaryManager::Reverse<int>(values[index++].get());

		for (int k = 0; k < decorationCount; ++k) {// モデルの数だけfor

			int modelID = BinaryManager::Reverse<int>(values[index++].get());
			
			int modelIndex = 0;
			for (const auto& [mIndex, mID] : modelIDMap_) {
				if (mID == modelID) {
					modelIndex = mIndex;
					break;
				}
			}

			int transformCount = BinaryManager::Reverse<int>(values[index++].get());
			std::vector<Transform> transforms;

			for (int i = 0; i < transformCount; ++i) { //配置された数だけfor
				Transform transform;
				transform.position = BinaryManager::Reverse<Vector3>(values[index++].get());
				transform.rotate = BinaryManager::Reverse<Vector3>(values[index++].get());
				transform.scale = BinaryManager::Reverse<Vector3>(values[index++].get());
				transforms.push_back(transform);
			}

			decorations_[j][modelIndex] = transforms;
		}

	}
}

void MapDecorationEditor::ModelListLoad() {
	auto values = binaryManager_.Read(saveFileNameModelList_);
	int index = 0;

	//ディレクトリにあるものを読み込み
	auto files = SearchFileNames("Assets/Model/.Deco/");

	//新しいものを追加するために、一番古いIDを取得する
	int oldestID = -1;

	//保存している分を読み込み、ディレクトリにあるものと結合
	while (index < static_cast<int>(values.size())) {
		int modelID = BinaryManager::Reverse<int>(values[index++].get());
		std::string modelPath = BinaryManager::Reverse<std::string>(values[index++].get());
		Vector3 color = BinaryManager::Reverse<Vector3>(values[index++].get());
		modelList_[modelID] = { modelPath, color };

		int modelIndex = modelManager_->LoadModel(".Deco/" + modelPath);
		modelIDMap_[modelIndex] = modelID;

		oldestID = std::max(oldestID, modelID);

		//ディレクトリにあるものを消す
		auto it = std::find(files.begin(), files.end(), modelPath);
		if (it != files.end()) {
			files.erase(it);
		}
	}

	//追加していないものを追加する
	for (const auto& file : files) {
		++oldestID;
		int modelIndex = modelManager_->LoadModel(".Deco/" + file);
		modelList_[oldestID] = { file, Vector3(1.0f, 1.0f, 1.0f) };
		modelIDMap_[modelIndex] = oldestID;
	}
}

int MapDecorationEditor::GetWallIndex() const {
	for (const auto& [modelID, modelInfo] : modelList_) {
		if (modelID == 0) {
			return modelManager_->LoadModel(".Deco/" + modelInfo.first);
		}
	}
	return 0;
}
