#pragma once
#include <Render/RenderObject.h>

class StaticObjectRender {
public:

	void Initialize();
	void Draw(const Matrix4x4 vpMatrix);

private:

	std::map<int, std::unique_ptr<RenderObject>> objects_{};

};
