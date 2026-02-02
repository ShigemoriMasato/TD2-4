#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Render/DrawDataManager.h>
#include <Game/NewMap/NewMap.h>

using float2 = Vector2;
using float3 = Vector3;
using float4x4 = Matrix4x4;

class StaticObjectRender {
public:

	~StaticObjectRender() {
		SaveWaveData();
	};

	void Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, bool debugMode = false);
	void Draw(const Matrix4x4& vpMatrix, Window* window);
	void DrawImGui();
	void SetAlpha(float alpha);

	void SetCurrentStage(int stage) { currentStage_ = stage; }

	void SetObjects(const std::map<int, std::vector<Transform>>& objects);

	void SetWallIndex(int index) { wallIndex_ = index; }

private:

	void Load();
	void Save();
	void SaveWaveData();

	struct VSData {
		Matrix4x4 world;
		Matrix4x4 wvp;
	};
	struct Material {
		Vector4 color;
		int textureIndex;
	};

	struct LightConfig {
		int directionalLightNum = 0;
		int pointLightNum = 0;
		int padding[2]{};
	};

	std::map<int, std::vector<VSData>> vsData_{};
	std::map<int, Material> materialData_{};
	std::map<int, std::unique_ptr<RenderObject>> objects_{};

	std::map<int, NodeModelData> modelDataList_{};

	bool debugMode_ = false;

	ModelManager* modelManager_ = nullptr;
	DrawDataManager* drawDataManager_ = nullptr;

	BinaryManager binaryManager_;
	const std::string saveFileName_ = "DirectionalLight";
	const std::string waveFileName_ = "WaveData";

	int wallIndex_ = 0;

	struct Wave {
		float2 direction;
		float amplitude;
		float wavelength;
		float speed;
		float3 pad;
	};

	struct WaveVS {
		float4x4 world;
		float4x4 vp;
		float3 cameraPos;
		float time;
		int waveCount;
		float3 pad;
		Wave waves[8];
	};
	std::vector<WaveVS> waveData_{};
	int currentStage_;
};
