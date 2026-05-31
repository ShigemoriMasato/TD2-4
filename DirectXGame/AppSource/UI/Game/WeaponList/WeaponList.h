#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <Render/Font/Text.h>
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

class WeaponDatabase;

// 背景の種類
enum class BGType {
	FullBG,           // 全体の背景
	AllWeaponsBG,     // 武器一覧の背景
	SelectWeaponBG,   // 選択中武器の背景
	WeaponName,       // 武器項目の背景
	SelectWeaponName, // 選択中武器名の背景
	CloseButton,      // 閉じるボタン
	ScrollbarTrack,   // スクロールバーの背景
	ScrollbarKnob,    // スクロールバーのつまみ

	Count,
};

/// <summary>
/// 武器図鑑
/// </summary>
class WeaponList {
public:
	void Initialize(
	    SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, KeyManager* keyManager, SHEngine::Input* input,
	    WeaponDatabase* weaponDatabase);
	void Update(Matrix4x4 orthoVpMatrix, Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool> key);
	void Draw(CmdObj* cmdObj);

	void SetCloseAction(std::function<void()> action) { closeAction_ = action; }

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
	std::vector<int> textureIndexArray_;

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
	std::array<std::string, kBGSpriteCount> bgSpriteNames_ = {"FullBG", "AllWeaponsBG", "SelectWeaponBG", "WeaponName", "SelectWeaponName", "CloseButton", "ScrollbarTrack", "ScrollbarKnob"};

	// 背景スプライトの色
	std::vector<Vector4> bgColors_;

	// 武器名テキストの開始位置
	Vector3 weaponTextStartPos_{};

	// 武器名テキストの間隔
	float textMarginY_ = 60.0f;

	// 選択中の武器のインデックス
	int selectedWeaponId_ = 0;

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

	// 武器モデルの描画オブジェクト配列
	std::vector<std::unique_ptr<SHEngine::RenderObject>> weaponModels_;

	// 武器モデルのテクスチャインデックス配列
	std::vector<int> weaponTextureIndices_;

	// 画面右側に表示するモデル用のTransform
	Transform weaponModelTransform_ = {
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

	// モデルをゆっくり回転させるための角度
	float modelRotationY_ = 0.0f;

	// 閉じるアクション用のコールバック
	std::function<void()> closeAction_;

	SHEngine::TextureManager* textureManager_ = nullptr;

	// 武器項目背後の矩形用
	std::unique_ptr<SHEngine::RenderObject> weaponBGRenders_;
	std::vector<Transform> weaponBGTransforms_;
	std::vector<Vector4> weaponBGColors_;

	// ホバー時と通常時の色
	const Vector4 kNormalBGColor = {0.1f, 0.1f, 0.1f, 0.3f}; // 通常時
	const Vector4 kHoverBGColor = {0.5f, 0.5f, 0.5f, 0.6f};  // ホバー時

	Vector2 textBGMargin_ = { 140.0f, 8.0f };

	// 選択中武器のステータス表示用テキスト
	std::vector<std::unique_ptr<SHEngine::Text>> selectDamageTexts_;
	std::vector<std::unique_ptr<SHEngine::Text>> selectSpeedTexts_;

	// ステータステキスト用のTransform
	Transform selectDamageTransform_;
	Transform selectSpeedTransform_;

	// 選択中武器名からのY座標オフセット
	float dmgOffsetY_ = -370.0f;
	float spdOffsetY_ = -420.0f;

	bool isDraggingScrollbar_ = false;   // スクロールバーをドラッグ中かどうか
	float dragStartY_ = 0.0f;            // ドラッグ開始時のマウスY座標
	float dragStartScrollOffset_ = 0.0f; // ドラッグ開始時のスクロール量

	bool playingCloseSE_ = false;
	bool playingTextSE_ = false;
};
