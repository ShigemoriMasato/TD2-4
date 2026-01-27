#include"GameParamEditor.h"
#include<cassert>
#include<algorithm>
#include <fstream>
#include<Windows.h>
#include <filesystem>
#include <Utility/SearchFile.h>

GameParamEditor* GameParamEditor::GetInstance() {
	static GameParamEditor instance;
	return &instance;
}

void GameParamEditor::CreateGroup(const std::string& groupName, const std::string& sceneName) {
	// 指定名のオブジェクトが無ければ追加する
	datas_[groupName].sceneName = sceneName;
}

void GameParamEditor::SaveFile(const std::string& groupName) {

	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	// 未登録チェック
	assert(itGroup != datas_.end());

	// 各項目について
	for (const auto& [name, item] : itGroup->second.items) {
		// binaryに値を保存する
		binaryManager_.RegisterOutput(item.priority);
		binaryManager_.RegisterOutput(item.value.get());
	}

	// ディレクトリがなければ作成する
	if (!std::filesystem::exists("Assets/Binary/" + kDirectoryPath)) {
		std::filesystem::create_directories("Assets/Binary/" + kDirectoryPath);
	}

	binaryManager_.Write(kDirectoryPath + groupName);
}

void GameParamEditor::LoadFiles() {

	// ディレクトリがなければスキップする
	std::filesystem::path dir("Assets/Binary/" + kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		return;
	}

	auto files = SearchFilePathsAddChild("Assets/Binary/" + kDirectoryPath, ".sg");
	for (auto& entry : files) {
		entry.erase(entry.end() - 3, entry.end()); // 拡張子削除
		// ファイル読み込み
		LoadFile(entry);
	}
}

void GameParamEditor::LoadFile(const std::string& groupName) {
	datas_[groupName].items.clear();

	auto values = binaryManager_.Read(kDirectoryPath + groupName);
	if (values.empty()) {
		return;
	}

	// 各アイテムについて
	for (size_t i = 0; i < values.size(); ) {
		int priority = binaryManager_.Reverse<int>(values[i++].get());
		AddItem(groupName, values[i++].get(), priority);
	}
}

void GameParamEditor::RemoveItem(const std::string& groupName, const std::string& key) {
	// グループを検索
	auto itGroup = datas_.find(groupName);

	// グループが存在しなければ終了
	if (itGroup == datas_.end()) {
		return;
	}

	// グループ内のアイテムマップの参照を取得
	std::map<std::string, Item>& items = itGroup->second.items;

	// アイテムを検索
	auto itItem = items.find(key);

	// アイテムが存在すれば削除
	if (itItem != items.end()) {
		items.erase(itItem);
	}
}

void GameParamEditor::AddItem(const std::string& groupName, ValueBase* value, int priority) {
	Group& group = datas_[groupName];
	std::string& key = value->name;

	// すでに登録されていれば何もしない
	if (group.items.find(key) != group.items.end()) {
		group.items[key].priority = priority; // 優先順位は別なので取得する
		return;
	}

	Item newItem{};
	newItem.priority = priority;
	newItem.value = value->Clone();

	group.items[key] = newItem;
}

void GameParamEditor::SelectGroup(const std::string& groupName) {
	selectedGroupName_ = groupName;
}

void GameParamEditor::SetActiveScene(const std::string& sceneName) {
	activeSceneName_ = sceneName;
}