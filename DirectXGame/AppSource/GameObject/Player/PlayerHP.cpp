#include "PlayerHP.h"
#include "Player.h"
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

using namespace SHEngine;
using namespace Player;

void HP::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	// HPバーの生成
	hpBarFill_.render = std::make_unique<RenderObject>("HPBarFill");  // 前面
	hpBarAfter_.render = std::make_unique<RenderObject>("HPBarAfter"); // 減った分
	hpBarBG_.render = std::make_unique<RenderObject>("HPBarBackGround");    // 背景

	// HPバーを初期化
	InitializeRenderHPBar(modelManager, drawDataManager, hpBarFill_.render);  // 前面
	InitializeRenderHPBar(modelManager, drawDataManager, hpBarAfter_.render); // 減った分
	InitializeRenderHPBar(modelManager, drawDataManager, hpBarBG_.render);    // 背景

	// 単位行列の代入
	hpBarFill_.wvp = Matrix4x4::Identity();
	hpBarAfter_.wvp = Matrix4x4::Identity();
	hpBarBG_.wvp = Matrix4x4::Identity();

	// 前面
	hpBarFill_.transform.scale = {hpBarSize_.x, hpBarSize_.y, 1.0f};
	hpBarFill_.transform.rotate = {0.0f, 0.0f, 0.0f};
	hpBarFill_.transform.position = {hpBarPos_.x, hpBarPos_.y, 0.0f};

	// 減った分
	hpBarAfter_.transform.scale = {hpBarSize_.x, hpBarSize_.y, 1.0f};
	hpBarAfter_.transform.rotate = {0.0f, 0.0f, 0.0f};
	hpBarAfter_.transform.position = {hpBarPos_.x, hpBarPos_.y, 0.0f};

	// 背景
	hpBarBG_.transform.scale = {hpBarSize_.x, hpBarSize_.y, 1.0f};
	hpBarBG_.transform.rotate = {0.0f, 0.0f, 0.0f};
	hpBarBG_.transform.position = {hpBarPos_.x, hpBarPos_.y, 0.0f};

	// モデルマネージャー
	modelManager_ = modelManager;

	// テキストの初期化
	int planeModelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Plane");
	auto planeModelData = modelManager->GetNodeModelData(planeModelHandle);
	DrawData data = drawDataManager->GetDrawData(planeModelData.drawDataIndex);

	hpText_ = std::make_unique<SHEngine::Text>();
	hpText_->Initialize(data, "YDWbananaslipplus.otf", 64);
	hpText_->SetText(L"0");
	hpText_->SetSize(hpTextSize_);
	//hpTextTransform_.position = {hpBarPos_.x, hpBarPos_.y, 0.0f}; // HPバーの初期位置辺りに配置
}

