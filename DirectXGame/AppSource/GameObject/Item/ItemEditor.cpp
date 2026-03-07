#include "ItemEditor.h"

#ifdef USE_IMGUI

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



namespace
{
	constexpr const char* kModelRoot = "Assets/Model";

	/// <summary>
	/// s が prefix で始まっているかを判定する関数
	/// </summary>
	/// <param name="s"> 判定対象の文字列 </param>
	/// <param name="prefix"> 先頭についててほしい文字列 </param>
	/// <returns> s が prefix で始まっているなら true、そうでないなら false </returns>
	static bool StartsWith(const std::string& s, const std::string& prefix)
	{
		bool sizeOk = s.size() >= prefix.size();
		if (!sizeOk) return false;

		bool equal = std::equal(prefix.begin(), prefix.end(), s.begin());
		if (!equal) return false;

		return true;
	}

	/// <summary>
	/// "Assets/Model/Weapon/sword.fbx" → "Weapon/sword.fbx" のように kModelRoot を削除した文字列を返す関数
	/// </summary>
	/// <param name="fullPath"> 例："Assets/Model/Weapon/sword.fbx" </param>
	/// <returns> 例："Weapon/sword.fbx" </returns>
	static std::string ToModelRelativePath(const std::string& fullPath)
	{
		std::string p = fullPath;
		// std::filesystemでパスを取得すると'\\'になることがあるため'/'に置換
		std::replace(p.begin(), p.end(), '\\', '/');

		// kModelRoot を削除
		const std::string root = std::string(kModelRoot) + "/";
		if (StartsWith(p, root))
		{
			p.erase(0, root.size());
		}
		return p;
	}

	/// <summary>
	/// "Assets/Model/Weapon/sword" → {"Assets", "Model", "Weapon", "sword"} のようにパスを分割する関数
	/// </summary>
	/// <param name="path"> 例："Assets/Model/Weapon/sword" </param>
	/// <returns> 例：{"Assets", "Model", "Weapon", "sword"} </returns>
	static std::vector<std::string> SplitPath(const std::string& path)
	{
		std::vector<std::string> parts;
		std::string cur;

		for (char ch : path)
		{
			if (ch == '/' || ch == '\\')
			{
				if (!cur.empty())
				{
					parts.push_back(cur);
					cur.clear();
				}
				continue;
			}
			cur.push_back(ch);
		}
		if (!cur.empty())
		{
			parts.push_back(cur);
		}
		return parts;
	}

	/// <summary>
	/// モデルファイル（.fbx, .obj, .gltf, .glb）が存在するフォルダ一覧を作る。
	/// </summary>
	/// <param name="outCandidates"></param>
	static void BuildModelCandidates(std::vector<std::string>& outCandidates)
	{
		static const std::vector<std::string> exts = { ".fbx", ".obj", ".gltf", ".glb" };

		// モデルファイルが存在するディレクトリ配列
		std::set<std::string> dirs;

		for (const auto& ext : exts)
		{
			// kModelRoot 以下の全てのファイルから、拡張子が ext のもののパスを取得
			const auto relFiles = SearchFilePathsAddChild(kModelRoot, ext);

			// 
			for (auto rel : relFiles)
			{
				// std::filesystemでパスを取得すると'\\'になることがあるため'/'に置換
				std::replace(rel.begin(), rel.end(), '\\', '/');

				// 例："Demo/Weapon/sword/sword_short/model.obj" なら "Demo/Weapon/sword/sword_short" 
				const auto pos = rel.find_last_of('/');
				if (pos == std::string::npos)
				{
					continue;
				}
				const std::string relDir = rel.substr(0, pos);

				// item.modelPath は "Assets/Model/...." を想定
				const std::string fullDir = std::string(kModelRoot) + "/" + relDir;
				dirs.insert(fullDir);
			}
		}

		outCandidates.assign(dirs.begin(), dirs.end());
		std::sort(outCandidates.begin(), outCandidates.end());
	}

	// Treeノード用構造体
	struct PathNode
	{
		// 例： "Demo/Weapon/sword/sword_short/model.obj" なら "Demo" -> "Weapon" -> "sword" -> "sword_short"
		std::map<std::string, PathNode> children;
		// そのモデルを使っているアイテムのインデックス　(インデックスとはItemManager::itemsの添字のこと)
		std::vector<int> itemIndices;
	};

