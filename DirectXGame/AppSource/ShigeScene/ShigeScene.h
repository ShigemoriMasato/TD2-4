#pragma once
#include <Scene/IScene.h>
#include <Game/Puzzle/PieceRender.h>
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>

class ShigeScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<DebugCamera> camera_;
	std::unique_ptr<Grid> grid_;

	std::vector<Piece> pieces_;

	std::unique_ptr<PieceRender> pieceRender_;

};
