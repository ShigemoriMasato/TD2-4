#pragma once
#include <Render/RenderObject.h>
#include <assets/Model/ModelManager.h>
#include <Render/Font/Text.h>
#include <Common/KeyConfig/KeyManager.h>
#include <SHEngine.h>

struct TextInitInfo {
	std::wstring key;
	std::string name;
	std::wstring text;
};

class PauseMenu {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool> key);
	void Draw(CmdObj* cmdObj);

	// コールバック登録
	void SetAction(int index, std::function<void()> func) { actions_[index] = func; }

private:
	// テキストの生成、初期化を行うヘルパー関数
	void CreateText(const std::wstring& key, SHEngine::DrawData& drawData, const std::string& font, int fontSize, const std::string& name, const std::wstring& text);

private:
	std::unordered_map<std::wstring, std::unique_ptr<SHEngine::Text>> texts_;
	std::unordered_map<std::wstring, Transform> transforms_;
	std::unordered_map<int, std::function<void()>> actions_;
	std::unique_ptr<SHEngine::RenderObject> backgroundRender_;
	Transform backgroundTransform_;
	Matrix4x4 backgroundWVP_;

	std::vector<TextInitInfo> infos_ = {
	    {L"Resume",  "ResumeText",  L"Resume" },
        {L"ToTitle", "ToTitleText", L"ToTitle"},
        {L"Weapons", "WeaponsText", L"Weapons"}
    };

	// テキストのサイズ
	float normalSize_ = 1.0f;
	float slectedSize_ = 2.0f;

	// 選択中の項目
	int selectedIndex_ = 0;
};
