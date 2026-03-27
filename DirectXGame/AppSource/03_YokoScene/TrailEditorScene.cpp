#include "TrailEditorScene.h"
#include <imgui/imgui.h>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <numbers>
#include "03_YokoScene/YokoScene.h"
#include "PrticleEditorScene.h"

using namespace SHEngine;

namespace
{
	std::unique_ptr<RenderObject> CreateTexturedModelRO(
		DrawDataManager* drawDataManager,
		const NodeModelData& modelData,
		int textureIndex)
	{
		auto ro = std::make_unique<RenderObject>();
		ro->Initialize();

		ro->psoConfig_.vs = "Game/Field.VS.hlsl";
		ro->psoConfig_.ps = "Game/Field.PS.hlsl";
		ro->SetUseTexture(true);

		ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		ro->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

		const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
		ro->SetDrawData(drawData);

		const Vector4 color = { 1,1,1,1 };
		ro->CopyBufferData(1, &color, sizeof(Vector4));
		ro->CopyBufferData(2, &textureIndex, sizeof(int));

		return ro;
	}

	std::unique_ptr<RenderObject> CreateColorMarkerRO(
		DrawDataManager* drawDataManager,
		const NodeModelData& modelData,
		const char* debugName)
	{
		auto ro = std::make_unique<RenderObject>(debugName);
		ro->Initialize();

		ro->psoConfig_.vs = "Simple.VS.hlsl";
		ro->psoConfig_.ps = "Color.PS.hlsl";
		ro->SetUseTexture(false);

		ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

		const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
		ro->SetDrawData(drawData);

		return ro;
	}

	Matrix4x4 MakeWorld(const Transform& tr)
	{
		return Matrix::MakeAffineMatrix(tr.scale, tr.rotate, tr.position);
	}
}

void TrailEditorScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->SetPosition({ 0.0f, 3.0f, -10.0f });
	camera_->Initialize(input_);

	modelTransform_.position = { 0.0f, 0.0f, 0.0f };
	modelTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	modelTransform_.scale = { 1.0f, 1.0f, 1.0f };

	// 編集データ初期化
	Reset(TrailType::RibbonTrail);

	// "Assets/Model/Item/Weapon"以下のモデルをリストアップしてmodelRenders_作成(名前とモデルパスのみ保存)
	BuildModelList();
	// モデル用RenderObjectインスタンス作成
	CreateModelRender();
	// Marker用RenderObjectインスタンス作成
	CreateMarkerRenders();

	// 初期モデルを選択
	SelectModel(0);

	RebuildTrail();
	LoadTrailData();
}

// 編集データ初期化
void TrailEditorScene::Reset(TrailType type)
{
	currentType_ = type;

	trailConfig_ = Trail::Config{};
	std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), trailConfig_.texturePath.c_str(), _TRUNCATE);

	// ribbon
	ribbonPreset_ = RibbonTrailConfig{};
	ribbonPreset_.cfg = trailConfig_;

	// shock
	shockPreset_ = ShockwaveRingConfig{};
	shockPreset_.cfg = trailConfig_;
}

// "Assets/Model/Item/Weapon"以下のモデルをリストアップしてmodelRenders_作成
void TrailEditorScene::BuildModelList()
{
	modelRenders_.clear();

	//const char* kWeaponDir = "Assets/Model/Item/Weapon";
	const char* kWeaponDir = "Assets/Model";
	std::error_code ec;
	if (!std::filesystem::exists(kWeaponDir, ec))
	{
		return;
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(kWeaponDir))
	{
		// is_directory() = それがフォルダかファイルか
		if (!entry.is_directory()) continue;

		// 子フォルダがある場合はスキップ
		bool hasChildDir = false;
		for (const auto& c : std::filesystem::directory_iterator(entry.path()))
		{
			if (c.is_directory()) { hasChildDir = true; break; }
		}
		if (hasChildDir) continue;

		// ここまで来たentryは最下層のフォルダ
		auto data = std::make_unique<DrawDataUnit>();
		data->modelPath = entry.path().generic_string();
		data->name = entry.path().filename().generic_string();
		modelRenders_.push_back(std::move(data));
	}
}

