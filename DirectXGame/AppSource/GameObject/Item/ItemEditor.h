#pragma once

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#include <vector>
#include <string>
#include <memory>

class ItemManager;

class PairHash {
public:
	size_t operator()(const std::pair<int, int>& p) const noexcept {
		const uint64_t x = static_cast<uint32_t>(p.first);
		const uint64_t y = static_cast<uint32_t>(p.second);
		return x | (y << 32);
	}
};

class ItemEditor
{
public:

	void Draw(ItemManager& itemManager);

private:

	struct Node {
		Node* parent = nullptr;
		std::vector<std::unique_ptr<Node>> children;
		std::string name;
		int itemID = -1; // アイテムID（-1ならフォルダ）
	};

	void CreateItemFromModel(ItemManager& itemManager);

	void DrawNode(Node* node);

	// Itemのモデルがあるフォルダのパス
	std::string basePath_ = "Assets/Model/Item/";

	// ファイルパスをツリー構造に変換したもの
	std::unique_ptr<Node> rootNode_ = std::make_unique<Node>();
	// 現在選択中のノード
	Node* selectedNode = nullptr;

	// stringを編集するときのバッファ
	char bufEdit_[256] = "";

	//　mapData編集用
	int gridW_ = 8;
	int gridH_ = 8;
	float cellSize_ = 16.0f;
	int paintMode_ = 0; // 0:none 1:add(L) 2:erase(R)
};

#else

class ItemManager;
class ItemEditor
{
public:
	void Draw(ItemManager&) {}
};

#endif