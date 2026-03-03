#pragma once
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include "GameObject/BackPack/GameConstants.h"

class ItemManager;
class BackPackPiece;
class DrawBackPack;
class Shop;
struct CommonData;

class BackPack
{
public:
	BackPack();
	~BackPack();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData, SHEngine::Input* input);
	void Update(const Matrix4x4& viewProj);
	void Draw(SHEngine::Command::Object* cmdObject);
	void DrawImGui();

private:

	// バックパックの描画部分
	std::unique_ptr<DrawBackPack> drawBackPack_;

	// バックパックのデータ部分
	std::vector<std::vector<std::unique_ptr<BackPackPiece>>> pieces_;
};