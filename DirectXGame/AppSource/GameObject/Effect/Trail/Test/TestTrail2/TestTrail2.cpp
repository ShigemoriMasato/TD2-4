#include "TestTrail2.h"

#include <cmath>
#include <algorithm>
#include "imgui/imgui.h"

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

void TestTrail2::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager)
{
	Trail::Config cfg{};
	cfg.maxSegments = 32;
	cfg.lifeTime = 0.35f;
	cfg.minDistance = 0.03f;

	trail_.Initialize(textureManager, cfg);

	// テスト用の初期位置
	swordCenterWS_ = { 0.0f, 2.0f, 0.0f };
	swordUpWS_ = { 0.0f, 1.0f, 0.0f };
}

void TestTrail2::Update(float deltaTime, const Matrix4x4& vpMatrix)
{
	time_ += deltaTime;

	// 剣の軸（長手方向）
	const Vector3 up = NormalizeSafe(swordUpWS_);

	// upに直交する基底right/forwardを作る（上ベクトルがほぼYならZを使う）
	Vector3 ref = (std::abs(up.y) > 0.99f) ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(0.0f, 1.0f, 0.0f);
	Vector3 right = NormalizeSafe(Cross(ref, up));
	Vector3 forward = NormalizeSafe(Cross(up, right));

	// 螺旋：高さ方向に少しだけずらしながら回転
	const float t = time_;

	// 2点（base/tip）は「リング断面の両端」として作る
	// base/tipをリング中心の両側に置くと“帯”になる
	const float phase = t * spinSpeed_;
	const float radial = auraRadius_ + std::sin(t * waveSpeed_) * waveAmp_;

	// 剣の中心から、長手方向に沿ってオフセット（螺旋っぽく）
	const float along = std::sin(t * 1.7f) * (auraLength_ * 0.15f);
	const Vector3 center = swordCenterWS_ + up * along;

	// リング上の方向ベクトル
	const float s = std::sin(phase);
	const float c = std::cos(phase);
	const Vector3 ringDir = right * c + forward * s;

	// リング中心点
	const Vector3 ringPos = center + ringDir * radial;

	// 帯の幅方向（ringDir に直交し、up にも直交する方向）
	Vector3 widthDir = NormalizeSafe(Cross(up, ringDir));
	if (std::abs(widthDir.x) + std::abs(widthDir.y) + std::abs(widthDir.z) <= 1e-6f)
	{
		widthDir = right;
	}

	// base/tip（WS）
	const Vector3 baseWS = ringPos - widthDir * (auraWidth_ * 0.5f);
	const Vector3 tipWS = ringPos + widthDir * (auraWidth_ * 0.5f);

	trail_.PushSegment(baseWS, tipWS);
	trail_.Update(deltaTime, vpMatrix);
}

void TestTrail2::Draw(CmdObj* cmdObj)
{
#ifdef USE_IMGUI

	ImGui::Begin("TestTrail2 Config");


	ImGui::DragFloat("Radius", &auraRadius_, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("Width", &auraWidth_, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("Length", &auraLength_, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("Spin Speed", &spinSpeed_, 0.1f, 0.0f, 20.0f);
	ImGui::DragFloat("Wave Speed", &waveSpeed_, 0.1f, 0.0f, 20.0f);
	ImGui::DragFloat("Wave Amp", &waveAmp_, 0.01f, 0.0f, 5.0f);


	ImGui::End();

#endif 

}