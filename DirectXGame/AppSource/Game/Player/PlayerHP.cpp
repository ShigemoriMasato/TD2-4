#include "PlayerHP.h"
#include "Player.h"
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

using namespace SHEngine;

void PlayerHP::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Input* input) {
	// HPバーの生成
	hpBarFill_.render = std::make_unique<RenderObject>();  // 前面
	hpBarAfter_.render = std::make_unique<RenderObject>(); // 減った分
	hpBarBG_.render = std::make_unique<RenderObject>();    // 背景

	// HPバーを初期化
	InitializeRenderHPBar(modelManager, drawDataManager, hpBarFill_.render);  // 前面
	InitializeRenderHPBar(modelManager, drawDataManager, hpBarAfter_.render); // 減った分
	InitializeRenderHPBar(modelManager, drawDataManager, hpBarBG_.render);    // 背景

	// 単位行列の代入
	hpBarFill_.wvp = Matrix4x4::Identity();
	hpBarAfter_.wvp = Matrix4x4::Identity();
	hpBarBG_.wvp = Matrix4x4::Identity();

	// 前面
	hpBarFill_.transform.scale = {kHPBarWidth, 1.0f, 1.0f};
	hpBarFill_.transform.rotate = {0.3f, 0.0f, 0.0f};
	hpBarFill_.transform.position = {-10.0f, 8.0f, 0.0f};

	// 減った分
	hpBarAfter_.transform.scale = {kHPBarWidth, 1.0f, 1.0f};
	hpBarAfter_.transform.rotate = {0.3f, 0.0f, 0.0f};
	hpBarAfter_.transform.position = {-10.0f, 8.0f, 0.01f};

	// 背景
	hpBarBG_.transform.scale = {kHPBarWidth, 1.0f, 1.0f};
	hpBarBG_.transform.rotate = {0.3f, 0.0f, 0.0f};
	hpBarBG_.transform.position = {-10.0f, 8.0f, -0.01f};

	// FPSObserver
	fpsObserver_ = std::make_unique<FPSObserver>();

	// 入力
	input_ = input;
}

void PlayerHP::Update(Matrix4x4 vpMatrix) {
	// 無敵状態のカウントダウン
	InvincibleTimerUpdate();

#ifdef _DEBUG
	// ダメージ
	if (input_->GetKeyState(DIK_1) && !input_->GetPreKeyState(DIK_1)) {
		Damage(1);
	}

	// 回復
	if (input_->GetKeyState(DIK_2) && !input_->GetPreKeyState(DIK_2)) {
		Heal(1);
	}
#endif

	// 前面の行列計算
	hpBarFill_.wvp = Matrix::MakeAffineMatrix(hpBarFill_.transform.scale, hpBarFill_.transform.rotate, hpBarFill_.transform.position);
	hpBarFill_.wvp *= vpMatrix;
	hpBarFill_.render->CopyBufferData(0, &hpBarFill_.wvp, sizeof(Matrix4x4));

	// 減った分の行列計算
	hpBarAfter_.wvp = Matrix::MakeAffineMatrix(hpBarAfter_.transform.scale, hpBarAfter_.transform.rotate, hpBarAfter_.transform.position);
	hpBarAfter_.wvp *= vpMatrix;
	hpBarAfter_.render->CopyBufferData(0, &hpBarAfter_.wvp, sizeof(Matrix4x4));

	// 背景の行列計算
	hpBarBG_.wvp = Matrix::MakeAffineMatrix(hpBarBG_.transform.scale, hpBarBG_.transform.rotate, hpBarBG_.transform.position);
	hpBarBG_.wvp *= vpMatrix;
	hpBarBG_.render->CopyBufferData(0, &hpBarBG_.wvp, sizeof(Matrix4x4));
}

void PlayerHP::Draw(CmdObj* cmdObj) {
	// 背景から前面の順に描画
	hpBarBG_.render->Draw(cmdObj);
	hpBarAfter_.render->Draw(cmdObj);
	hpBarFill_.render->Draw(cmdObj);

	ImGui::Begin("PlayerHP");

	// --- 基本パラメータ ---
	if (ImGui::CollapsingHeader("Basic Parameters")) {
		ImGui::DragFloat3("Fill - scale", &hpBarFill_.transform.scale.x, 0.01f);
		ImGui::DragFloat3("Fill - rotate", &hpBarFill_.transform.rotate.x, 0.01f);
		ImGui::DragFloat3("Fill - translate", &hpBarFill_.transform.position.x, 0.01f);
		ImGui::Separator();
		ImGui::DragFloat3("After - scale", &hpBarAfter_.transform.scale.x, 0.01f);
		ImGui::DragFloat3("After - rotate", &hpBarAfter_.transform.rotate.x, 0.01f);
		ImGui::DragFloat3("After - translate", &hpBarAfter_.transform.position.x, 0.01f);
		ImGui::Separator();
		ImGui::DragFloat3("BG - scale", &hpBarBG_.transform.scale.x, 0.01f);
		ImGui::DragFloat3("BG - rotate", &hpBarBG_.transform.rotate.x, 0.01f);
		ImGui::DragFloat3("BG - translate", &hpBarBG_.transform.position.x, 0.01f);
	}

	ImGui::End();
}

void PlayerHP::Damage(float amount) {
	// 不正値をガード
	if (amount <= 0.0f || isInvincible_)
		return;

	// HPの減算
	currentHP_ = std::max(currentHP_ - amount, 0.0f);

	// 無敵状態にする
	isInvincible_ = true;

	// HPバーの変更
	HPBarScaleChange();
}

void PlayerHP::Heal(float amount) {
	// 不正値をガード
	if (amount <= 0.0f)
		return;

	// HPの加算
	currentHP_ = std::min(currentHP_ + amount, maxHP);

	// HPバーの変更
	HPBarScaleChange();
}

void PlayerHP::HPBarScaleChange() {
	// 前面のスケール変更
	float hpRatio = currentHP_ / maxHP;
	hpBarFill_.transform.scale.x = hpRatio * kHPBarWidth;

	// 減った分のスケール変更
	hpBarAfter_.transform.scale.x = kHPBarWidth;
}

void PlayerHP::InvincibleTimerUpdate() {
	if (isInvincible_) {
		invincibleTimer_ -= fpsObserver_->GetDeltatime(); // 無敵状態のカウントダウン

		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;                  // 無敵状態フラグを下す
			invincibleTimer_ = invincibleDuration_; // タイマーのリセット
		}
	}
}

void PlayerHP::InitializeRenderHPBar(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, std::unique_ptr<SHEngine::RenderObject>& render) {
	render->Initialize();
	render->psoConfig_.vs = "Simple.VS.hlsl"; // WPだけ送るVS
	render->psoConfig_.ps = "White.PS.hlsl";  // とりあえず白で返すPS

	render->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);

	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Plane");

	// 描画するデータの読み込み
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render->SetDrawData(drawData);
}
