#pragma once
#include "GameObject/Effect/Particle/IParticle.h"

#include <Render/RenderObject.h>
#include <memory>
#include <vector>

class Particle1 : public IParticle
{
public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix) override;

	void Trigger(const Vector3& pos) override;
	void Stop() override;

private:
	struct PrticleUnit
	{
		Vector3 pos{};
		Vector3 vel{};
		float age = 0.0f;
		float life = 1.0f;
	};

	void Spawn(int count);

private:
	std::unique_ptr<SHEngine::RenderObject> renderObject_;

	// 噴水の発生原点（Triggerで更新）
	Vector3 origin_{ 0.0f, 0.0f, 0.0f };

	// 再生状態：Stopでfalse、Triggerでtrue
	bool isEmitting_ = false;

	int maxParticles_ = 512;
	float emitRate_ = 240.0f; // 1秒あたり生成数
	float emitAcc_ = 0.0f;

	float minLife_ = 0.35f;
	float maxLife_ = 0.75f;

	float minSpeed_ = 4.0f;
	float maxSpeed_ = 7.5f;

	float spread_ = 0.6f;   // 横方向拡散
	float gravity_ = 12.5f; // 重力

	std::vector<PrticleUnit> particles_;

	// インスタンス描画用
	std::vector<Matrix4x4> worlds_;
	std::vector<Vector4> colors_;
};