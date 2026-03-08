#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Render/DrawDataManager.h>
#include <Utility/Matrix.h>
#include <Utility/Vector.h>
#include <memory>
#include <array>

namespace Title {

	/// UIモデルの種類
	enum class Kinds {
		Logo,
		Start,
		Option,
		Quit,

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

class TitleUI {
public:

	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager);
	void Update(const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void UpdateSelection(bool upPressed, bool downPressed);
	Title::Select GetCurrentSelect() const { return currentSelect_; }

#ifdef USE_IMGUI
	void DrawImGui();
#endif // USE_IMGUI

	void SetPosition(Title::Kinds kind, const Vector3& position) { positions_[static_cast<size_t>(kind)] = position; }
	void SetScale(Title::Kinds kind, const Vector3& scale) { scales_[static_cast<size_t>(kind)] = scale; }

	void SetLogoPosition(const Vector3& position) { SetPosition(Title::Kinds::Logo, position); }
	void SetStartPosition(const Vector3& position) { SetPosition(Title::Kinds::Start, position); }
	void SetOptionPosition(const Vector3& position) { SetPosition(Title::Kinds::Option, position); }
	void SetQuitPosition(const Vector3& position) { SetPosition(Title::Kinds::Quit, position); }

	void SetLogoScale(const Vector3& scale) { SetScale(Title::Kinds::Logo, scale); }
	void SetStartScale(const Vector3& scale) { SetScale(Title::Kinds::Start, scale); }
	void SetOptionScale(const Vector3& scale) { SetScale(Title::Kinds::Option, scale); }
	void SetQuitScale(const Vector3& scale) { SetScale(Title::Kinds::Quit, scale); }

	Vector3& GetPosition(Title::Kinds kind) { return positions_[static_cast<size_t>(kind)]; }
	Vector3& GetScale(Title::Kinds kind) { return scales_[static_cast<size_t>(kind)]; }

private:

	static constexpr size_t kUICount = static_cast<size_t>(Title::Kinds::Count);
	
	std::array<std::unique_ptr<SHEngine::RenderObject>, kUICount> renders_;
	std::array<Vector3, kUICount> positions_{
		Vector3{ 0.0f, 1.5f, 10.0f },   // Logo
		Vector3{ 0.0f, 0.0f, 10.0f },   // Start
		Vector3{ 0.0f, -0.75f, 10.0f },  // Option
		Vector3{ 0.0f, -1.5f, 10.0f }   // Quit
	};
	std::array<Vector3, kUICount> scales_{
		Vector3{ 2.0f, 2.0f, 0.5f },   // Logo
		Vector3{ 1.0f, 1.0f, 0.5f },   // Start
		Vector3{ 1.0f, 1.0f, 0.5f },   // Option
		Vector3{ 1.0f, 1.0f, 0.5f }    // Quit
	};
	std::array<int, kUICount> modelIDs_{ -1, -1, -1, -1 };

	Title::Select currentSelect_ = Title::Select::Start;

	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;
};