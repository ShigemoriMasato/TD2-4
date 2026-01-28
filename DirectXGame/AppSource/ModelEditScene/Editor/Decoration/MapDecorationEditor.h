#pragma once
#include <Assets/Model/ModelManager.h>

class MapDecorationEditor {
public:

	void Initialize(ModelManager* modelManager);
	void Update();
	/// @brief カーソルの上にクリックしたら出てくるモデルのプレビューを描画
	void PreviewDraw();
	void DrawImGui();

	void Save();

	void SetCursorPosition(const Vector2& pos) { cursorPos_ = pos; }

	bool isSomeSelected() const { return someSelected_; }
	void NonEdit() { editing_ = false; }

	const std::vector<std::pair<Transform, int>>& GetDecorations() { return decorations_; }

private:

	void DevoDataLoad();
	void ModelListLoad();

	// Index:	Engine's ModelID
	// ID:		Unique Model ID for Decoration

	/// @brief 描画用の位置とモデルIndexのペアリスト
	std::vector<std::pair<Transform, int>> forOutput_{};
	/// @brief 位置とモデルIDのペアリスト
	std::vector<std::pair<Transform, int>> decorations_{};
	/// @brief モデルID->モデル名、表示カラーのマップ
	std::map<int, std::pair<std::string, Vector3>> modelList_{};

	/// @brief 次にクリックしたとき新しく出てくるモデルのID
	int currentModelID_ = 0;
	/// @brief 編集中のデコレーションのインデックス(Transformの編集を行っているdecorations_のIndex)
	int editingModelIndex_ = -1;

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
