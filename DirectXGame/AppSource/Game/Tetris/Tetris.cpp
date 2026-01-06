#include "Tetris.h"

void Tetris::Initialize(KeyCoating* keys, Camera* camera, const DrawData& drawData) {
	field_ = std::make_unique<Field>(fieldWidth_, fieldHeight_);
	field_->Initialize(camera);

	blockRender_ = std::make_unique<BlockRender>();
	blockRender_->Initialize({
		{ int(Tetrimino::Type::None), {0x00000, 0x00000000} }, // 空白		Air
		{ int(Tetrimino::Type::S), {0x00ff00ff, 0xffffffff} }, // 赤			S
		{ int(Tetrimino::Type::Z), {0xff0000ff, 0xffffffff} }, // 緑			Z
		{ int(Tetrimino::Type::T), {0xff00ffff, 0xffffffff} }, // 紫			T
		{ int(Tetrimino::Type::O), {0xffff00ff, 0xffffffff} }, // 黄			O
		{ int(Tetrimino::Type::I), {0x00ffffff, 0xffffffff} }, // 水色		I
		{ int(Tetrimino::Type::L), {0x0000ffff, 0xffffffff} }, // 青			L
		{ int(Tetrimino::Type::J), {0xff8000ff, 0xffffffff} }, // オレンジ	J
		{ int(Tetrimino::Type::Wall), {0x808080ff, 0x000000ff} }, // 灰色	Wall
		{ int(Tetrimino::Type::Del), {0xffffffff, 0xffffffff} },// 白		DeleteEffect
		{ int(Tetrimino::Type::Hold), {0x000000ff, 0xffffffff} },
		{ int(Tetrimino::Type::Next), {0x000000ff, 0xffffffff} }
		}, fieldWidth_, fieldHeight_, camera, drawData);

	tetrimino_ = std::make_unique<Tetrimino>();
	std::random_device rd;
	mt = std::mt19937(rd());
	tetrimino_->Initialize(mt);

	player_ = std::make_unique<Player>();
	player_->Initialize(field_.get(), tetrimino_.get());

	keys_ = keys;
}

void Tetris::Update(float deltaTime) {
	auto key = keys_->GetKeyStates();
	deletedLine_ = 0;

	if (!blockRender_->GetIsEffecting()) {
		player_->SetDownTime(downInterval_);

		bool success = player_->SpawnMino();
		if (!success) {
			gameOver_ = true;
			return;
		}
		gameOver_ = false;

		player_->Update(deltaTime, key);

		auto mapData = field_->GetField();
		blockRender_->SetStageData(mapData, player_->GetMoveMino());
		std::vector<std::pair<int, int>> holdPos;
		holdPos = tetrimino_->GetOffset(Tetrimino::Type(player_->GetHoldMino()));
		blockRender_->SetHoldMino(holdPos, Tetrimino::Hold);

		//Next 4個分
		const int nextNum = 4;
		std::vector<std::pair<int, int>> nextPos;
		nextPos.resize(4 * nextNum);
		for (int i = 0; i < nextNum; ++i) {
			auto nextType = tetrimino_->GetNextTetrimino(i);
			auto next = tetrimino_->GetOffset(nextType);
			for(int j = 0; j < next.size(); ++j) {
				nextPos[j + (4 * i)] = next[j];
			}
		}
		blockRender_->SetNextMino(nextPos, int(Tetrimino::Type::Next));

		auto fillLines = field_->FillLineIndex();
		if (!fillLines.empty()) {
			field_->DeleteFillLine();
			blockRender_->BeginDeleteEffect(fillLines, field_->GetField());
			deletedLine_ = int(fillLines.size());
		}

	}

	blockRender_->Update(deltaTime);
}

void Tetris::Draw(Window* window) {
	blockRender_->Draw(window);
}

void Tetris::DrawImGui() {
	blockRender_->DrawImGui();
}