// RenderObJectのインスタンスを作成
void TrailEditorScene::CreateModelRender()
{
	for (int i = 0; i <  modelRenders_.size(); ++i)
	{
		modelRenders_[i]->modelHandle = modelManager_->LoadModel(modelRenders_[i]->modelPath);
		auto modelData = modelManager_->GetNodeModelData(modelRenders_[i]->modelHandle);
		modelRenders_[i]->textureIndex = modelData.materials[modelData.materialIndex.front()].textureIndex;
		modelRenders_[i]->render = CreateTexturedModelRO(drawDataManager_, modelData, modelRenders_[i]->textureIndex);
	}
}
void TrailEditorScene::CreateMarkerRenders()
{
	// マーカー用のキューブモデルロード
	for (int i = 0; i < 2; ++i)
	{
		marker[i].modelHandle = modelManager_->LoadModel("Assets/.EngineResource/Model/Cube");
		auto modelData = modelManager_->GetNodeModelData(marker[i].modelHandle);
		marker[i].textureIndex = 0;
		marker[i].render = CreateTexturedModelRO(drawDataManager_, modelData, marker[i].textureIndex);
	}
}

// 選択モデルを変更
void TrailEditorScene::SelectModel(int index)
{
	if (index < 0 || index >= int(modelRenders_.size())) return;
	selectedModelIndex_ = index;

	// モデル選択が変わったらデフォルト名を設定
	SetDefaultName();
}
void TrailEditorScene::SetDefaultName()
{
	std::string type;
	if (currentType_ == TrailType::RibbonTrail) type = "_Ribbon";
	else if (currentType_ == TrailType::ShockwaveRing) type = "_Shockwave";
	std::string def = modelRenders_[selectedModelIndex_]->name + type;
	std::memset(presetNameBuf_, 0, sizeof(presetNameBuf_));
	strncpy_s(presetNameBuf_, sizeof(presetNameBuf_), def.c_str(), _TRUNCATE);
}

// Trailの再構築(config更新&テクスチャ再設定)
void TrailEditorScene::RebuildTrail()
{
	// プレビュー用Trailは共通で1つ。typeに応じた設定を適用する
	if (currentType_ == TrailType::RibbonTrail)
	{
		trail_.Initialize(drawDataManager_, textureManager_, trailConfig_);
		trail_.SetTexture(trailConfig_.texturePath);
		trail_.Clear();
	}
	else if (currentType_ == TrailType::ShockwaveRing)
	{
		trail_.Initialize(drawDataManager_, textureManager_, trailConfig_);
		trail_.SetTexture(trailConfig_.texturePath);
		trail_.Clear();
	}
}

