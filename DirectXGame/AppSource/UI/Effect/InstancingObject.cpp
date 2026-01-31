#include"InstancingObject.h"
#include"Utility/MatrixFactory.h"
#include<numbers>

void InstancingObject::Initialize(DrawData spriteDrawData) {

	// psoを設定
	renderObject_ = std::make_unique<RenderObject>();
	renderObject_->Initialize();
	renderObject_->psoConfig_.inputLayoutID = InputLayoutID::Default;
	renderObject_->psoConfig_.vs = "Instancing3d.VS.hlsl";
	renderObject_->psoConfig_.ps = "Instancing3d.PS.hlsl";

	// 描画モデルを設定
	renderObject_->SetDrawData(spriteDrawData);
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
		instancingData_[index].textureHandle = texture_;
	}

	// 値を設定
	renderObject_->CopyBufferData(vsDataIndex_, instancingData_.data(), sizeof(ParticleForGPU) * instancingData_.size());
}

void InstancingObject::Update() {

	for (size_t i = 0; i < index_; ++i) {
		transformDatas_[i].worldMatrix = Matrix::MakeAffineMatrix(transformDatas_[i].transform.scale, transformDatas_[i].transform.rotate, transformDatas_[i].transform.position);
	}
}

void InstancingObject::Draw(Window* window, const Matrix4x4& vpMatrix) {

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
}