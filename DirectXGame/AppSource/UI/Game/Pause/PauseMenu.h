#pragma once
#include "GameObject/EasingAnimation/AnimationBundle.h"
#include <Common/KeyConfig/KeyManager.h>
#include <Render/Font/Text.h>
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

struct TextInitInfo {
	std::wstring key;
	std::string name;
	std::wstring text;
};

class PauseMenu {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, SHEngine::Input* input, KeyManager* keyManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool> key);
	void Draw(CmdObj* cmdObj);

	// コールバック登録
	void SetAction(int index, std::function<void()> func) { actions_[index] = func; }

private:
	// テキストの生成、初期化を行うヘルパー関数
	void CreateText(const std::wstring& key, SHEngine::DrawData& drawData, const std::string& font, int fontSize, const std::string& name, const std::wstring& text);

private:
	// Resume,ToTitle,Weapons
	std::unordered_map<std::wstring, std::unique_ptr<SHEngine::Text>> texts_;
	std::unordered_map<std::wstring, Transform> transforms_;
	std::unordered_map<int, std::function<void()>> actions_;

	// 背景
	std::unique_ptr<SHEngine::RenderObject> backgroundRender_;
	Transform backgroundTransform_;
	Matrix4x4 backgroundWVP_;

	// メニュー画面表記
	std::unique_ptr<SHEngine::Text> menuText_;
	Transform menuTextTransform_;

	std::vector<TextInitInfo> infos_ = {
	    {L"Resume",  "ResumeText",  L"再開"      },
        {L"ToTitle", "ToTitleText", L"タイトルへ"},
        {L"Weapons", "WeaponsText", L"武器図鑑"  }
    };

	// テキストのサイズ
	float normalSize_ = 1.0f;
	float slectedSize_ = 2.0f;

	// 選択中の項目
	int selectedIndex_ = 0;

	SHEngine::Input* input_ = nullptr;
	KeyManager* keyManager_ = nullptr;

	// テキストの当たり判定サイズ
	Vector2 hitBoxSize_ = {400.0f, 60.0f};

	// 前フレームのマウス位置
	Vector2 lastMousePos_ = {0.0f, 0.0f};

	// 選択時の色
	Vector4 selectColor_ = {1.0f, 0.4f, 0.0f, 1.0f};

	// 選択中項目の背景
	std::unique_ptr<SHEngine::RenderObject> itemBgRender_;
	Transform itemBgTransform_;
	Matrix4x4 itemBgWVP_;
	Vector4 itemBgColor_ = {1.0f, 0.4f, 0.0f, 0.5f};
	Vector2 itemBgSize_ = {700.0f, 60.0f};

	// アニメーション関連
	AnimationBundle<Vector3> scaleAnim_;
	int previousSelectedIndex_ = -1; // 選択切り替えを検知するための変数
};
