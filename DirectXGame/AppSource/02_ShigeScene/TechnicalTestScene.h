#pragma once
#include <Scene/iscene.h>
#include <Render/Font/Text.h>
#include <Camera/DebugCamera.h>

class TechnicalTestScene : public IScene {
public:

	virtual void Initialize() override;
	virtual std::unique_ptr<IScene> Update() override;
	virtual void Draw() override;

private:

	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	std::unique_ptr<SHEngine::Text> text_ = nullptr;
	Transform textTransform_ = {};

};
