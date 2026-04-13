#include "TestTrail4.h"
#include <numbers>
#include <cmath>
#include <algorithm>
#include "imgui/imgui.h"

using namespace SHEngine;

namespace
{
	Vector3 NormalizeSafe(const Vector3& v)
	{
		const float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len <= 1e-6f) return { 0.0f, 1.0f, 0.0f };
		return { v.x / len, v.y / len, v.z / len };
	}

	Vector3 Cross(const Vector3& a, const Vector3& b)
	{
		return Vector3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}
}

void TestTrail4::Initialize(
	DrawDataManager* drawDataManager,
	TextureManager* textureManager,
	ModelManager* modelManager,
	int swordModelHandle)
{
	drawDataManager_ = drawDataManager;
	textureManager_ = textureManager;
	modelManager_ = modelManager;

	swordModelData_ = modelManager_->GetNodeModelData(swordModelHandle);
	auto drawData = drawDataManager_->GetDrawData(swordModelData_.drawDataIndex);
	auto& material = swordModelData_.materials[swordModelData_.materialIndex.front()];
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
	transform_.position = { 0.0f, 0.0f, 0.0f };
	transform_.rotate = { 0.0f, 0.0f, -std::numbers::pi_v<float> / 2 };
	transform_.scale = { 0.5f, 0.5f, 0.5f };


	// ---- Trail（剣軌跡用）----
	Trail::Config cfg{};
	cfg.maxSegments = 48;
	cfg.lifeTime = 0.25f;
	cfg.minDistance = 0.01f;
	cfg.texturePath = "Assets/.EngineResource/Texture/white1x1.png";
	trail_.Initialize(textureManager_, cfg);

	active_ = false;
	emitting_ = false;
	time_ = 0.0f;
}


void TestTrail4::Stop()
{
}

void TestTrail4::Update(float deltaTime, const Matrix4x4& vpMatrix)
{
	time_ += deltaTime;

	transform_.rotate.x += deltaTime;
	Matrix4x4 world = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ = world * vpMatrix;
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));

	const Vector4 originV4 = swordModelData_.vertices[vertexOriginIndex_].position;
	const Vector4 tipV4 = swordModelData_.vertices[vertexTipIndex_].position;

	const Vector3 originLS(originV4.x, originV4.y, originV4.z);
	const Vector3 tipLS(tipV4.x, tipV4.y, tipV4.z);

	Origin_ = originLS * world;
	tip_ = tipLS * world;

	trail_.PushSegment(Origin_, tip_);
	
	trail_.Update(deltaTime);
}

void TestTrail4::Draw(CmdObj* cmdObj)
{
#ifdef USE_IMGUI

	ImGui::Begin("TestTrail4");
	ImGui::SliderInt("origin index", &vertexOriginIndex_, 0, static_cast<int>(swordModelData_.vertices.size()) - 1);
	ImGui::SliderInt("tip index", &vertexTipIndex_, 0, static_cast<int>(swordModelData_.vertices.size()) - 1);
	ImGui::End();

#endif // 



	render_->Draw(cmdObj);
}