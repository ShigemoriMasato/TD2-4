#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Render/DrawDataManager.h>

class StaticObjectRender {
public:

	void Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, bool debugMode = false);
	void Draw(const Matrix4x4& vpMatrix, Window* window);
	void SetAlpha(float alpha);

	void SetObjects(const std::map<int, std::vector<Transform>>& objects);

private:

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
	struct DirectionalLight {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};
	struct PointLight {
		Vector4 color;
		Vector3 position;
		float intensity;
		float radius;
		float decay;
		Vector2 pad;
	};

	std::map<int, std::vector<VSData>> vsData_{};
	std::map<int, Material> materialData_{};
	std::map<int, std::unique_ptr<RenderObject>> objects_{};

	std::map<int, NodeModelData> modelDataList_{};

	bool debugMode_ = false;

	ModelManager* modelManager_ = nullptr;
	DrawDataManager* drawDataManager_ = nullptr;
};
