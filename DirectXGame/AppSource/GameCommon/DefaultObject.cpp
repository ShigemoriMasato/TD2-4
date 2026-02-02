#include"DefaultObject.h"
#include"Utility/MatrixFactory.h"
#include"LightManager.h"

void DefaultObject::Initialize(DrawData drawData, int texture) {

	// psoを設定
	renderObject_ = std::make_unique<RenderObject>();
	renderObject_->Initialize();
	renderObject_->psoConfig_.inputLayoutID = InputLayoutID::Default;
	renderObject_->psoConfig_.vs = "Object3d.VS.hlsl";
	renderObject_->psoConfig_.ps = "Object3d.PS.hlsl";

	// 描画モデルを設定
	renderObject_->SetDrawData(drawData);
	renderObject_->SetUseTexture(true);

	// worldTransformを登録
	vsDataIndex_ = renderObject_->CreateCBV(sizeof(TransformationMatrix), ShaderType::VERTEX_SHADER, "TestScene::VSData");

	// Materialを登録
	psDataIndex_ = renderObject_->CreateCBV(sizeof(Material), ShaderType::PIXEL_SHADER, "TestScene::psData");

	// ライトを登録
	lightDataIndex_ = renderObject_->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "TestScene::psData");

	//カメラの座標
	renderObject_->CreateCBV(sizeof(Vector3), ShaderType::PIXEL_SHADER, "CameraPos");

	// 色を設定
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.textureIndex = texture;
}

void DefaultObject::Update() {
	// worldTransformを更新
	worldMatrix_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	vsData_.World = worldMatrix_;
	vsData_.worldInverseTranspose = Matrix::InverseMatrix(worldMatrix_);
}

void DefaultObject::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// カメラを設定
	vsData_.WVP = worldMatrix_ * vpMatrix;
	renderObject_->CopyBufferData(vsDataIndex_, &vsData_, sizeof(TransformationMatrix));
	renderObject_->CopyBufferData(psDataIndex_, &material_, sizeof(Material));
	renderObject_->CopyBufferData(lightDataIndex_, &LightManager::GetInstance()->GetDirLights()[0], sizeof(DirectionalLight));
	Vector3 cameraPos = LightManager::GetInstance()->GetCameraPos();
	renderObject_->CopyBufferData(lightDataIndex_ + 1, &cameraPos, sizeof(Vector3));

	// 描画
	renderObject_->Draw(window);
}