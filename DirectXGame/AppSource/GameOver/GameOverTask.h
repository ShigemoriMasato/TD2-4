#pragma once
#include <Render/RenderObject.h>
#include <Utility/DataStructures.h>

class GameOverTask {
public:

	~GameOverTask() { Save(); };

	void Initialize(const DrawData& drawData, int textureIndex, float* fade);
	bool Update(float deltaTime);
	void Draw(Window* window, const Matrix4x4& vpMat);

	void DrawImGui();

private:

	void Save();
	void Load();

	struct VSData {
		Matrix4x4 worldMatrix;
		Matrix4x4 vpMatrix;
		Matrix4x4 uvMatrix;
		Vector3 scale;
		float progress;
	};

	std::unique_ptr<RenderObject> render_ = nullptr;
	std::unique_ptr<RenderObject> backGround_ = nullptr;
	std::vector<VSData> vsData_;
	std::vector<Transform> transforms_;
	std::vector<Transform> uvTransforms_;
	
	float timer_ = 0.0f;
	float duration_ = 4.5f;
	float* fade_ = nullptr;

	int textureIndex_ = 0;

	const int tieNum_ = 4;

	BinaryManager bManager_;
	const std::string name_ = "GameOverTask";

	bool debug_ = true;
	float alpha_ = 0.0f;
};