	static void InsertItemByModelPath(PathNode& root, const std::string& modelPathRel, int itemIndex)
	{
		auto parts = SplitPath(modelPathRel);
		PathNode* node = &root;
		for (const auto& part : parts)
		{
			node = &node->children[part];
		}
		node->itemIndices.push_back(itemIndex);
	}

	static void DrawPathTree(
		PathNode& node,
		std::vector<Item>& items,
		int& currentItemIndex,
		int& deleteIndex)
	{
		// 子フォルダ
		for (auto& [name, child] : node.children)
		{
			const bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
			if (open)
			{
				DrawPathTree(child, items, currentItemIndex, deleteIndex);
				ImGui::TreePop();
			}
		}

		// この階層のリーフアイテム
		for (int idx : node.itemIndices)
		{
			ImGui::PushID(idx);
			
			const float delW = 70.0f;
			const float spacing = ImGui::GetStyle().ItemSpacing.x;

			// 行の開始Xと、行で使える幅
			const float startX = ImGui::GetCursorPosX();
			const float availW = ImGui::GetContentRegionAvail().x;

			// SelectableがDelete領域に被らないように幅を制限
			const float selectableW = std::max(1.0f, availW - delW - spacing);

			const bool selected = (currentItemIndex == idx);
			const std::string label = ConvertString(items[idx].name);

			// 1) 左側：Selectable（幅固定）
			if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_None, ImVec2(selectableW, 0.0f)))
			{
				currentItemIndex = idx;
			}

			// 2) 右側：Delete（Selectableの右に固定配置）
			ImGui::SameLine(startX + selectableW + spacing);
			if (ImGui::SmallButton("Delete"))
			{
				deleteIndex = idx;
			}

			ImGui::PopID();
		}
	}
}


void ItemEditor::Draw(ItemManager& itemManager)
{
	auto& items = itemManager.GetItemsForEdit();
	auto& baseParam = itemManager.GetBaseParamsForEdit();

	ImGui::Begin("Item Manager");

#pragma region Item追加

	// 新規追加するアイテムの名前
	ImGui::InputText("追加アイテムName", newItemName_, sizeof(newItemName_));
	// 新規追加するアイテムのカテゴリ
	ImGui::Combo("追加アイテムCategory", &newItemCategory_, "Weapon\0Armor\0Item\0");

	// モデル候補をLazy生成（起動直後/必要時のみ）
	BuildModelCandidates(modelCandidates_);

	// 現在値のプレビュー
	std::string preview = newItemModelPath_;
	preview = ToModelRelativePath(preview);
	if (preview.empty())
	{
		preview = "モデルパスを設定してね";
	}


	if (ImGui::BeginCombo("追加アイテムModelPath", preview.c_str()))
	{
		for (const auto& fullDir : modelCandidates_)
		{
			const std::string rel = ToModelRelativePath(fullDir);

			const bool selected = (std::string(newItemModelPath_) == fullDir);
			if (ImGui::Selectable(rel.c_str(), selected))
			{
				strncpy_s(newItemModelPath_, sizeof(newItemModelPath_), fullDir.c_str(), _TRUNCATE);
			}
		}

		ImGui::EndCombo();
	}

	bool nameEmpty = false;
	bool nameExists = false;
	bool modelPathEmpty = false;

	// アイテム追加実行ボタン
	if (ImGui::Button("Add Item"))
	{
		// 入力欄に入っている名前をwstringに変換
		const std::wstring newNameW = ConvertString(std::string(newItemName_));

		// 空
		nameEmpty = newNameW.empty();

		// 重複
		nameExists = std::any_of(items.begin(), items.end(),
			[&](const Item& it) { return it.name == newNameW; });

		// モデルパス空
		modelPathEmpty = (preview == "モデルパスを設定してね");

		// 空だったら失敗フラグをたてる
		if (nameEmpty)
		{
			addItemState_ = 1;
		}
		// 重複してたら失敗フラグをたてる
		else if (nameExists)
		{
			addItemState_ = 2;
		}
		// モデルパス空だったら失敗フラグをたてる
		else if (modelPathEmpty)
		{
			addItemState_ = 3;
		}
		// 重複してないし空でもないならアイテム追加
		else
		{
			addItemState_ = 0;

			Item item{};
			item.name = newNameW;
			item.category = static_cast<Category>(newItemCategory_);
			item.modelPath = newItemModelPath_;
			for (int r = 0; r < 4; ++r)
			{
				item.ranks[r].price = 0;
				item.ranks[r].effect = 0u;
				item.ranks[r].params = baseParam;
			}

			items.push_back(std::move(item));
		}
	}

	if (addItemState_ == 1)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error : 名前を入力してください");
	}
	else if (addItemState_ == 2)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error : 既に存在する名前です");
	}
	else if (addItemState_ == 3)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error : モデルパスを設定してください");
	}

