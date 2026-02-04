#pragma once
#include <Render/RenderObject.h>
#include <UI/Object/FontObject.h>

class TotalManager {
public:

	~TotalManager() { Save(); };

	void Initialize(int maxOre, int getOre, int killOre, int totalGold, int maxGold, const DrawData& drawData, FontLoader* fontLoader);
	void Update(float deltaTime);
	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void DrawImGui();

private:

	void Save();
	void Load();

	std::unique_ptr<RenderObject> bg_ = nullptr;

	std::unique_ptr<FontObject> maxOre_ = nullptr;
	std::unique_ptr<FontObject> getOre_ = nullptr;
	std::unique_ptr<FontObject> killOre_ = nullptr;
	std::unique_ptr<FontObject> totalGold_ = nullptr;
	std::unique_ptr<FontObject> maxGold_ = nullptr;
	std::unique_ptr<FontObject> uiMessage_ = nullptr;

	std::vector<Vector4> colors_;

	float t = 0.0f;
	float rotate_ = 0.2f;

	BinaryManager bManager_;
	const std::string save_ = "TotalData";
};
