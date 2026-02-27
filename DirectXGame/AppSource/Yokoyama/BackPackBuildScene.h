#pragma once
#include <Scene/IScene.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include <Camera/DebugCamera.h>

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
	// カメラ
	std::unique_ptr<DebugCamera> camera_ = nullptr;

	// Transform
	Transform transform_{};
	// WVP行列
	Matrix4x4 wvp_;

	std::unique_ptr<ItemManager> itemManager_;

	std::unique_ptr<SHEngine::RenderObject> render_ = nullptr;
};

