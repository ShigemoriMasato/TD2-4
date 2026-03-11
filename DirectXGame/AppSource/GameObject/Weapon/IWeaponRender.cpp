#include "IWeaponRender.h"
#include <numbers>

using namespace SHEngine;

void IWeaponRender::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, const std::string& filepath) {
	render_ = std::make_unique<RenderObject>();

	int modelHandle = modelManager->LoadModel("Assets/Model/Item/Weapon/" + filepath + "/");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;

	// モデルを初期化
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();

	// シェーダー設定
	render_->psoConfig_.vs = "Game/Field.VS.hlsl";
	render_->psoConfig_.ps = "Game/Field.PS.hlsl";
	render_->SetUseTexture(true);

	// CBVの生成
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

	// 描画データを設定
	render_->SetDrawData(drawData);

	// ワールド座標を設定
	transform_.position = {0.0f, 0.0f, 0.0f};
	transform_.rotate = {0.0f, 0.0f, -std::numbers::pi_v<float> / 2};
	transform_.scale = {0.5f, 0.5f, 0.5f};

	// 単位行列の代入
	wvp_ = Matrix4x4::Identity();
}

void IWeaponRender::Update(Matrix4x4 vpMatrix, Vector3 playerPos) {
	transform_.position = playerPos;
	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ *= vpMatrix;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));
}

void IWeaponRender::Draw(CmdObj* cmdObj) { render_->Draw(cmdObj); }

Matrix4x4 IWeaponRender::LookAt(const Vector3& direction, const Vector3& up) {
	Vector3 forward = MyMath::Normalize(direction);
	Vector3 right = MyMath::Normalize(MyMath::cross(up, forward));
	Vector3 trueUp = MyMath::cross(forward, right);

	Matrix4x4 m;

	m.m[0][0] = right.x;
	m.m[1][0] = right.y;
	m.m[2][0] = right.z;

	m.m[0][1] = trueUp.x;
	m.m[1][1] = trueUp.y;
	m.m[2][1] = trueUp.z;

	m.m[0][2] = forward.x;
	m.m[1][2] = forward.y;
	m.m[2][2] = forward.z;

	m.m[3][3] = 1;

	return m;
}