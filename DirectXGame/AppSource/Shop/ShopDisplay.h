#pragma once
#include <Render/Screen/MultiDisplay.h>
#include <Render/RenderObject.h>

class ShopDisplay {
public:

	void Initialize(CmdObj* cmdObj, SHEngine::DrawData& drawData, SHEngine::TextureManager* textureManager);
	void Update();
	void PreDraw();
	void PostDraw();
	void Draw();

private:

	CmdObj* cmdObj_;

	std::unique_ptr<SHEngine::Screen::MultiDisplay> disp_;
	std::unique_ptr<SHEngine::RenderObject> render_;

};
