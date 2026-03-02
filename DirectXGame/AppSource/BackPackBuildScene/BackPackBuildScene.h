#pragma once
#include <Scene/IScene.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>

class BackPack;
class ItemManager;

class BackPackBuildScene : public IScene
{
public:
	BackPackBuildScene();
	~BackPackBuildScene();
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:


	// Transform
	Transform transform_{};
	// WVP行列
	Matrix4x4 wvp_;

	// 色
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };



	// バックパック
	std::unique_ptr<BackPack> backPack_;

	// マップgrid
	std::unique_ptr<Grid> grid_;
	
	// カメラ
	std::unique_ptr<DebugCamera> camera_ = nullptr;

	// アイテムマネージャー(多分いずれどこからかポインタを持ってくる)
	std::unique_ptr<ItemManager> itemManager_;
};

