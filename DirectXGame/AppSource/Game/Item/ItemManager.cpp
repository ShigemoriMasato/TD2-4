#include "ItemManager.h"
#include "Item.h"
#include <Utility/ConvertString.h>
#include <imgui/imgui.h>

ItemManager::~ItemManager() {
	SaveBaseParam();
	SaveItem();
	SaveModel();
}

void ItemManager::Initialize(SHEngine::ModelManager* modelManager)
{
	modelManager_ = modelManager;

	LoadBaseParam();
	LoadItem();
	LoadModel();

	if (items_.empty())
	{
		Item defaultItem{};
		defaultItem.name = L"Default Item";
		defaultItem.category = Category::Item;
		defaultItem.effect = 0u;
		defaultItem.params = baseParam_;
		items_.push_back(std::move(defaultItem));
	}
}

const Item& ItemManager::GetItem(std::wstring itemName) const {
	for(const auto& item : items_) {
		if (item.name == itemName) {
			return item;
		}
	}
	throw std::runtime_error("Item not found: " + ConvertString(itemName));
}

void ItemManager::DrawImGui()
{
#ifdef USE_IMGUI

	ImGui::Begin("Item Manager");

#pragma region Item追加

	// 追加に成功したかフラグ
	static bool successAddItem = false;

	// 新規追加するアイテムの名前
	static char newItemName[64] = "NewItem";
	ImGui::InputText("NewItem Name", newItemName, sizeof(newItemName));
	// 新規追加するアイテムのカテゴリ
	static Category newItemCategory = Category::Item;
	ImGui::Combo("Category", reinterpret_cast<int*>(&newItemCategory), "Weapon\0Armor\0Item\0");

	// アイテム追加実行ボタン
	if (ImGui::Button("Add Item"))
	{
		// フラグリセット
		successAddItem = false;

		// 日本語対応
		const std::wstring newNameW = ConvertString(std::string(newItemName));

		// 重複チェック
		const bool exists = std::any_of(items_.begin(), items_.end(),
			[&](const Item& it) { return it.name == newNameW; });
		// 重複してたら失敗フラグをたてる
		if (exists)
		{
			successAddItem = true;
		}
		// 重複してなければ追加
		else
		{
			Item item{};
			item.name = newNameW;
			item.category = newItemCategory;
			item.effect = 0u;
			item.params.clear();
			items_.push_back(std::move(item));
		}
	}
	// 失敗フラグがたってたら重複エラーを表示
	if (successAddItem)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error : exists Item!");
	}

#pragma endregion

	ImGui::Separator();

