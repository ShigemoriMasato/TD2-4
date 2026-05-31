#pragma once
#include <Render/Renderer.h>

struct LightData {
	SHEngine::GPUBuffer* directional;
	SHEngine::GPUBuffer* point;
	SHEngine::GPUBuffer* config;
};

class LightManager {
public:

	~LightManager();

	void Initialize();
	void DrawImGui();

	const LightData& GetLightData() const { return lightData_; }

private:

	void Save();
	void Load();

	std::unique_ptr<SHEngine::BufferContainer> container_;

	LightData lightData_;

	std::vector<DirectionalLight> directionalLights_;
	std::vector<PointLight> pointLights_;

	struct Config {
		int directionalNum = 1;
		int pointNum = 1;
	} config_;

	int currentDirNum_ = 0;
	int currentPointNum_ = 0;

};
