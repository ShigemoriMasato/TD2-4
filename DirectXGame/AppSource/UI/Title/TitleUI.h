#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Render/DrawDataManager.h>
#include <Utility/Matrix.h>
#include <Utility/Vector.h>
#include <memory>
#include <array>


struct CommonData;
namespace Player { class Base; }

namespace Title {

	/// UIモデルの種類
	enum class Kinds {
		Frame,
		Frame2,
		Frame3,
		Logo,
		Start,
		Option,
		Quit,
		Gauge,
		Gauge2,
		Gauge3,

		Count,
	};

	/// 選択肢の種類
	enum class Select {
		Start,
		Option,
		Quit,

		Count,
	};
}

namespace Option {

	enum class Select {

		Master,
		BGM,
		SE,
		Quit,

		Count,
	};

}

class TitleUI {
public:

	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, CommonData* commonData);
	void Update(const Matrix4x4& vpMatrix, float deltaTime);
	void Draw(CmdObj* cmdObj);

	void UpdateSelection(bool upPressed, bool downPressed);
	Title::Select GetCurrentSelect() const { return currentSelect_; }
	void SetCurrentSelect(Title::Select select) { currentSelect_ = select; }
	void SetPlayerInRange(bool inRange) { playerInRange_ = inRange; }
	void SetPlayer(Player::Base* player) { player_ = player; }
	void StartCompassExitAnimation();

#ifdef USE_IMGUI
	void DrawImGui();
#endif // USE_IMGUI

	void SetPosition(Title::Kinds kind, const Vector3& position) { positions_[static_cast<size_t>(kind)] = position; }
	void SetRotation(Title::Kinds kind, const Vector3& rotation) { rotations_[static_cast<size_t>(kind)] = rotation; }
	void SetScale(Title::Kinds kind, const Vector3& scale) { scales_[static_cast<size_t>(kind)] = scale; }

	void SetFramePosition(const Vector3& position) { SetPosition(Title::Kinds::Frame, position); }
	void SetFrame2Position(const Vector3& position) { SetPosition(Title::Kinds::Frame2, position); }
	void SetFrame3Position(const Vector3& position) { SetPosition(Title::Kinds::Frame3, position); }
	void SetLogoPosition(const Vector3& position) { SetPosition(Title::Kinds::Logo, position); }
	void SetStartPosition(const Vector3& position) { SetPosition(Title::Kinds::Start, position); }
	void SetOptionPosition(const Vector3& position) { SetPosition(Title::Kinds::Option, position); }
	void SetQuitPosition(const Vector3& position) { SetPosition(Title::Kinds::Quit, position); }

	void SetFrameRotation(const Vector3& rotation) { SetRotation(Title::Kinds::Frame, rotation); }
	void SetFrame2Rotation(const Vector3& rotation) { SetRotation(Title::Kinds::Frame2, rotation); }
	void SetFrame3Rotation(const Vector3& rotation) { SetRotation(Title::Kinds::Frame3, rotation); }
	void SetLogoRotation(const Vector3& rotation) { SetRotation(Title::Kinds::Logo, rotation); }
	void SetStartRotation(const Vector3& rotation) { SetRotation(Title::Kinds::Start, rotation); }
	void SetOptionRotation(const Vector3& rotation) { SetRotation(Title::Kinds::Option, rotation); }
	void SetQuitRotation(const Vector3& rotation) { SetRotation(Title::Kinds::Quit, rotation); }

	void SetFrameScale(const Vector3& scale) { SetScale(Title::Kinds::Frame, scale); }
	void SetFrame2Scale(const Vector3& scale) { SetScale(Title::Kinds::Frame2, scale); }
	void SetFrame3Scale(const Vector3& scale) { SetScale(Title::Kinds::Frame3, scale); }
	void SetLogoScale(const Vector3& scale) { SetScale(Title::Kinds::Logo, scale); }
	void SetStartScale(const Vector3& scale) { SetScale(Title::Kinds::Start, scale); }
	void SetOptionScale(const Vector3& scale) { SetScale(Title::Kinds::Option, scale); }
	void SetQuitScale(const Vector3& scale) { SetScale(Title::Kinds::Quit, scale); }

	Vector3& GetPosition(Title::Kinds kind) { return positions_[static_cast<size_t>(kind)]; }
	Vector3& GetRotation(Title::Kinds kind) { return rotations_[static_cast<size_t>(kind)]; }
	Vector3& GetScale(Title::Kinds kind) { return scales_[static_cast<size_t>(kind)]; }

