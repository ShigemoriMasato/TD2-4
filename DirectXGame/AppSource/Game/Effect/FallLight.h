#pragma once
#include <Render/RenderObject.h>
#include <Assets/Texture/TextureManager.h>
#include <Tool/Binary/BinaryManager.h>
#include <random>

class FallLight {
public:

	FallLight() = default;
	~FallLight();

	void Initialize(const DrawData& drawData, TextureManager* textureManager, Camera* camera);
	void Update(float deltaTime);
	void Draw(Window* window);
	void DrawImGui();

private:

	void Save();
	void Load();

	struct VSData {
		Matrix4x4 vp;

		Vector3 dir = { 1.0f, 0.0f, 0.0f };
		float timer = 0.0f;

		Vector3 color = { 1.0f, 1.0f, 1.0f };
		float lifeSpan = 0.1f;

		Vector3 firstPosition;
		float Speed = 1.0f;
	};
	Camera* camera_ = nullptr;
	int textureIndex_ = -1;
	const uint32_t maxInstance_ = 10000;
	uint32_t activeNum_ = 0;
	std::vector<VSData> instanceData_{};
	std::unique_ptr<RenderObject> renderObject_{};

	std::string saveFile_ = "FallLight.sg";
	std::string textureFile_ = "circle.png";
	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	char texturePathBuffer_[256];

private://ParticleManagement

	float timer_ = 0.0f;
	float castTime_ = 0.1f;
	float lifeSpan_ = 2.0f;
	float speed_ = 0.0f;
	float minColorParam_ = 0.3f;
	std::vector<bool> isActive_;
	uint32_t searchIndex_ = 0;

	std::mt19937 rand_;
};
