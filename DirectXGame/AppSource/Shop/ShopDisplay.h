#pragma once
#include <Render/Screen/MultiDisplay.h>
#include <Render/RenderObject.h>

class ShopDisplay {
public:

	void Initialize(CmdObj* cmdObj, SHEngine::DrawData& drawData, SHEngine::TextureManager* textureManager);
	void SetTransform(const Vector2& leftTop, const Vector2& size);
	void Update();
	void PreDraw();
	void PostDraw();
	void Draw();

	SHEngine::Screen::IDisplay* GetDisplay() const { return disp_.get(); }

private:

	CmdObj* cmdObj_;

	std::unique_ptr<SHEngine::Screen::MultiDisplay> disp_;
	std::unique_ptr<SHEngine::RenderObject> render_;

	Transform transform_;

};
