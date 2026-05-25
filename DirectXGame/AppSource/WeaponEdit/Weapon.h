#pragma once
#include <Tool/Binary/BinaryManager.h>
#include <Assets/Model/ModelManager.h>
#include <cfloat>

class Weapon {
public:

	//特殊なキー。攻撃地点とプレイヤーの位置は動的に変わるため、固定の値を入れておく
	struct UniqueKey {
		Vector4 playerAnchor = { FLT_MAX, 0.0f, 0.0f, 0.0f };	//この値が入っていた場合、Playerの情報を代入する
		Vector4 focusAnchor = { 0.0f, FLT_MAX, 0.0f, 0.0f };	//この値が入っていた場合、攻撃地点の情報を代入する
	};

	struct Key {
		float time;
		std::vector<Transform> data;
	};

	struct Data {
		std::string modelFilePath;
		std::vector<Transform> regularKey;	//通常時の位置
		std::vector<Key> attackAnimation;	//攻撃時のアニメーション

		void Load(const std::string& filePath);
		void Save(const std::string& filePath) const;
	};

	struct RenderData {
		Matrix4x4 world;
		NodeModelData modelData;
		SHEngine::DrawData drawData;
	};

	static void SetModelManager(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
		modelManager_ = modelManager, drawDataManager_ = drawDataManager;
	}

	Weapon();
	void Update(float deltaTime);
	void Boot() { timer_ = 0.0f; floatTimer_ = 0.0f; };

	static void SetPlayerMatrix(const Matrix4x4& playerMatrix) { playerMatrix_ = playerMatrix; }

	//攻撃する時に狙う位置をセットする
	void SetAimMatrix(const Matrix4x4& aimMatrix) { aimMatrix_ = aimMatrix; }
	void SetTimer(float timer) { timer_ = timer; }
	void SetWeaponData(const std::string& dataFilePath);
	void SetWeaponData(const Data& data) { data_ = data; DataSetting(); }

	RenderData GetRenderData() const { return renderData_; }

private:

	void DataSetting();

	float timer_ = 10000000.0f;
	float floatTimer_ = 0.0f;
	float animationTime_ = 0.0f;
	Data data_;
	RenderData renderData_;

	Matrix4x4 aimMatrix_;

	static inline Matrix4x4 playerMatrix_;
	static inline BinaryManager binManager_;
	static inline SHEngine::ModelManager* modelManager_ = nullptr;
	static inline SHEngine::DrawDataManager* drawDataManager_ = nullptr;

};
