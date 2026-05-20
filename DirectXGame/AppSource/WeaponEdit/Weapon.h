#pragma once
#include <Tool/Binary/BinaryManager.h>
#include <Assets/Model/ModelManager.h>

class Weapon {
public:

	//特殊なキー。攻撃地点とプレイヤーの位置は動的に変わるため、固定の値を入れておく
	struct UniqueKey {
		static inline const Matrix4x4 player = {
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		};
		static inline const Matrix4x4 aim = {
			1, 1, 1, 1,
			1, 1, 1, 1,
			1, 1, 1, 1,
			1, 1, 1, 1
		};
	};

	struct Key {
		float time;
		std::vector<Matrix4x4> matrices;
	};

	struct Data {
		std::string modelFilePath;
		Matrix4x4 regularMatrix;
		std::vector<Key> attackAnimation;
	};

	struct RenderData {
		Matrix4x4 world;
		NodeModelData modelData;
		SHEngine::DrawData drawData;
	};

	static void SetModelManager(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
		modelManager_ = modelManager, drawDataManager_ = drawDataManager;
	}

	Weapon(std::string dataFilePath);
	void Update(float deltaTime);
	void Boot() { timer_ = 0.0f; };

	static void SetPlayerMatrix(const Matrix4x4& playerMatrix) { playerMatrix_ = playerMatrix; }

	//攻撃する時に狙う位置をセットする
	void SetAimMatrix(const Matrix4x4& aimMatrix) { aimMatrix_ = aimMatrix; }
	void SetTimer(float timer) { timer_ = timer; }
	void SetWeaponData(const Data& data) { data_ = data; }

	RenderData GetRenderData(RenderData& renderData) const { renderData = renderData_; }

private:

	void DataSetting();

	float timer_ = 0.0f;
	float animationTime_ = 0.0f;
	Data data_;
	RenderData renderData_;

	Matrix4x4 aimMatrix_;

	static inline Matrix4x4 playerMatrix_;
	static inline BinaryManager binManager_;
	static inline SHEngine::ModelManager* modelManager_ = nullptr;
	static inline SHEngine::DrawDataManager* drawDataManager_ = nullptr;

};
