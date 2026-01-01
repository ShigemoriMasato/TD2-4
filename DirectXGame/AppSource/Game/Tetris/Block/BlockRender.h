#pragma once
#include <Camera/Camera.h>
#include <Game/Tetris/Data.h>
#include <Render/RenderObject.h>

class BlockRender {
public:

	BlockRender() = default;
	~BlockRender();

	void Initialize(std::map<int, std::pair<uint32_t, uint32_t>> colorMap, uint32_t fieldWidth, uint32_t fieldHeight, Camera* camera, const DrawData& drawData);

	void Update(float deltaTime);

	void SetStageData(std::vector<std::vector<int>> fieldData, const MovableMino& mino);
	//4*1まで
	void SetHoldMino(std::vector<std::pair<int, int>> blockPos, int colorID);
	//4*5まで
	void SetNextMino(std::vector<std::pair<int, int>> blockPos, int colorID);

	void Draw(Window* window);
	void DrawImGui();

	bool GetIsEffecting() const { return false; }

private:

	Logger logger_;

	std::map<int, std::pair<uint32_t, uint32_t>> colorMap_{};
	Camera* camera_ = nullptr;

	//Field->Wall->Hold->Next
	std::unique_ptr<RenderObject> blockObject_ = nullptr;
	//todo いつかどっかに作る
	struct Transform {
		Vector3 scale = { 1.0f, 1.0f, 1.0f };
		Vector3 rotate{};
		Vector3 position{};
	};
	std::vector<Transform> blockTransforms_{};
	std::vector<std::pair<uint32_t, uint32_t>> blockColors_{};


	struct VSData {
		Matrix4x4 worldMatrix = Matrix4x4::Identity();
		Matrix4x4 vpMatrix = Matrix4x4::Identity();
		uint32_t color = 0;
		uint32_t outlineColor = 0;
		uint64_t padding = 0;
	};
	int vertexDataIndex_ = -1;
	std::vector<VSData> vertexDatas_{};

	//表示するフィールドサイズ
	uint32_t fieldWidth_ = 0;
	uint32_t fieldHeight_ = 0;

private://Binary保存

	void Save();
	void Load();

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	const std::string fileName_ = "TetrisBlockRenderData.sg";
	
	Vector3 holdBasePosition_ = Vector3(-11.0f, 6.0f, 0.0f);
	Vector3 nextBasePosition_ = Vector3(11.0f, 6.0f, 0.0f);
	Vector3 nextGap_ = Vector3(0.0f, -4.0f, 0.0f);

private://ImGui
	int colorMapEditID_ = 0;
};