#pragma endregion

	ImGui::Separator();

#pragma region Item一覧と削除

	// アイテム削除した時とかにオーバーするかも
	currentItemIndex_ = std::clamp(currentItemIndex_, 0, static_cast<int>(items.size()) - 1);

	int deleteIndex = -1;

	// modelPathベースのTree表示（Assets/Modelを共通基盤にして相対化）
	PathNode treeRoot;
	for (int i = 0; i < static_cast<int>(items.size()); ++i)
	{
		std::string mp = items[i].modelPath;
		std::replace(mp.begin(), mp.end(), '\\', '/');

		// "Assets/Model/..." 以外が入っててもツリーには乗せる（その場合はそのまま表示）
		std::string rel = ToModelRelativePath(mp);
		if (rel.empty())
		{
			// Assets/Model 直下扱い
			rel = "(root)";
		}

		InsertItemByModelPath(treeRoot, rel, i);
	}

	ImGui::BeginChild("ItemTreeList", ImVec2(0, 160.0f), true);
	DrawPathTree(treeRoot, items, currentItemIndex_, deleteIndex);
	ImGui::EndChild();

	// 削除（既存ロジック維持）
	if (deleteIndex >= 0)
	{
		items.erase(items.begin() + deleteIndex);

		if (items.empty())
		{
			Item defaultItem{};
			defaultItem.name = L"Default Item";
			defaultItem.category = Category::Item;

			for (int r = 0; r < 4; ++r)
			{
				defaultItem.ranks[r].price = 0;
				defaultItem.ranks[r].effect = 0u;
				defaultItem.ranks[r].params = baseParam;
			}

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

	// 選択アイテム変更時の初期化
	if (lastItemIndex_ != currentItemIndex_)
	{
		lastItemIndex_ = currentItemIndex_;

		addItemState_ = 0;

		const std::string curPath = currentItem.modelPath;
		strncpy_s(editModelPath_, sizeof(editModelPath_), curPath.c_str(), _TRUNCATE);

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

	// モデルパス編集
	if (ImGui::InputText("Model Path", editModelPath_, sizeof(editModelPath_)))
	{
		currentItem.modelPath = editModelPath_;
	}
#pragma endregion

#pragma region mapData（2Dタイル形状）編集

	if (ImGui::TreeNode("----------------Shape---------------"))
	{
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

		std::unordered_set<Vector2int, Vector2intHash> filled;
		filled.reserve(currentItem.mapData.size() * 2);
		for (const auto& c : currentItem.mapData)
		{
			filled.insert({ c.first, c.second });
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
				const Vector2int cell(cx, cy);
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
		currentItem.mapData.clear();
		for (const auto& cell : filled)
		{
			currentItem.mapData.emplace_back(cell.x, cell.y);
		}
		std::sort(currentItem.mapData.begin(), currentItem.mapData.end());

		ImGui::TreePop();
	}

#pragma endregion

#pragma region ランク毎価格

	static int rankTab = 0;
	rankTab = std::clamp(rankTab, 0, 3);

	if (ImGui::BeginTabBar("RankTabs"))
	{
		for (int r = 0; r < 4; ++r)
		{
			ImGui::PushID(r);
			char tabName[16];
			sprintf_s(tabName, "Rank %d", r + 1);

			if (ImGui::BeginTabItem(tabName))
			{
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

	// 今後増えていくだろうに固定なのが心底悔やまれる
	static const char* names[] = { "HP", "MP", "Attack", "Defense", "Speed" };

	if (ImGui::TreeNode("----------------parm---------------"))
	{
		static int currentParamType = 0;

		if (ImGui::Button("Add"))
		{
			// 以降ランクに追加する方針に変更
			const std::string paramName = names[currentParamType];
			for (int r = rankTab; r < 4; ++r)
			{
				currentItem.ranks[r].params[paramName] = 0.0f;
			}
		}
		ImGui::SameLine();
		ImGui::Combo("ParamType", &currentParamType, names, IM_ARRAYSIZE(names));

		ImGui::Text("Params:");

		std::string deleteParamName;
		bool doDelete = false;

		for (auto& [name, value] : editRank.params)
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
			editRank.params.erase(deleteParamName);
		}

		ImGui::TreePop();
	}

#pragma endregion

	ImGui::End();
}

#endif