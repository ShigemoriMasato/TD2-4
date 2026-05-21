#pragma once
#include <Render/Renderer.h>
#include <SHEngine.h>
#include "Weapon.h"

class WeaponRender {
public:

	WeaponRender(SHEngine::Engine* engine);
	~WeaponRender() = default;

	int AddRenderData(const Weapon::RenderData& renderData);
	void DeleteRenderer(int id);

	SHEngine::GPUBuffer* GetDirLightBuffer() const { return dirLightBuffer_; }
	SHEngine::GPUBuffer* GetPointLightBuffer() const { return pointLightBuffer_; }
	SHEngine::GPUBuffer* GetLightNumBuffer() const { return lightNumBuffer_; }

	void DrawImGui();

private:

	int CreateRenderer(Weapon::RenderData renderData);

	std::unique_ptr<SHEngine::BufferContainer> container_;
	SHEngine::GPUBuffer* dirLightBuffer_ = nullptr;
	SHEngine::GPUBuffer* pointLightBuffer_ = nullptr;
	SHEngine::GPUBuffer* lightNumBuffer_ = nullptr;
	struct LightNumData {
		int dirLightNum;
		int pointLightNum;
	};
	struct MaterialData {
		Vector4 color;
		int textureIndex;
	};
	struct VSData {
		Matrix4x4 world;
		Matrix4x4 wvp;
	};

	const int maxNum_ = 32;

	std::map<int, std::unique_ptr<SHEngine::Renderer>> renderers_;
	std::map<int, SHEngine::GPUBuffer*> gpuBuffers_;
	int nextID_ = 0;

private: //Light関係

	std::vector<DirectionalLight> dirLights_;
	std::vector<PointLight> pointLights_;

private: //入出力関係

	BinaryManager binManager_;
	const std::string saveFile_ = "LightConfig.bin";

};
