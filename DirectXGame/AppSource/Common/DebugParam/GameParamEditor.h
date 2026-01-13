#pragma once
#include<iostream>
#include<variant>
#include<map>
#include<string>

#include"Utility/MyMath.h"
#include <Tool/Binary/BinaryManager.h>

class GameParamEditor final {
public:

	// 項目
	struct Item {
		std::shared_ptr<ValueBase> value; // 値
		int priority = INT_MAX; // 優先順位
	};

	// グループ
	struct Group {
		std::map<std::string, Item> items;
		std::string sceneName; // シーンの名前
	};

public:

	// シングルトン
	static GameParamEditor* GetInstance();

	/// <summary>
	/// グループ作成
	/// </summary>
	/// <param name="groupName"></param>
	void CreateGroup(const std::string& groupName,const std::string& sceneName = "none");

	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルから読み込む
	/// </summary>
	/// <param name="groupName"></param>
	void LoadFile(const std::string& groupName);

	/// <summary>
	/// jsonファイルに保存する
	/// </summary>
	/// <param name="groupName"></param>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// グループを選択
	/// </summary>
	/// <param name="groupName"></param>
	void SelectGroup(const std::string& groupName);

	/// <summary>
	/// 選択中のグループを取得
	/// </summary>
	/// <returns></returns>
	const std::string& GetSelectGroup() const { return selectedGroupName_; }

	/// <summary>
	/// シーンを選択
	/// </summary>
	/// <param name="sceneName"></param>
	void SetActiveScene(const std::string& sceneName);

	/// <summary>
	/// 選択中のシーンを取得
	/// </summary>
	/// <returns></returns>
	const std::string& GetActiveScene() const { return activeSceneName_; }

	/// <summary>
	/// 全グループのデータを取得
	/// </summary>
	/// <returns></returns>
	std::map<std::string, Group>& GetAllGroups() { return datas_; }

	/// <summary>
	/// 値を登録する
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="groupName"></param>
	/// <param name="key"></param>
	/// <param name="value"></param>
	template<typename T>
	void AddItem(const std::string& groupName, const std::string& key, const T& value, int priority = INT_MAX) {
		// グループの参照を取得
		Group& group = datas_[groupName];

		// すでに登録されていれば何もしない
		if (group.items.find(key) != group.items.end()) {
			group.items[key].priority = priority; // 優先順位は別なので取得する
			return;
		}

		// アクティブなシーンを登録
		if (group.sceneName.empty()) {
			group.sceneName = activeSceneName_;
		}

		// 新しい項目のデータを設定
		Item newItem{};
		newItem.value = std::make_shared<Value<T>>(value, key);
		newItem.priority = priority;
		// 設定した項目をstd::mapに追加
		group.items[key] = newItem;
	}

	/// <summary>
	/// 登録した値を取得する
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="groupName"></param>
	/// <param name="key"></param>
	/// <returns></returns>
	template<typename T>
	T GetValue(const std::string& groupName, const std::string& key) const {
		// 指定グループが存在するかチェック
		assert(datas_.find(groupName) != datas_.end());

		// 指定キーが存在するかチェック
		const Group& group = datas_.at(groupName);
		assert(group.items.find(key) != group.items.end());

		auto itItem = group.items.find(key);

		// 型が一致しない場合はデフォルト値を返す
		if(uint8_t(TypeIDResolver<T>::id) != itItem->second.value->GetTypeID()) {
			//todo Logを出す
			assert(false && "GameParamEditor::GetValue() failed: Type mismatch");
			return T();
		}

		// グループの参照を取得
		return binaryManager_.Reverse<T>(itItem->second.value.get());
	}

	/// <summary>
	/// 登録した値を削除する
	/// </summary>
	/// <param name="groupName"></param>
	/// <param name="key"></param>
	void RemoveItem(const std::string& groupName, const std::string& key);

private:
	GameParamEditor() = default;
	~GameParamEditor() = default;
	GameParamEditor(const GameParamEditor&) = delete;
	GameParamEditor& operator=(const GameParamEditor&) = delete;

	// 全データ
	std::map<std::string, Group> datas_;

	// 選択中のグループ名
	std::string selectedGroupName_;

	// 現在アクティブなシーン名
	std::string activeSceneName_ = "None";

	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Assets/Json/GameData/";

	BinaryManager binaryManager_;

private:

	/// <summary>
	/// 外部ファイルから値を取得する
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="groupName"></param>
	/// <param name="key"></param>
	/// <param name="value"></param>
	template<typename T>
	void SetValue(const std::string& groupName, const std::string& key, T value) {
		// グループの参照を取得
		Group& group = datas_[groupName];
		Item& item = group.items[key];

		if (static_cast<uint8_t>(TypeIDResolver<T>::id) != item.value->GetTypeID()) {
			//todo Logを出す
			return;
		}

		// 新しい項目のデータを設定
		static_cast<Value<T>*>(item.value.get())->set(value);
	}

	/// <summary>
	/// 読み込んだ値を登録する
	/// </summary>
	/// <param name="groupName"></param>
	/// <param name="key"></param>
	/// <param name="value"></param>
	/// <param name="priority"></param>
	void AddItem(const std::string& groupName, ValueBase* value, int priority = INT_MAX);
};