void HP::Update(Matrix4x4 vpMatrix, float deltaTime, float currentHP, float maxHP) {
	// HPに変化があったらアニメーションを開始する
	if(previousHP_!=currentHP){
		HPBarScaleChange(currentHP, maxHP);
		previousHP_ = currentHP;
	}

#ifdef USE_IMGUI
	// ImGuiでのサイズ・位置変更を即座に反映させるための再計算
	hpBarBG_.transform.scale = {hpBarSize_.x, hpBarSize_.y, 1.0f};
	hpBarBG_.transform.position = {hpBarPos_.x, hpBarPos_.y, 0.0f};
	
	hpBarFill_.transform.scale.y = hpBarSize_.y;
	hpBarAfter_.transform.scale.y = hpBarSize_.y;

	float offsetX = (hpBarSize_.x - hpBarFill_.transform.scale.x) / 2.0f;
	hpBarFill_.transform.position = {hpBarPos_.x - offsetX, hpBarPos_.y, 0.0f};
	
	offsetX = (hpBarSize_.x - hpBarAfter_.transform.scale.x) / 2.0f;
	hpBarAfter_.transform.position = {hpBarPos_.x - offsetX, hpBarPos_.y, 0.0f};
#endif

	// スケールアニメーションの更新
	AnimationHPBarAfter(deltaTime);

	// 前面の行列計算
	hpBarFill_.wvp = Matrix::MakeAffineMatrix(hpBarFill_.transform.scale, hpBarFill_.transform.rotate, hpBarFill_.transform.position);
	hpBarFill_.wvp *= vpMatrix;
	hpBarFill_.render->CopyBufferData(0, &hpBarFill_.wvp, sizeof(Matrix4x4));

	Vector4 color = {0.0f, 1.0f, 0.0f, 1.0f};

	hpBarFill_.render->CopyBufferData(1, &hpColor_, sizeof(Vector4));

	// 減った分の行列計算
	hpBarAfter_.wvp = Matrix::MakeAffineMatrix(hpBarAfter_.transform.scale, hpBarAfter_.transform.rotate, hpBarAfter_.transform.position);
	hpBarAfter_.wvp *= vpMatrix;
	hpBarAfter_.render->CopyBufferData(0, &hpBarAfter_.wvp, sizeof(Matrix4x4));

	color = {1.0f, 1.0f, 0.0f, 1.0f};

	hpBarAfter_.render->CopyBufferData(1, &color, sizeof(Vector4));

	// 背景の行列計算
	hpBarBG_.wvp = Matrix::MakeAffineMatrix(hpBarBG_.transform.scale, hpBarBG_.transform.rotate, hpBarBG_.transform.position);
	hpBarBG_.wvp *= vpMatrix;
	hpBarBG_.render->CopyBufferData(0, &hpBarBG_.wvp, sizeof(Matrix4x4));

	color = {0.0f, 0.0f, 0.0f, 1.0f};

	hpBarBG_.render->CopyBufferData(1, &color, sizeof(Vector4));

	// HPテキスト更新
	std::wstring hpString = std::to_wstring((int)currentHP);
	hpTextTransform_.scale = { hpTextSize_, hpTextSize_, 1.0f };
	hpText_->SetText(hpString);
	hpText_->SetColor(hpTextColor_);
	hpText_->SetTransform(hpTextTransform_);
	hpText_->Update(vpMatrix);
}

void HP::Draw(CmdObj* cmdObj) {
	// 背景から前面の順に描画
	hpBarBG_.render->Draw(cmdObj);
	hpBarAfter_.render->Draw(cmdObj);
	hpBarFill_.render->Draw(cmdObj);

	// HPテキスト描画
	hpText_->Draw(cmdObj);
}

void HP::HPBarScaleChange(float currentHP, float maxHP) {
	// 現在のHPの比率
	float hpRatio = currentHP / maxHP;

	// 新しいスケール
	float newScale = hpRatio * hpBarSize_.x;

	// HPバー　減った分のアニメーション用変数の初期化
	scaleAnimationHPBarAfter_.anim.Start(hpBarFill_.transform.scale.x, newScale, 1.0f, EaseType::EaseOutCubic);

	// 前面のスケール変更
	hpBarFill_.transform.scale.x = newScale;

	// X位置を調整してHPバーが左詰めに見えるようにする
	float offsetX = (hpBarSize_.x - hpBarFill_.transform.scale.x) / 2.0f;
	hpBarFill_.transform.position.x = hpBarPos_.x - offsetX;
	hpBarAfter_.transform.position.x = hpBarPos_.x - offsetX;
}

void HP::InitializeRenderHPBar(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, std::unique_ptr<SHEngine::RenderObject>& render) {
	render->Initialize();
	render->psoConfig_.vs = "Simple.VS.hlsl";
	render->psoConfig_.ps = "Color.PS.hlsl";

	render->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	render->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Plane");

	// 描画するデータの読み込み
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render->SetDrawData(drawData);
}

void HP::AnimationHPBarAfter(float deltaTime) {
	// スケールアニメーションの更新
	scaleAnimationHPBarAfter_.anim.Update(deltaTime, scaleAnimationHPBarAfter_.temp);

	// 変更した値の変更
	hpBarAfter_.transform.scale.x = scaleAnimationHPBarAfter_.temp;

	// 座標の変更
	float offsetX = (hpBarSize_.x - hpBarAfter_.transform.scale.x) / 2.0f;
	hpBarAfter_.transform.position.x = hpBarPos_.x - offsetX;
}