#pragma region Item一覧と削除

	static int currentItemIndex = 0;
	currentItemIndex = std::clamp(currentItemIndex, 0, static_cast<int>(items_.size()) - 1);

	int deleteIndex = -1;

	if (ImGui::BeginTable("ItemTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY, ImVec2(0, 160.0f)))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("##Del", ImGuiTableColumnFlags_WidthFixed, 70.0f);

		for (int i = 0; i < static_cast<int>(items_.size()); ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			// 名前表示
			ImGui::TableSetColumnIndex(0);
			const bool selected = (currentItemIndex == i);
			const std::string label = ConvertString(items_[i].name);
			if (ImGui::Selectable(label.c_str(), selected))
			{
				currentItemIndex = i;
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

	if (deleteIndex >= 0)
	{
		items_.erase(items_.begin() + deleteIndex);

		if (items_.empty())
		{
			Item defaultItem{};
			defaultItem.name = L"Default Item";
			defaultItem.category = Category::Item;
			defaultItem.effect = 0u;
			defaultItem.params = baseParam_;
			items_.push_back(std::move(defaultItem));
		}

		if (currentItemIndex >= static_cast<int>(items_.size()))
		{
			currentItemIndex = static_cast<int>(items_.size()) - 1;
		}
	}

	Item& currentItem = items_[currentItemIndex];


#pragma endregion

	ImGui::Separator();

#pragma region アイテムの基本情報編集

	//ImGui::InputText("Name", &currentItem.name[0], 64);
	ImGui::Combo("Category##2", reinterpret_cast<int*>(&currentItem.category), "Weapon\0Armor\0Item\0");

#pragma endregion


#pragma region バフパラメータ編集

	static int currentParamType = 0;
	if (ImGui::Button("Add"))
	{
		const std::string paramName = GetParamTypeNames()[currentParamType];
		currentItem.params[paramName] = 0.0f;
	}
	ImGui::SameLine();
	ImGui::Combo("ParamType", &currentParamType, GetParamTypeNames(), GetParamTypeCount());

	ImGui::Text("Params:");
	for (auto& [name, value] : currentItem.params)
	{
		ImGui::DragFloat(name.c_str(), &value, 0.1f);
	}

#pragma endregion

	ImGui::End();
#endif
}

//=================================================================
// セーブとロード
//=================================================================

void ItemManager::SaveModel() {
	int size = static_cast<int>(modelIDtoName_.size());
	binaryManager_.RegisterOutput(&size);
	for (const auto& [id, name] : modelIDtoName_) {
		binaryManager_.RegisterOutput(&id);
		binaryManager_.RegisterOutput(&name);
	}
	binaryManager_.Write(modelFile_);
}

void ItemManager::LoadModel() {
	auto data = binaryManager_.Read(modelFile_);
	if (data.empty()) {
		return;
	}
	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i) {
		int id = binaryManager_.Reverse<int>(data);
		std::string name = binaryManager_.Reverse<std::string>(data);
		modelIDtoName_[id] = name;
	}
}

void ItemManager::SaveItem() {
	int size = static_cast<int>(items_.size());
	binaryManager_.RegisterOutput(&size);
	for (const auto& item : items_) {
		std::string tmp = ConvertString(item.name);
		binaryManager_.RegisterOutput(&tmp);
		int category = static_cast<int>(item.category);
		binaryManager_.RegisterOutput(&category);
		binaryManager_.RegisterOutput(&item.effect);
		int mapDataSize = static_cast<int>(item.mapData.size());
		binaryManager_.RegisterOutput(&mapDataSize);
		for (const auto& [x, y] : item.mapData) {
			binaryManager_.RegisterOutput(&x);
			binaryManager_.RegisterOutput(&y);
		}
		int buffsSize = static_cast<int>(item.params.size());
		binaryManager_.RegisterOutput(&buffsSize);
		for (const auto& buff : item.params) {
			binaryManager_.RegisterOutput(&buff.first);
			binaryManager_.RegisterOutput(&buff.second);
		}
		binaryManager_.RegisterOutput(&item.weaponID);
	}
	binaryManager_.Write(itemFile_);
}

void ItemManager::LoadItem() {
	auto data = binaryManager_.Read(itemFile_);
	if (data.empty()) {
		return;
	}

	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i) {
		Item item;
		item.name = ConvertString(binaryManager_.Reverse<std::string>(data));
		int category = binaryManager_.Reverse<int>(data);
		item.category = static_cast<Category>(category);
		item.effect = binaryManager_.Reverse<uint32_t>(data);
		int mapDataSize = binaryManager_.Reverse<int>(data);
		for (int j = 0; j < mapDataSize; ++j) {
			int x = binaryManager_.Reverse<int>(data);
			int y = binaryManager_.Reverse<int>(data);
			item.mapData.emplace_back(x, y);
		}
		int buffsSize = binaryManager_.Reverse<int>(data);
		item.params = baseParam_; // 基礎値をコピー
		for (int j = 0; j < buffsSize; ++j) {
			std::string name = binaryManager_.Reverse<std::string>(data);
			float value = binaryManager_.Reverse<float>(data);
			item.params[name] = value;
		}
		item.weaponID = binaryManager_.Reverse<int>(data);
		items_.push_back(item);
	}
}

void ItemManager::SaveBaseParam() {
	int size = static_cast<int>(baseParam_.size());
	binaryManager_.RegisterOutput(&size);
	for (const auto& param : baseParam_) {
		binaryManager_.RegisterOutput(&param.first);
		binaryManager_.RegisterOutput(&param.second);
	}
	binaryManager_.Write(baseParamFile_);
}

void ItemManager::LoadBaseParam() {
	auto data = binaryManager_.Read(baseParamFile_);
	if (data.empty()) {
		return;
	}
	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i) {
		std::string name = binaryManager_.Reverse<std::string>(data);
		float value = binaryManager_.Reverse<float>(data);
		baseParam_[name] = value;
	}
}


void ItemManager::UpdateItemNameCache()
{
	itemNameUtf8_.clear();
	itemNameCStr_.clear();

	itemNameUtf8_.reserve(items_.size());
	itemNameCStr_.reserve(items_.size());

	for (const auto& item : items_)
	{
		itemNameUtf8_.push_back(ConvertString(item.name));
	}

	for (const auto& s : itemNameUtf8_)
	{
		itemNameCStr_.push_back(s.c_str());
	}
}