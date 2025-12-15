#pragma once
#include "CommonData.h"
#include <memory>

class IScene {
public:

	IScene(CommonData* commonData);
	virtual ~IScene() = default;

	virtual void Initialize() = 0;
	virtual std::unique_ptr<IScene> Update() = 0;
	virtual void Draw() = 0;

private:

	CommonData* commonData_;

};
