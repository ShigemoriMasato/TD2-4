#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <Render/Font/Text.h>
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

// 背景の種類
enum class BGType {
	FullBG,           // 全体の背景
	AllWeaponsBG,     // 武器一覧の背景
	SelectWeaponBG,   // 選択中武器の背景
	WeaponName,       // 武器項目の背景
	SelectWeaponName, // 選択中武器名の背景

	Count,
};

/// <summary>
/// 武器図鑑
/// </summary>
class WeaponList {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, KeyManager* keyManager, SHEngine::Input* input);
	void Update(Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool> key);
	void Draw(CmdObj* cmdObj);

private:
	// 武器テキストの追加
	std::unique_ptr<SHEngine::Text> AddText(const std::wstring& textName, SHEngine::DrawData& data, const std::string& fontPath, int fontSize, const std::string& debugName);

private:
	// 全体の背景、武器一覧の背景、武器項目の背景、選択中武器名の背景
	std::unique_ptr<SHEngine::RenderObject> bgRenders_;

	// 武器名テキストマップ
	std::vector<std::unique_ptr<SHEngine::Text>> weaponTexts_;

	// 武器一覧テキスト
	std::unique_ptr<SHEngine::Text> allWeaponText_;

	// 選択中の武器テキスト
	std::unique_ptr<SHEngine::Text> selectWeaponText_;

	// 背景スプライトの数
	static const int kBGSpriteCount = static_cast<int>(BGType::Count);

	// 武器の数
	static const int kWeaponCount = 10;

	// テクスチャインデックス
	int textureIndex_ = 0;

	// DirectionalLight
	DirectionalLight dirLight_{};

	// 背景のTransform配列
	std::vector<Transform> bgTransforms_;

	// 武器のTransform配列
	std::vector<Transform> weaponTransforms_;

	// 武器一覧テキストTransform
	Transform allWeaponsTransform_;

	// 選択中の武器テキストTransform
	Transform selectWeaponTransform_;

	// 武器名配列
	std::array<std::wstring, kWeaponCount> weaponNames_ = {L"Sword", L"Pistol", L"Spear", L"Shotgun", L"Axe", L"Bow", L"Fist", L"Gurepon", L"Pickaxe", L"Shuriken"};

	// スプライト名配列
	std::array<std::string, kBGSpriteCount> bgSpriteNames_ = {"FullBG", "AllWeaponsBG", "SelectWeaponBG", "WeaponName", "SelectWeaponName"};

	// 背景スプライトの色
	std::vector<Vector4> bgColors_;

	// 武器名テキストの開始位置
	Vector3 weaponTextStartPos_{};

	// 武器名テキストの間隔
	float textMarginY_ = 60.0f;

	// 選択中の武器のインデックス
	int selectedWeaponId_ = -1;

	// テキストの当たり判定
	Vector2 hitBoxSize_ = {300.0f, 50.0f};

	// キーマネージャー
	KeyManager* keyManager_ = nullptr;

	// スクロール関連
	float scrollOffset_ = 0.0f;    // 現在のスクロール移動量
	float maxScrollOffset_ = 0.0f; // 最大スクロール量

	// 設定値
	static const int kVisibleItemCount = 6; // 一度に枠内に表示する武器数
	const float kItemHeight = 72.0f;        // 武器名テキスト間のY距離

	SHEngine::Input* input_ = nullptr;
};
