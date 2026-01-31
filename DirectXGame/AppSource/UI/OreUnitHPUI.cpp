#include"OreUnitHPUI.h"
#include"Utility/MatrixFactory.h"
#include<numbers>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

void OreUnitHPUI::Initialize(DrawData wallDrawData) {

	// psoを設定
	renderObject_ = std::make_unique<RenderObject>();
	renderObject_->Initialize();
	renderObject_->psoConfig_.inputLayoutID = InputLayoutID::Default;
	renderObject_->psoConfig_.vs = "Instancing3d.VS.hlsl";
	renderObject_->psoConfig_.ps = "Instancing3d.PS.hlsl";

	// 描画モデルを設定
	renderObject_->SetDrawData(wallDrawData);
	renderObject_->SetUseTexture(true);

	// worldTransformを登録
	vsDataIndex_ = renderObject_->CreateSRV(sizeof(ParticleForGPU), maxNum_, ShaderType::VERTEX_SHADER, "TestScene::SkinningMatrices");

	// 初期化
	TransformData transformData;
	transformDatas_.reserve(maxNum_);
	for (uint32_t i = 0; i < maxNum_; ++i) {
		transformData.transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{} };
		transformData.worldMatrix = Matrix::MakeAffineMatrix(transformData.transform.scale, transformData.transform.rotate, transformData.transform.position);
		transformData.color = { 1.0f,1.0f,1.0f,1.0f };
		transformData.textureHandle = 0;
		transformDatas_.push_back(transformData);
	}

	// 単位行列を書き込んでおく
	instancingData_.resize(transformDatas_.size());
	for (uint32_t index = 0; index < transformDatas_.size(); ++index) {
		instancingData_[index].WVP = Matrix4x4::Identity();
		instancingData_[index].World = Matrix4x4::Identity();
		instancingData_[index].color = { 1.0f,1.0f,1.0f,1.0f };
		instancingData_[index].textureHandle = 0;
	}

	// 値を設定
	renderObject_->CopyBufferData(vsDataIndex_, instancingData_.data(), sizeof(ParticleForGPU) * instancingData_.size());
}

void OreUnitHPUI::Update() {

	// 現在の数をリセット
	index_ = 0;

}

void OreUnitHPUI::Add(const Vector3& pos, const int32_t& hp, const int32_t& maxHp) {
	
	// hpの比率を取得
	float p = static_cast<float>(hp) / static_cast<float>(maxHp);

	for (int32_t i = index_; i < index_ + 3; ++i) {

		transformDatas_[i].transform.rotate.x = rotX_;

		if (i == index_) {
			// 外枠
			transformDatas_[i].transform.position = pos + Vector3(-baseScale_.x * 0.6f,0.0f,0.0f);
			transformDatas_[i].transform.scale = baseScale_ + Vector3(0.2f,0.2f,0.0f);
			transformDatas_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		} else if (i == index_ + 1) {
			// 中
			transformDatas_[i].transform.position = pos + Vector3(-baseScale_.x * 0.5f,0.01f,-0.01f);
			transformDatas_[i].transform.scale = baseScale_;
			transformDatas_[i].color = { 0.2f,0.2f,0.2f,1.0f };
		} else if (i == index_ + 2) {
			// ゲージ
			transformDatas_[i].transform.position = pos + Vector3(-baseScale_.x * 0.5f, 0.011f, -0.011f);
			transformDatas_[i].transform.scale = baseScale_;
			transformDatas_[i].transform.scale.x = p * baseScale_.x;
			transformDatas_[i].color = { 0.0f,1.0f,0.0f,1.0f };
		}

		transformDatas_[i].worldMatrix = Matrix::MakeAffineMatrix(transformDatas_[i].transform.scale, transformDatas_[i].transform.rotate, transformDatas_[i].transform.position);
	}
	// 数をカウント
	index_ += 3;
}

void OreUnitHPUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画個数
	renderObject_->instanceNum_ = index_;

	// カメラの位置を適応
	for (uint32_t i = 0; i < renderObject_->instanceNum_; ++i) {
		instancingData_[i].WVP = transformDatas_[i].worldMatrix * vpMatrix;
		instancingData_[i].World = transformDatas_[i].worldMatrix;
		instancingData_[i].color = transformDatas_[i].color;
		instancingData_[i].textureHandle = transformDatas_[i].textureHandle;
	}

	// 値を設定
	renderObject_->CopyBufferData(vsDataIndex_, instancingData_.data(), sizeof(ParticleForGPU) * instancingData_.size());

	// 描画
	renderObject_->Draw(window);

	//ImGui::Begin("tst");
	//ImGui::DragFloat("rot", &rotX_, 0.1f);
	//ImGui::DragFloat3("sca", &baseScale_.x, 0.1f);
	//ImGui::End();
}