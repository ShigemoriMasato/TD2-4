#include"OreOutLineObject.h"
#include"Utility/MatrixFactory.h"

void OreOutLineObject::Initialize(DrawData drawData) {

	// psoを設定
	renderObject_ = std::make_unique<RenderObject>();
	renderObject_->Initialize();
	renderObject_->psoConfig_.inputLayoutID = InputLayoutID::Default;
	renderObject_->psoConfig_.vs = "Object3d.VS.hlsl";
	renderObject_->psoConfig_.ps = "Object3d.PS.hlsl";
	renderObject_->psoConfig_.rasterizerID = RasterizerID::Back;

	// 描画モデルを設定
	renderObject_->SetDrawData(drawData);
	renderObject_->SetUseTexture(true);

	// worldTransformを登録
	vsDataIndex_ = renderObject_->CreateCBV(sizeof(TransformationMatrix), ShaderType::VERTEX_SHADER, "TestScene::VSData");

	// Materialを登録
	psDataIndex_ = renderObject_->CreateCBV(sizeof(Material), ShaderType::PIXEL_SHADER, "TestScene::psData");

	transform_.scale = { 1.2f,1.2f,1.2f };

	// 色を設定
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	renderObject_->CopyBufferData(psDataIndex_, &material_, sizeof(Material));
}

void OreOutLineObject::Update() {
	// worldTransformを更新
	worldMatrix_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	vsData_.World = worldMatrix_;
	vsData_.worldInverseTranspose = Matrix::InverseMatrix(worldMatrix_);
}

void OreOutLineObject::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// カメラによる位置を設定
	vsData_.WVP = worldMatrix_ * vpMatrix;
	renderObject_->CopyBufferData(vsDataIndex_, &vsData_, sizeof(TransformationMatrix));

	// 描画
	renderObject_->Draw(window);
}