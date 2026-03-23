#include "ItemEditor.h"

#include "ItemManager.h"
#include <Utility/ConvertString.h>
#include <Utility/SearchFile.h>
#include <imgui/imgui.h>
#include <algorithm>
#include <unordered_set>
#include <cfloat>
#include <filesystem>
#include <map>
#include <set>
#include <Utility/Easing.h>

namespace fs = std::filesystem;

namespace {
	using u32 = ImU32;
	ImU32 ColLerp(ImU32 begin, ImU32 end, float ratio) {
		u32 r1 = (begin >> 24) & 0xFF;
		u32 g1 = (begin >> 16) & 0xFF;
		u32 b1 = (begin >>  8) & 0xFF;
		u32 a1 = (begin >>  0) & 0xFF;

		u32 r2 = (end >> 24) & 0xFF;
		u32 g2 = (end >> 16) & 0xFF;
		u32 b2 = (end >>  8) & 0xFF;
		u32 a2 = (end >>  0) & 0xFF;

		u32 r = (u32)(r1 + (r2 - r1) * ratio) & 0xFF;
		u32 g = (u32)(g1 + (g2 - g1) * ratio) & 0xFF;
		u32 b = (u32)(b1 + (b2 - b1) * ratio) & 0xFF;
		u32 a = (u32)(a1 + (a2 - a1) * ratio) & 0xFF;

		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

void ItemEditor::DrawNode(Node* node) {
#ifdef USE_IMGUI
	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (node->children.empty()) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (node == selectedNode) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool opened = ImGui::TreeNodeEx(node, flags, "%s", node->name.c_str());

	if (ImGui::IsItemClicked()) {
		selectedNode = node;
	}

	if (opened) {
		for (auto& child : node->children) {
			DrawNode(child.get());
		}
		ImGui::TreePop();
	}
#endif
}

// モデルの候補を取得する
void ItemEditor::CreateItemFromModel(ItemManager& itemManager) {
	auto& items = itemManager.GetItemsForEdit();
	rootNode_->name = "Root";
	rootNode_->itemID = -1;

	//アイテム用のモデルをすべて取得する
	auto files = SearchDirectoryNames(basePath_);

	//モデルが存在しないアイテムを削除する
	std::vector<int> toRemove;
	for (auto& [id, item] : items) {
		bool found = false;
		for (const auto& f : files) {
			if (item.modelPath == basePath_ + f) {
				found = true;
				break;
			}
		}
		if (!found) {
			toRemove.push_back(id);
		}
	}

	for (int id : toRemove) {
		items.erase(id);
	}

	for (const auto& f : files) {
		fs::path p(f);
		Node* currentNode = rootNode_.get();

		fs::path path = *p.begin();

		// ルートからファイルまでのパスを順にたどる
		while (p != L".") {
			path = *p.begin();

			bool found = false;
			int index = 0;

			// currentNodeの子ノードの中から、pathと同じ名前のノードを探す
			for (size_t i = 0; i < currentNode->children.size(); ++i) {
				if (currentNode->children[i]->name == path.string()) {
					found = true;
					index = (int)i;
				}
			}

			//無ければ作る
			if (!found) {
				currentNode->children.push_back(std::make_unique<Node>());
				currentNode->children.back()->name = path.string();
				index = (int)currentNode->children.size() - 1;
			}

			// currentNodeを子ノードの方に進める
			auto parentNode = currentNode;
			currentNode = currentNode->children[index].get();
			currentNode->parent = parentNode;

			//次のパスに進める
			p = p.lexically_relative(path);
		}

		//この時点でcurrentNodeはファイルの末端を指しているので、それをItemとして扱う
		//pは空で、pathはファイル名のみになっている

		//アイテムが作成済みの可能性があるので、同じモデルパスのアイテムがあればidをセットしてcontinueする
		bool exists = false;
		for (auto& [id, item] : items) {
			if (item.modelPath == basePath_ + f) {
				currentNode->itemID = id;
				currentNode->name = path.string();
				item.name = ConvertString(path.string());
				exists = true;
				break;
			}
		}

		if (exists) {
			continue;
		}

		//新規作成
		Item item{};
		item.name = ConvertString(path.string());
		item.id = itemManager.GetUsedID()++;
		item.category = Category::Item;
		item.modelPath = basePath_ + f;
		item.modelID = itemManager.ResolveModelID(item);
		for (int r = 0; r < 4; ++r) {
			item.ranks[r].price = 0;
			item.ranks[r].effect = 0u;
			item.ranks[r].params = itemManager.GetBaseParamsForEdit();
		}

		//CurrentNodeのitemIDにセット
		currentNode->itemID = item.id;

		//ItemManagerに登録
		items[item.id] = std::move(item);
	}
}

void ItemEditor::Draw(ItemManager& itemManager) {
#ifdef USE_IMGUI

	auto& items = itemManager.GetItemsForEdit();
	auto& baseParam = itemManager.GetBaseParamsForEdit();
	int& nextID = itemManager.GetUsedID();

	static bool initialized = false;

	if (!initialized) {
		initialized = true;
	}
	CreateItemFromModel(itemManager);

	ImGui::Begin("Item Manager");

	//Item一覧と削除
	DrawNode(rootNode_.get());

	ImGui::Separator();

#pragma region アイテムの基本情報編集

	if (selectedNode && selectedNode->itemID != -1) {

		Item& currentItem = items[selectedNode->itemID];

		// 名前編集
		ImGui::Text("%s", ConvertString(currentItem.name).c_str());

		// カテゴリ編集
		ImGui::Combo("Category##2", reinterpret_cast<int*>(&currentItem.category), "Weapon\0Armor\0Item\0");

		ImGui::DragFloat2("Visual Offset Cells", &currentItem.visualOffsetCells.x, 0.01f);

		if (currentItem.category == Category::Weapon) {
			currentItem.weaponID = std::max(currentItem.weaponID, 0);
			ImGui::InputInt("Weapon ID", &currentItem.weaponID);
		} else {
			currentItem.weaponID = -1;
		}

		ImGui::Checkbox("Is Active", &currentItem.isActive);

#pragma endregion

#pragma region mapData（2Dタイル形状）編集

		if (ImGui::TreeNode("----------------Shape---------------")) {
			if (ImGui::Button("左上詰め")) {
				if (!currentItem.mapData.empty()) {
					// 最大値で初期化
					int minX = gridW_;
					int minY = gridH_;

					// 一番左上のセルを探す
					for (const auto& [x, y] : currentItem.mapData) {
						minX = std::min(minX, x);
						minY = std::min(minY, y);
					}
					// 全セルをずらす
					for (auto& c : currentItem.mapData) {
						c.first -= minX;
						c.second -= minY;
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("全削除")) {
				currentItem.mapData.clear();
			}
			ImGui::SameLine();

			ImGui::Text("合計セル数: %d", (int)currentItem.mapData.size());
			ImGui::Text("左クリック: セル追加   右クリック: セル削除");

			ImDrawList* dl = ImGui::GetWindowDrawList();
			// グリッドの原点（左上の座標）
			const ImVec2 origin = ImGui::GetCursorScreenPos();
			// グリッド全体のサイズ
			const ImVec2 gridSize(cellSize_ * gridW_, cellSize_ * gridH_);

			// セルのすべてをカバーするでかボタンを置く
			ImGui::InvisibleButton("##shape_grid", gridSize,
				ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

			// デカボタンの上にマウスがあるか
			const bool hovered = ImGui::IsItemHovered();

			if (hovered) {
				// 左クリックで塗りモードに設定
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))  paintMode_ = 1;
				// 右クリックで消しモードに設定
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) paintMode_ = 2;
			}

			// どちらのボタンも離されたらペイントモードをフラットな状態に戻す
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
				paintMode_ = 0;
			}

			// マウスがデカボタンの上にある && 編集モードになっている(クリックされている)
			if (hovered && paintMode_ != 0) {
				const ImVec2 mp = ImGui::GetIO().MousePos;
				const float localX = mp.x - origin.x;
				const float localY = mp.y - origin.y;
				const int cx = (int)(localX / cellSize_);
				const int cy = (int)(localY / cellSize_);

				if (0 <= cx && cx < gridW_ && 0 <= cy && cy < gridH_) {
					const std::pair<int, int> cell(cx, (gridH_ - 1) - cy);
					if (paintMode_ == 1) {
						// 塗りモード: セルを追加
						if (std::find_if(currentItem.mapData.begin(), currentItem.mapData.end(),
							[&cell](const std::pair<int, int>& c) { return c == cell; }) == currentItem.mapData.end()) {
							currentItem.mapData.emplace_back(cell);
						}
					} else if (paintMode_ == 2) {
						// 消しモード: セルを削除
						currentItem.mapData.erase(std::remove(currentItem.mapData.begin(), currentItem.mapData.end(), cell), currentItem.mapData.end());
					}
				}
			}

			// セルの色とグリッド線の色
			const ImU32 colBg = IM_COL32(30, 30, 30, 255);
			const ImU32 colGrid = IM_COL32(80, 80, 80, 255);
			const ImU32 colFillMin = IM_COL32(0, 0, 100, 255);
			const ImU32 colFillMax = IM_COL32(255, 255, 0, 255);
			const int chipNum = (int)currentItem.mapData.size();

			dl->AddRectFilled(origin, ImVec2(origin.x + gridSize.x, origin.y + gridSize.y), colBg);

			// 透明なボタンの上に実際にユーザーが見るグリッドを描画
			for (int y = 0; y < gridH_; ++y) {
				for (int x = 0; x < gridW_; ++x) {
					const ImVec2 p0(origin.x + x * cellSize_, origin.y + y * cellSize_);
					const ImVec2 p1(p0.x + cellSize_, p0.y + cellSize_);

					int index = -1;
					for(int i = 0; i < (int)currentItem.mapData.size(); ++i) {
						if (currentItem.mapData[i].first == x && currentItem.mapData[i].second == ((gridH_ - 1) - y)) {
							index = i + 1;
							break;
						}
					}

					if (index != -1) {
						ImU32 colFill = ColLerp(colFillMin, colFillMax, (float)index / std::max(1, chipNum));
						dl->AddRectFilled(p0, p1, colFill);
					}
					dl->AddRect(p0, p1, colGrid);
				}
			}

			ImGui::TreePop();
		}

#pragma endregion

#pragma region ランク毎価格

		static int rankTab = 0;
		rankTab = std::clamp(rankTab, 0, 3);

		if (ImGui::BeginTabBar("RankTabs")) {
			for (int r = 0; r < 4; ++r) {
				ImGui::PushID(r);
				char tabName[16];
				sprintf_s(tabName, "Rank %d", r + 1);

				if (ImGui::BeginTabItem(tabName)) {
					rankTab = r;
					ImGui::EndTabItem();
				}
				ImGui::PopID();
			}
			ImGui::EndTabBar();
		}

		ItemRankData& editRank = currentItem.ranks[rankTab];

		ImGui::DragInt("price", &editRank.price, 1.0f, 0, 999999);

#pragma endregion

#pragma region バフパラメータ編集

		if (paramNamesCStr_.size() != paramNames_.size()) {
			paramNamesCStr_.clear();
			paramNamesCStr_.reserve(paramNames_.size());

			for (const auto& name : paramNames_) {
				paramNamesCStr_.push_back(name.c_str());
			}
		}

		if (ImGui::TreeNode("----------------parm---------------")) {
			static int currentParamType = 0;

			if (ImGui::Button("Add")) {
				// 以降ランクに追加する方針に変更
				const std::string paramName = paramNames_[currentParamType];
				for (int r = rankTab; r < 4; ++r) {
					currentItem.ranks[r].params[paramName] = 0.0f;
				}
			}
			ImGui::SameLine();
			ImGui::Combo("ParamType", &currentParamType, paramNamesCStr_.data(), (int)paramNamesCStr_.size());

			ImGui::Text("Params:");

			std::string deleteParamName;
			bool doDelete = false;

			for (auto& [name, value] : editRank.params) {
				ImGui::Separator();

				ImGui::PushID(name.c_str());

				ImGui::Text("%s", name.c_str());

				ImGui::DragFloat("##Value", &value, 0.1f);
				ImGui::SameLine();
				if (ImGui::SmallButton("Delete")) {
					deleteParamName = name;
					doDelete = true;
				}

				ImGui::PopID();
			}

			if (doDelete && !deleteParamName.empty()) {
				editRank.params.erase(deleteParamName);
			}

			ImGui::TreePop();
		}

#pragma endregion

	}

	ImGui::End();

	ImGui::Begin("New Param");

	static char newParamName[256] = "";
	ImGui::InputText("Name", newParamName, sizeof(newParamName));
	ImGui::DragFloat("Default Value", &baseParamValue_, 0.1f);
	if (ImGui::Button("Add")) {
		auto it = baseParam.find(newParamName);

		if (it == baseParam.end()) {
			baseParam[newParamName] = baseParamValue_;
			paramNames_.push_back(newParamName);
			std::sort(paramNames_.begin(), paramNames_.end());

			baseParamValue_ = 0.0f;
			newParamName[0] = '\0';
		}
	}

	//paramNamesの準備
	if (paramNames_.size() != baseParam.size()) {
		paramNames_.clear();
		paramNames_.reserve(baseParam.size());

		for (const auto& [name, value] : baseParam) {
			paramNames_.push_back(name);
		}
	}

	ImGui::BeginChild("##BaseParamList", ImVec2(0, 200), true);

	for (int i = 0; i < (int)paramNames_.size(); ++i) {
		if (ImGui::Selectable(paramNames_[i].c_str(), currentParamType_ == i)) {
			currentParamType_ = i;
		}

		ImGui::SameLine();

		if (ImGui::Button(("Erase##BaseParam" + std::to_string(i)).c_str())) {
			baseParam.erase(paramNames_[i]);
			paramNames_.erase(paramNames_.begin() + i);
			--i;
		}
	}

	ImGui::EndChild();

	if (baseParam.size() > currentParamType_) {
		float& param = baseParam[paramNames_[currentParamType_]];
		ImGui::DragFloat("Slected Base Value", &param, 0.1f);
	}

	ImGui::End();
#endif
}
