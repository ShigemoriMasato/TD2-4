//#pragma once
//#include <SHEngine.h>
//#include <assets/Model/ModelManager.h>
//#include <Render/RenderObject.h>
//
//
//enum class GridState
//{
//	// まだ解放されていない
//	Locked,
//	// 解放されているが、商品が置かれていない
//	Empty,
//	// 商品が置かれている
//	Contained,
//};
//
//
///// <summary>
///// バックパックの1マス
///// </summary>
//class BackPackGrid
//{
//public:
//	BackPackGrid();
//	~BackPackGrid();
//	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager_, GridState state);
//	void Update();
//	void Draw(SHEngine::Command::Object* cmdObject);
//
//private:
//	GridState state_ = GridState::Locked;
//
//};
//
///// <summary>
///// BackPackGridの集合体
///// </summary>
//class BackPack
//{
//public:
//	BackPack();
//	~BackPack();
//	void Initialize(SHEngine::ModelManager* modelManager);
//	void Update();
//	void Draw(SHEngine::Command::Object* cmdObject);
//
//private:
//
//	// WVP行列
//	Matrix4x4 wvp_;
//
//	std::unique_ptr<SHEngine::RenderObject> render_ = nullptr;
//
//};
//
