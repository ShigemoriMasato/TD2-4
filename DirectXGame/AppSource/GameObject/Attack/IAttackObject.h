#pragma once
#include <Collision/Collider.h>
#include <GameObject/DrawInfo.h>
#include <Render/RenderObject.h>

class IAttackObject : public Collider {
public:

	virtual void Initialize(float size) { size_ = size; }
	virtual void Update(float deltaTime) {}
	//何か特殊な描画をする場合はここで行う
	virtual void Draw(CmdObj* cmdObj) {}

	bool IsActive() { return isActive_; }

	virtual DrawInfo GetDrawInfo() = 0;

protected:

	//ポインターだけセットしたコンフィグを、それ以外の設定を付与して設定する関数
	void SetCollider(CollConfig& config);

	bool isActive_ = true;
	float size_ = 0.0f;

};
