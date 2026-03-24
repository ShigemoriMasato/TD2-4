#pragma once
#include <Render/RenderObject.h>

class GameFrame {
public:

	void Initialize(SHEngine::DrawData& drawData, int textureIndex);
	void Update();
	void Draw(CmdObj* cmdObj);

private:

	std::unique_ptr<SHEngine::RenderObject> render_;

	Transform transform_;
	int textureIndex_ = 0;

};
