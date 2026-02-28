#include "ItemEditor.h"

#ifdef USE_IMGUI

#include "ItemManager.h"
#include <Utility/ConvertString.h>
#include <imgui/imgui.h>
#include <algorithm>
#include <unordered_set>

namespace
{
	struct PairHash
	{
		size_t operator()(const std::pair<int, int>& p) const noexcept
		{
			const uint64_t x = static_cast<uint32_t>(p.first);
			const uint64_t y = static_cast<uint32_t>(p.second);
			return static_cast<size_t>((x << 32) ^ y);
		}
	};
}

void ItemEditor::Draw(ItemManager& itemManager)
{
	auto& items = itemManager.GetItemsForEdit();
	auto& baseParam = itemManager.GetBaseParamsForEdit();

	ImGui::Begin("Item Manager");

#pragma region Item追加

	// 新規追加するアイテムの名前
	ImGui::InputText("NewItem Name", newItemName_, sizeof(newItemName_));
	// 新規追加するアイテムのカテゴリ
	ImGui::Combo("Category", &newItemCategory_, "Weapon\0Armor\0Item\0");

	bool empty = false;
	bool exists = false;

	// アイテム追加実行ボタン
	if (ImGui::Button("Add Item"))
	{
		// 入力欄に入っている名前をwstringに変換
		const std::wstring newNameW = ConvertString(std::string(newItemName_));

		// 空
		empty = newNameW.empty();

		// 重複
		exists = std::any_of(items.begin(), items.end(),
			[&](const Item& it) { return it.name == newNameW; });

		// 重複してたら失敗フラグをたてる
		if (exists)
		{
			addItemState_ = 1;
		}
		// 空だったら失敗フラグをたてる
		else if (empty)
		{
			addItemState_ = 2;
		}
		// 重複してないし空でもないならアイテム追加
		else
		{
			addItemState_ = 0;

			Item item{};
			item.name = newNameW;
			item.category = static_cast<Category>(newItemCategory_);
			item.effect = 0u;
			item.params.clear();
			items.push_back(std::move(item));
		}
	}

	if (addItemState_ == 1)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error : 既に存在する名前です");
	}
	else if (addItemState_ == 2)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error : 名前を入力してください");
	}

#pragma endregion

	ImGui::Separator();

#pragma region Item一覧と削除

	// アイテム削除した時とかにオーバーするかも
	currentItemIndex_ = std::clamp(currentItemIndex_, 0, static_cast<int>(items.size()) - 1);

	int deleteIndex = -1;

	// アイテム一覧表示
	if (ImGui::BeginTable("ItemTable", 2,
		ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY,
		ImVec2(0, 160.0f)))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("##Del", ImGuiTableColumnFlags_WidthFixed, 70.0f);

		for (int i = 0; i < static_cast<int>(items.size()); ++i)
		{
			ImGui::PushID(i);

			// テーブル更新
			ImGui::TableNextRow();

			// 名前表示
			ImGui::TableSetColumnIndex(0);
			const bool selected = (currentItemIndex_ == i);
			const std::string label = ConvertString(items[i].name);
			if (ImGui::Selectable(label.c_str(), selected))
			{
				currentItemIndex_ = i;
			}

			// Deleteボタン
			ImGui::TableSetColumnIndex(1);
			if (ImGui::SmallButton("Delete"))
			{
				deleteIndex = i;
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	// もし削除ボタンが押されてたらアイテム削除
	if (deleteIndex >= 0)
	{
		items.erase(items.begin() + deleteIndex);

		if (items.empty())
		{
			Item defaultItem{};
			defaultItem.name = L"Default Item";
			defaultItem.category = Category::Item;
			defaultItem.effect = 0u;
			defaultItem.params = baseParam;
			items.push_back(std::move(defaultItem));
		}

		if (currentItemIndex_ >= static_cast<int>(items.size()))
		{
			currentItemIndex_ = static_cast<int>(items.size()) - 1;
		}
	}

	Item& currentItem = items[currentItemIndex_];

#pragma endregion

	ImGui::Separator();

#pragma region アイテムの基本情報編集

	// 選択アイテムが変わったら編集用バッファを更新
	if (lastItemIndex_ != currentItemIndex_)
	{
		lastItemIndex_ = currentItemIndex_;

		addItemState_ = 0;

		const std::string curName = ConvertString(currentItem.name);
		strncpy_s(editItemName_, sizeof(editItemName_), curName.c_str(), _TRUNCATE);
	}

	// 名前編集
	ImGui::InputText("Name##Edit", editItemName_, sizeof(editItemName_));
	ImGui::SameLine();

	bool empty2 = false;
	bool exists2 = false;

	// リネームボタン
	if (ImGui::Button("Rename"))
	{
		const std::wstring newNameW = ConvertString(std::string(editItemName_));

		// 空
		empty2 = newNameW.empty();

		// 重複
		exists2 = std::any_of(items.begin(), items.end(),
			[&](const Item& it) { return it.name == newNameW && &it != &currentItem; });

		// 重複してたら失敗フラグをたてる
		if (exists2)
		{
			addItemState_ = 1;
		}
		// 空だったら失敗フラグをたてる
		else if (empty2)
		{
			addItemState_ = 2;
		}
		else
		{
			addItemState_ = 0;
			currentItem.name = newNameW;
		}
	}

	if (addItemState_ == 1)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error : 既に存在する名前です");
	}
	else if (addItemState_ == 2)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error : 名前を入力してください");
	}

	// カテゴリ編集
	ImGui::Combo("Category##2", reinterpret_cast<int*>(&currentItem.category), "Weapon\0Armor\0Item\0");