// データ保存
void TrailEditorScene::SaveTrailData()
{
	if (presetNameBuf_[0] == '\0') return;

	const std::string fileBaseName = presetNameBuf_;
	json_.Boot(fileBaseName);

	// 共通：type
	{
		std::string type = ToString(currentType_);
		json_.Add("type", type);
	}

	// 共通：cfg
	{
		int maxSeg = trailConfig_.maxSegments;
		float life = trailConfig_.lifeTime;
		float minDist = trailConfig_.minDistance;

		Vector4 cN = trailConfig_.colorNormal;
		Vector4 cA = trailConfig_.colorAdd;

		bool dn = trailConfig_.drawNormal;
		bool da = trailConfig_.drawAdd;

		std::string tex = trailConfig_.texturePath;

		json_.Add("cfg.maxSegments", maxSeg);
		json_.Add("cfg.lifeTime", life);
		json_.Add("cfg.minDistance", minDist);

		json_.Add("cfg.colorNormal", cN);
		json_.Add("cfg.colorAdd", cA);

		json_.Add("cfg.drawNormal", dn);
		json_.Add("cfg.drawAdd", da);

		json_.Add("cfg.texturePath", tex);
	}

	// type固有
	if (currentType_ == TrailType::RibbonTrail)
	{
		std::string modelName = modelRenders_[selectedModelIndex_]->name;
		json_.Add("ribbon.modelName", modelName);

		Vector3 o = ribbonPreset_.originLocal;
		Vector3 t = ribbonPreset_.tipLocal;
		json_.Add("ribbon.originLocal", o);
		json_.Add("ribbon.tipLocal", t);
	}
	else if (currentType_ == TrailType::ShockwaveRing)
	{
		int seg = shockPreset_.segments;
		float dur = shockPreset_.duration;
		float rs = shockPreset_.radiusStart;
		float re = shockPreset_.radiusEnd;
		float th = shockPreset_.thickness;
		float na = shockPreset_.noiseAmp;
		float nf = shockPreset_.noiseFreq;

		json_.Add("shock.segments", seg);
		json_.Add("shock.duration", dur);
		json_.Add("shock.radiusStart", rs);
		json_.Add("shock.radiusEnd", re);
		json_.Add("shock.thickness", th);
		json_.Add("shock.noiseAmp", na);
		json_.Add("shock.noiseFreq", nf);
	}

	json_.Save();
}
// データ読み込み
void TrailEditorScene::LoadTrailData()
{
	if (presetNameBuf_[0] == '\0') return;

	const std::string fileBaseName = presetNameBuf_;

	TrailPresetVariant v{};
	try
	{
		v = presetDataBank_.Get(fileBaseName);
	}
	catch (...)
	{
		// エディタ用途：ファイルが無い/壊れてる等は黙って何もしない（現状の挙動に合わせる）
		return;
	}

	std::visit([&](const auto& preset)
		{
			using T = std::decay_t<decltype(preset)>;

			trailConfig_ = preset.cfg;

			// ImGui用バッファ同期
			std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
			strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), trailConfig_.texturePath.c_str(), _TRUNCATE);

			if constexpr (std::is_same_v<T, RibbonTrailConfig>)
			{
				currentType_ = TrailType::RibbonTrail;
				ribbonPreset_ = preset;
			}
			else if constexpr (std::is_same_v<T, ShockwaveRingConfig>)
			{
				currentType_ = TrailType::ShockwaveRing;
				shockPreset_ = preset;
			}
		}, v);

	requestRebuildTrail_ = true;
}


void TrailEditorScene::DrawConfigUI_()
{
#ifdef USE_IMGUI

	requestRebuildTrail_ |= ImGui::DragInt("cfg.maxSegments", &trailConfig_.maxSegments, 1.0f, 1, 512);
	requestRebuildTrail_ |= ImGui::DragFloat("cfg.lifeTime", &trailConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("cfg.minDistance", &trailConfig_.minDistance, 0.001f, 0.0f, 10.0f);

	requestRebuildTrail_ |= ImGui::ColorEdit4("cfg.colorNormal", &trailConfig_.colorNormal.x);
	requestRebuildTrail_ |= ImGui::ColorEdit4("cfg.colorAdd", &trailConfig_.colorAdd.x);

	requestRebuildTrail_ |= ImGui::Checkbox("cfg.drawNormal", &trailConfig_.drawNormal);
	requestRebuildTrail_ |= ImGui::Checkbox("cfg.drawAdd", &trailConfig_.drawAdd);

	if (ImGui::InputText("cfg.texturePath", texturePathBuf_, sizeof(texturePathBuf_)))
	{
		trailConfig_.texturePath = texturePathBuf_;
		requestRebuildTrail_ = true;
	}

#endif
}

void TrailEditorScene::DrawRibbonUI_()
{
#ifdef USE_IMGUI

	ImGui::SeparatorText("Ribbon (2Point)");
	requestRebuildTrail_ |= ImGui::DragFloat3("ribbon.originLocal", &ribbonPreset_.originLocal.x, 0.01f);
	requestRebuildTrail_ |= ImGui::DragFloat3("ribbon.tipLocal", &ribbonPreset_.tipLocal.x, 0.01f);

#endif
}
void TrailEditorScene::DrawShockwaveUI_()
{
#ifdef USE_IMGUI

	ImGui::SeparatorText("Shockwave Ring");
	requestRebuildTrail_ |= ImGui::DragInt("shock.segments", &shockPreset_.segments, 1.0f, 3, 512);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.duration", &shockPreset_.duration, 0.01f, 0.01f, 10.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.radiusStart", &shockPreset_.radiusStart, 0.01f, 0.0f, 100.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.radiusEnd", &shockPreset_.radiusEnd, 0.01f, 0.0f, 100.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.thickness", &shockPreset_.thickness, 0.01f, 0.0f, 100.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.noiseAmp", &shockPreset_.noiseAmp, 0.01f, 0.0f, 100.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.noiseFreq", &shockPreset_.noiseFreq, 0.01f, 0.0f, 100.0f);

#endif
}

