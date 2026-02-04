#pragma once
#include <UI/Number.h>

class Card {
public:

	~Card() { Save(); };

	void Initialize(const std::string& fontName, const DrawData& drawData, FontLoader* fontLoader, TextureManager* textureManager, int floorNum, bool isFalse);
	void SetNumber(int current, int norma) { number_[0]->SetNumber(current), number_[1]->SetNumber(norma); }
	bool Update(float deltaTime, const Matrix4x4& parentMat);
	void Draw(Window* window, const Matrix4x4& vpMatrix);
	void DrawImGui();

	void SetTransData(const Vector3& position, float scale);

private:

	void Load();
	void Save();

	std::vector<std::unique_ptr<Number>> number_;
	std::unique_ptr<FontObject> slash_ = nullptr;
	std::unique_ptr<FontObject> floorNum_ = nullptr;
	std::unique_ptr<RenderObject> cardBack_ = nullptr;
	int cbTextureIndex_ = -1;
	Transform cbTransform_{};
	std::unique_ptr<RenderObject> stamp_ = nullptr;
	int spTextureIndex_ = -1;
	Transform spTransform_{};

	Transform transform_{};
	Transform targetTransform_{};
	Matrix4x4 parentMatrix_{};

	float timer_ = 0.0f;
	bool isFalse_ = false;

	BinaryManager bManager_;
};
