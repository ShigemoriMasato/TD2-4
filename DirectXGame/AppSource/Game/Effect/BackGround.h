#pragma once
#include <Render/RenderObject.h>
#include <Utility/Matrix.h>
#include <Utility/DataStructures.h>

class BackGround {
public:

	struct Wave {
		float strength = 0.0f;
		float speed = 8.0f;
		float tickness = 0.5f;
		float lifeTime = 0.0f;
		//======================
		float lifeSpan = 2.0f;
		Vector2 generatePosition = { 0.0f, 0.0f };
		float pad;
		//======================
		Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
		//======================
		Vector3 direction = {0.0f, 1.0f, 0.0f};
		float pad2;
	};

	BackGround() = default;
	~BackGround();

	void Initialize(DrawData& drawData, Camera* camera);
	void Update(float deltaTime);
	void AddWave(const Wave& waveData);
	void DrawImGui();
	void Draw(Window* window);

	Wave GetWave() const {
		return editWaveData_;
	}

private:

	void Save();
	void Load();

	struct VSData {
		int x;                //ブロックの量
		int y;                //ブロックの量
		int pad[2];
		Matrix4x4 parentMatrix;
		Matrix4x4 vpMatrix;
		Vector4 innerColor;
		Vector4 outerColor;
	};

	Camera* camera_ = nullptr;
	std::unique_ptr<RenderObject> renderObject_;
	std::vector<Wave> waves_;
	VSData vsData_{};

	Transform transform_{};

private://ImGui

	Wave editWaveData_{};

	std::unique_ptr<BinaryManager> binaryManager_{ nullptr };
	std::string saveFile_ = "BackGround.sg";
};
