#pragma once
#include <Input/Input.h>
#include <unordered_map>
#include <Tool/Binary/BinaryManager.h>
#include <Common/MainDisplay.h>

enum class Key {
	// 移動系 -===========

	Right,
	Left,
	Up,
	Down,
	
	Correct,//確定キー
	Reverse,//取消キー
	Pause,//ポーズキー

	// Debug -===========

	Restart,//リスタートキー
	Debug1,
	Debug2,
	Debug3,

	Count
};

enum class KeyState {
	None,
	Trigger,
	Hold,
	Release,

	Count
};

//キーコンフィグの設定画面の作成は考慮してないです。よって削除はできません.
class KeyManager {
public:

	KeyManager() = default;
	~KeyManager() = default;

	void Initialize(SHEngine::Input* input, MainDisplay* display);
	void Update();

	std::unordered_map<Key, bool> GetKeyStates() const { return resultKeyFlugs_; }
	Vector2 GetCursorPos() const;

	void SetKey(Key action, uint8_t DIK, KeyState state = KeyState::Release);
	void SetButton(Key action, XBoxController button, KeyState state);
	void SetStick(Key action, bool isLeft, bool isY, float toggleValue);

private:

	MainDisplay* display_ = nullptr;
	SHEngine::Input* input_ = nullptr;

	//スティックの方向
	enum Direction {
		Right,
		Left
	};

	//Keyに割り当てられているDIKと状態
	std::unordered_map<Key, std::vector<std::pair<uint8_t, KeyState>>> keyMap_;
	std::unordered_map<Key, std::vector<std::pair<XBoxController, KeyState>>> buttonMap_;
	std::unordered_map<Key, std::pair<Direction, std::pair<bool, float>>> stickMap_;

	//キー入力の履歴を保存する。
	std::vector<std::unordered_map<uint8_t, bool>> keyHistory_;
	std::vector<std::unordered_map<XBoxController, bool>> buttonHistory_;
	std::vector<std::unordered_map<Direction, Vector2>> stickHistory_;
	//履歴の最大数
	static const int kMaxHistory_ = 10;

	//最終的な状態を格納する
	std::unordered_map<Key, bool> resultKeyFlugs_;
};
