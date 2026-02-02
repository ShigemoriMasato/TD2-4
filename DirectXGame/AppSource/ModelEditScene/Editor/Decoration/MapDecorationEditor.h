#pragma once
#include <Assets/Model/ModelManager.h>
#include <Game/NewMap/NewMap.h>

class MapDecorationEditor {
public:

	void Initialize(ModelManager* modelManager);
	void Update();
	/// @brief カーソルの上にクリックしたら出てくるモデルのプレビューを描画
	void PreviewDraw();
	void DrawImGui();

	void Save();
	void SetCurrentMap(int mapID);

	void SetCursorPosition(const Vector2& pos) { cursorPos_ = pos; }

	bool isSomeSelected() const { return someSelected_; }
	void NonEdit() { editing_ = false; }

	const std::map<int, std::vector<Transform>>& GetDecorations() { return decorations_[currentMapID_]; }


	void GenerateWW(std::vector<std::vector<TileType>> mapData);
	int GetWallIndex() const;

private:

	void DecoDataLoad();
	void ModelListLoad();

	// Index:	Engine's ModelID
	// ID:		Unique Model ID for Decoration

	/// @brief モデルIndex->座標のマップ
	std::vector<std::map<int, std::vector<Transform>>> decorations_{};
	/// @brief モデルID->モデル名、表示カラーのマップ
	std::map<int, std::pair<std::string, Vector3>> modelList_{};
	/// @brief モデルIDのマップ（モデルIndex->モデルID）
	std::map<int, int> modelIDMap_{};

	/// @brief 次にクリックしたとき新しく出てくるモデルのID
	int currentModelIndex_ = 0;
	/// @brief 現在編集中のforOutput用のIndex
	int editingTransformIndex_ = -1;

	int currentMapID_ = 0;

	Vector2 cursorPos_{};
	Vector2 cursorOffset_{};
	
	ModelManager* modelManager_{ nullptr };

	bool someSelected_ = false;
	bool editing_ = false;
	bool isHold_ = false;
	bool isHovered_ = false;

	const float radius_ = 0.5f;

	BinaryManager binaryManager_;
	const std::string saveFileNameDecorationData_ = "MapDecorationData";
	const std::string saveFileNameModelList_ = "MapDecorationModelList";
};