void TrailEditorScene::DrawImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("TrailEditor");

	// Preset名
	ImGui::InputText("PresetName (no ext)", presetNameBuf_, sizeof(presetNameBuf_));

	// type
	{
		int t = (currentType_ == TrailType::RibbonTrail) ? 0 : 1;
		const char* items[] = { "RibbonTrail", "ShockwaveRing" };
		if (ImGui::Combo("type", &t, items, 2))
		{
			currentType_ = (t == 0) ? TrailType::RibbonTrail : TrailType::ShockwaveRing;

			// モデル名サフィックスも更新
			if (selectedModelIndex_ >= 0)
			{
				std::string suffix = (currentType_ == TrailType::RibbonTrail) ? "_Ribbon" : "_Shockwave";
				std::string def = modelRenders_[selectedModelIndex_]->name + suffix;
				std::memset(presetNameBuf_, 0, sizeof(presetNameBuf_));
				strncpy_s(presetNameBuf_, sizeof(presetNameBuf_), def.c_str(), _TRUNCATE);
			}

			requestRebuildTrail_ = true;
		}
	}

	if (ImGui::BeginListBox("Weapon Models"))
	{
		for (int i = 0; i < (int)modelRenders_.size(); ++i)
		{
			const bool selected = (i == selectedModelIndex_);
			if (ImGui::Selectable(modelRenders_[i]->name.c_str(), selected))
			{
				SelectModel(i);
				// モデル切替時に見た目が変わる可能性があるので再構築
				requestRebuildTrail_ = true;
			}
		}
		ImGui::EndListBox();
	}

	ImGui::SeparatorText("Config (shared keys)");
	DrawConfigUI_();

	if (currentType_ == TrailType::RibbonTrail)DrawRibbonUI_();
	else DrawShockwaveUI_();

	ImGui::Separator();
	ImGui::Checkbox("Emit Trail", &emitTrail_);

	if (ImGui::Button("Save"))
	{
		SaveTrailData();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		LoadTrailData();
	}

	ImGui::Text("File: Assets/Json/%s.json", presetNameBuf_);

	ImGui::End();

	ImGui::Begin("modelTransform");
	ImGui::DragFloat3("position", &modelTransform_.position.x, 0.01f);
	ImGui::DragFloat3("rotate", &modelTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &modelTransform_.scale.x, 0.01f);
	ImGui::End();

#endif
}