private:

	static constexpr size_t kUICount = static_cast<size_t>(Title::Kinds::Count);
	
	std::array<std::unique_ptr<SHEngine::RenderObject>, kUICount> renders_;
	std::array<Vector3, kUICount> positions_{
		Vector3{ 8.0f, 0.0f, 0.0f },     // Frame
		Vector3{ -200.0f, 0.0f, -10.0f },   // Frame2
		Vector3{ 22.0f, 0.0f, 0.0f },   // Frame3
		Vector3{ 0.0f, -4.7f, 1.75f },   // Logo
		Vector3{ 8.0f, 0.2f, 0.0f },     // Start
		Vector3{ -200.0f, 0.2f, -10.0f },   // Option
		Vector3{ 22.0f, 0.2f, 0.0f },   // Quit
		Vector3{ -4.8f + 8.0f, 0.15f, 3.5f },   // Gauge
		Vector3{ -204.8f, 0.15f, -6.5f },  // Gauge2
		Vector3{ -4.8f + 22.0f, 0.15f, 3.5f }  // Gauge3
	};
	std::array<Vector3, kUICount> rotations_{
		Vector3{ 0.0f, 0.0f, 0.0f },    // Frame (90度 = π/2)
		Vector3{ 0.0f, 0.0f, 0.0f },    // Frame2
		Vector3{ 0.0f, 0.0f, 0.0f },    // Frame3
		Vector3{ 0.55f, 0.0f, 0.0f },   // Logo
		Vector3{ -0.55f, 0.0f, 0.0f },   // Start
		Vector3{ -0.55f, 0.0f, 0.0f },  // Option
		Vector3{ -0.55f, 0.0f, 0.0f },  // Quit
		Vector3{ 0.0f, 0.0f, 0.0f },    // Gauge
		Vector3{ 0.0f, 0.0f, 0.0f },    // Gauge2
		Vector3{ 0.0f, 0.0f, 0.0f }     // Gauge3
	};
	std::array<Vector3, kUICount> scales_{
		Vector3{ 2.0f, 2.0f, 2.0f },    // Frame
		Vector3{ 2.0f, 2.0f, 2.0f },    // Frame2
		Vector3{ 2.0f, 2.0f, 2.0f },    // Frame3
		Vector3{ 2.0f, 2.0f, 0.5f },    // Logo
		Vector3{ 1.25f, 1.0f, 1.25f },  // Start
		Vector3{ 1.25f, 1.0f, 1.25f },  // Option
		Vector3{ 1.25f, 1.0f, 1.25f },  // Quit
		Vector3{ 1.0f, 0.25f, 0.25f },  // Gauge
		Vector3{ 1.0f, 0.25f, 0.25f },  // Gauge2
		Vector3{ 1.0f, 0.25f, 0.25f }   // Gauge3
	};
	std::array<int, kUICount> modelIDs_{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	Title::Select currentSelect_ = Title::Select::Start;
	bool playerInRange_ = false;

	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;
	CommonData* commonData_ = nullptr;
	Player::Base* player_ = nullptr;

	// Compass
	std::unique_ptr<SHEngine::RenderObject> compassRender_;
	int compassModelID_ = -1;
	Vector3 compassOffset_{ 0.0f, 5.0f, 0.0f }; // Playerからの頭上オフセット
	Vector3 compassRotation_{ -0.9f, 0.0f, 0.0f };
	Vector3 compassScale_{ 0.75f, 1.0f, 0.75f };
	float compassAnimTimer_ = 0.0f;
	float compassAnimSpeed_ = 1.0f; // 1周にかかる秒数の逆数（大きいほど速い）

	// Compass退場アニメーション
	bool compassExiting_ = false;
	float compassExitTimer_ = 0.0f;
	float compassExitDuration_ = 1.f; // 退場にかかる秒数
	Vector3 compassExitStartScale_{ 0.75f, 1.0f, 0.75f };

	// ThumbsUp
	std::unique_ptr<SHEngine::RenderObject> thumbsUpRender_;
	int thumbsUpModelID_ = -1;
	Vector3 thumbsUpOffset_{ 0.0f, 6.0f, 0.0f }; // Playerからの頭上オフセット
	Vector3 thumbsUpRotation_{ -0.9f, 0.0f, 0.9f }; // 初期回転（z=0.9）
	Vector3 thumbsUpScale_{ 0.0f, 0.0f, 0.0f };   // 初期スケール（非表示）

	// ThumbsUp登場アニメーション
	bool thumbsUpEntering_ = false;
	float thumbsUpEnterTimer_ = 0.0f;
	float thumbsUpEnterDuration_ = 0.5f; // 登場にかかる秒数

	// ThumbsUp退場アニメーション
	bool thumbsUpExiting_ = false;
	float thumbsUpExitTimer_ = 0.0f;
	float thumbsUpExitDuration_ = 0.4f;  // 退場にかかる秒数
	float thumbsUpWaitDuration_ = 0.8f;  // 登場完了後の待機時間
	float thumbsUpWaitTimer_ = -1.0f;     // 待機タイマー（-1=未開始）

	bool thumbsUpEnd_ = false;

	// Gaugeアニメーション
	std::array<float, 3> gaugeTimers_{ 0.0f, 0.0f, 0.0f };
	float kGaugeAnimDuration_ = 3.0f;
	float kGaugeAnimBackDuration_ = 1.0f;
	float kGaugeMaxScale_ = 4.8f;
	float kNowGaugeScale_ = 0.0f;
};