#pragma endregion

#pragma region mapData（2Dタイル形状）編集

	if (ImGui::TreeNode("----------------Shape---------------"))
	{
		ImGui::SeparatorText("Shape (mapData)");

		if (ImGui::Button("左上詰め"))
		{
			if (!currentItem.mapData.empty())
			{
				// 最大値で初期化
				int minX = gridW_;
				int minY = gridH_;

				// 一番左上のセルを探す
				for (const auto& [x, y] : currentItem.mapData)
				{
					minX = std::min(minX, x);
					minY = std::min(minY, y);
				}
				// 全セルをずらす
				for (auto& c : currentItem.mapData)
				{
					c.first -= minX;
					c.second -= minY;
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("全削除"))
		{
			currentItem.mapData.clear();
		}
		ImGui::SameLine();

		ImGui::Text("合計セル数: %d", (int)currentItem.mapData.size());
		ImGui::Text("左クリック: セル追加   右クリック: セル削除");

		std::unordered_set<std::pair<int, int>, PairHash> filled;
		filled.reserve(currentItem.mapData.size() * 2);
		for (const auto& c : currentItem.mapData)
		{
			filled.insert(c);
		}

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

		if (hovered)
		{
			// 左クリックで塗りモードに設定
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))  paintMode_ = 1;
			// 右クリックで消しモードに設定
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) paintMode_ = 2;
		}

		// どちらのボタンも離されたらペイントモードをフラットな状態に戻す
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			paintMode_ = 0;
		}

		// マウスがデカボタンの上にある && 編集モードになっている(クリックされている)
		if (hovered && paintMode_ != 0)
		{
			const ImVec2 mp = ImGui::GetIO().MousePos;
			const float localX = mp.x - origin.x;
			const float localY = mp.y - origin.y;
			const int cx = (int)(localX / cellSize_);
			const int cy = (int)(localY / cellSize_);

			if (0 <= cx && cx < gridW_ && 0 <= cy && cy < gridH_)
			{
				const std::pair<int, int> cell(cx, cy);
				if (paintMode_ == 1) filled.insert(cell);
				else if (paintMode_ == 2) filled.erase(cell);
			}
		}

		// セルの色とグリッド線の色
		const ImU32 colBg = IM_COL32(30, 30, 30, 255);
		const ImU32 colGrid = IM_COL32(80, 80, 80, 255);
		const ImU32 colFill = IM_COL32(120, 200, 120, 255);

		dl->AddRectFilled(origin, ImVec2(origin.x + gridSize.x, origin.y + gridSize.y), colBg);

		// 透明なボタンの上に実際にユーザーが見るグリッドを描画
		for (int y = 0; y < gridH_; ++y)
		{
			for (int x = 0; x < gridW_; ++x)
			{
				const ImVec2 p0(origin.x + x * cellSize_, origin.y + y * cellSize_);
				const ImVec2 p1(p0.x + cellSize_, p0.y + cellSize_);

				if (filled.contains({ x, y }))
				{
					dl->AddRectFilled(p0, p1, colFill);
				}
				dl->AddRect(p0, p1, colGrid);
			}
		}

		// 編集結果をItemのmapDataに反映
		currentItem.mapData.assign(filled.begin(), filled.end());
		std::sort(currentItem.mapData.begin(), currentItem.mapData.end());

		ImGui::TreePop();
	}

#pragma endregion

#pragma region バフパラメータ編集

	// 今後増えていくだろうに固定なのが心底悔やまれる
	static const char* names[] = { "HP", "MP", "Attack", "Defense", "Speed" };

	if (ImGui::TreeNode("----------------parm---------------"))
	{
		static int currentParamType = 0;

		if (ImGui::Button("Add"))
		{
			const std::string paramName = names[currentParamType];
			currentItem.params[paramName] = 0.0f;
		}
		ImGui::SameLine();
		ImGui::Combo("ParamType", &currentParamType, names, IM_ARRAYSIZE(names));

		ImGui::Text("Params:");

		std::string deleteParamName;
		bool doDelete = false;

		for (auto& [name, value] : currentItem.params)
		{
			ImGui::PushID(name.c_str());

			ImGui::DragFloat("##Value", &value, 0.1f);
			ImGui::SameLine();
			ImGui::Text("%s", name.c_str());
			ImGui::SameLine();

			if (ImGui::SmallButton("Delete"))
			{
				deleteParamName = name;
				doDelete = true;
			}

			ImGui::PopID();
		}

		if (doDelete && !deleteParamName.empty())
		{
			currentItem.params.erase(deleteParamName);
		}

		ImGui::TreePop();
	}
#pragma endregion

	ImGui::End();
}

#endif