void TrailEditorScene::UpdateRenders(const Matrix4x4& vpMatrix)
{
	if (selectedModelIndex_ < 0) return;

	const Matrix4x4 modelWorld = MakeWorld(modelTransform_);

	// モデル
	{
		const Matrix4x4 wvp = modelWorld * vpMatrix;
		const Vector4 color = { 1,1,1,1 };
		auto& ro = modelRenders_[selectedModelIndex_]->render;

		ro->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		ro->CopyBufferData(1, &color, sizeof(Vector4));
		ro->CopyBufferData(2, &modelRenders_[selectedModelIndex_]->textureIndex, sizeof(int));
	}

	markerPos[0] = ribbonPreset_.originLocal * modelWorld;
	markerPos[1] = ribbonPreset_.tipLocal * modelWorld;

	// marker origin
	{
		for (int i = 0; i < 2; ++i)
		{
			Transform tr{};
			tr.position = markerPos[i];
			tr.scale = { 0.5f, 0.5f, 0.5f };
			const Matrix4x4 wvp = MakeWorld(tr) * vpMatrix;
			const Vector4 color = { 1.0f, 0.2f, 0.2f, 1.0f };
			auto& ro = marker[i].render;

			ro->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
			ro->CopyBufferData(1, &color, sizeof(Vector4));
			ro->CopyBufferData(2, &marker[i].textureIndex, sizeof(int));
		}
	}
}

std::unique_ptr<IScene> TrailEditorScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	camera_->Update();
	DrawImGui();

	if (requestRebuildTrail_)
	{
		requestRebuildTrail_ = false;
		RebuildTrail();
	}

	const Matrix4x4 vp = camera_->GetVPMatrix();
	UpdateRenders(vp);

	// プレビュー
	if (emitTrail_)
	{
		if (currentType_ == TrailType::RibbonTrail)
		{
			trail_.PushSegment(markerPos[0], markerPos[1]);
		}
		else
		{
			// Shockwaveプレビュー：中心はモデル原点、法線はY+
			// 毎フレームClearしてリング生成（Runtimeと同じ思想）
			trail_.Clear();

			const Vector3 center = modelTransform_.position;
			const Vector3 normal = { 0.0f,1.0f,0.0f };

			// 簡易：ShockwaveRingTrailと同等の基底をここで作る
			const float t = 0.0f; // プレビューは固定（半径Endで表示したい等は後で拡張）
			const float radius = shockPreset_.radiusEnd;

			Vector3 n = normal;
			Vector3 ref = (std::abs(n.y) > 0.99f) ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(0.0f, 1.0f, 0.0f);
			auto cross = [](const Vector3& a, const Vector3& b)
				{
					return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
				};
			auto norm = [](const Vector3& v)
				{
					const float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
					if (len <= 1e-6f) return Vector3(0, 1, 0);
					return Vector3(v.x / len, v.y / len, v.z / len);
				};

			Vector3 u = norm(cross(ref, n));
			Vector3 v = norm(cross(n, u));

			const int seg = std::max(3, shockPreset_.segments);
			const float twoPi = std::numbers::pi_v<float> *2.0f;

			for (int i = 0; i < seg; ++i)
			{
				const float a = (float)i / (float)seg * twoPi;
				const float s = std::sin(a);
				const float c = std::cos(a);

				const Vector3 pos = center + u * (c * radius) + v * (s * radius);
				Vector3 tangent = norm((u * (-s)) + (v * (c)));
				const Vector3 baseWS = pos - tangent * (shockPreset_.thickness * 0.5f);
				const Vector3 tipWS = pos + tangent * (shockPreset_.thickness * 0.5f);
				trail_.PushSegment(baseWS, tipWS);
			}
		}
	}

	trail_.Update(dt, vp);

	// Zキーで決定
	if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z))
	{
		return std::make_unique<YokoScene>();
	}

	return nullptr;
}

void TrailEditorScene::Draw()
{
	if (selectedModelIndex_ < 0) return;

	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	modelRenders_[selectedModelIndex_]->render->Draw(cmdObj);

	// Ribbon時マーカー描画
	if (currentType_ == TrailType::RibbonTrail)
	{
		for (int i = 0; i < 2; ++i)
		{
			marker[i].render->Draw(cmdObj);
		}
	}

	trail_.Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}