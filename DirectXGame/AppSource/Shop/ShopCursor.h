#pragma once
#include <Common/KeyConfig/KeyManager.h>

class ShopCursor {
public:

	void Initialize(KeyManager* keyManager);

	void Update(float deltaTime);

private:

	KeyManager* keyManager_ = nullptr;

};
