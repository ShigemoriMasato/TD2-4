#pragma once
#include <Collision/Collider.h>
#include <GameObject/DrawInfo.h>
#include <Render/RenderObject.h>

class IAttackObject : public Collider {
public:

	struct Config {
		Vector3 position = { 0.0f, 0.0f, 0.0f };
		float direction = 0.0f;
		float spreadAngle = 0.0f;
		float attackPower = 0.0f;
		float range = 0.0f;
		float speed = 0.0f;
		float penetration = 0.0f;
		float knockbackPower = 0.0f;
		float criticalChance = 0.0f;
		float criticalMultiplier = 0.0f;
		float lifeSteelChance = 0.0f;
	};

	virtual void Initialize(const Config& config) { config_ = config; }
	virtual void Update(float deltaTime) {}
	//何か特殊な描画をする場合はここで行う
	virtual void Draw(CmdObj* cmdObj) {}

	bool IsActive() { return isActive_; }

	virtual DrawInfo GetDrawInfo() = 0;

	virtual int GetDamage() const { return static_cast<int>(config_.attackPower); }

protected:

	//ポインターだけセットしたコンフィグを、それ以外の設定を付与して設定する関数
	void SetCollider(CollConfig& config);

	bool isActive_ = true;
	Config config_{};
};
