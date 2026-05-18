#include "EnemyEffectManager.h"
#include <GameObject/Effect/Particle/IParticle.h>

EnemyEffect::EnemyEffect()
{}

EnemyEffect::~EnemyEffect()
{}

void EnemyEffect::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager, CommonData* commonData)
{
	textureManager_ = textureManager;
	modelManager_ = modelManager;
	commonData_ = commonData;
}

void EnemyEffect::SetCameraPos(Vector3 pos)
{
	for (auto& [key, data] : multiParticles_)
	{
		data.multiParticle.SetCameraPos(pos);
	}
}

void EnemyEffect::Update(float dt)
{
	// Update:新たに発生させる。発生済のものを更新する。
	for (auto& [key, data] : multiParticles_)
	{
		data.multiParticle.Update(dt);
	}
	for (auto& [key, data] : multiTrails_)
	{
		data.multiTrail.Update(dt);
	}
	for (auto& [key, data] : trailOnParticle_)
	{
		data.trailOnParticle.Update(dt);
	}

	// Delete:oneShotがtrueの時は二回目の発生で削除する。
	//for (auto it = multiParticles_.begin(); it != multiParticles_.end(); )
	//{
	//	// oneShotがtrue && ThisFrameで発生した
	//	if (it->second.oneShot && it->second.multiParticle.GetIsJustEmitted(0))
	//	{
	//		// 2回目の発生で削除する
	//		if (it->second.emittedOnce)
	//		{
	//			it = multiParticles_.erase(it);
	//			continue;
	//		}
	//		// カウントを進める
	//		it->second.emittedOnce = true;
	//	}
	//	++it;
	//}

	for (auto it = trailOnParticle_.begin(); it != trailOnParticle_.end();)
	{
		// oneShotがtrue && ThisFrameで発生した
		if (it->second.oneShot && it->second.trailOnParticle.GetParticle().GetIsJustEmitted(0))
		{
			// 2回目の発生で削除する
			if (it->second.emittedOnce)
			{
				it = trailOnParticle_.erase(it);
				continue;
			}
			// カウントを進める
			it->second.emittedOnce = true;
		}
		++it;
	}
}

void EnemyEffect::Draw()
{
	// Update:新たに発生させる。発生済のものを更新する。
	for (auto& [key, data] : multiParticles_)
	{
		data.multiParticle.Draw();
	}
	for (auto& [key, data] : multiTrails_)
	{
		data.multiTrail.Draw();
	}
	for (auto& [key, data] : trailOnParticle_)
	{
		data.trailOnParticle.Draw();
	}
}

void EnemyEffect::CreateDeathEffect1(const Matrix4x4& world)
{
	// 死亡エフェクトの例
	MultiParticleData particleData;
	particleData.multiParticle.Initialize(textureManager_, modelManager_, commonData_);
	//particleData.multiParticle.Add("death2");
	//particleData.multiParticle.Add("death2_2");
	//particleData.multiParticle.Add("death2_3");
	particleData.multiParticle.Add("big.json");
	particleData.multiParticle.SetModelWorld(world);
	particleData.oneShot = true; // 一回の発生で削除する
	multiParticles_.emplace(nextMultiParticleId_++, std::move(particleData));
}
