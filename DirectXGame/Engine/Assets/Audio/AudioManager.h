#pragma once
#include "Data/AudioData.h"
#include <memory>
#include <wrl.h>
#include <map>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

/**
 * @class AudioManager
 * @brief オーディオシステム全体を管理するマネージャークラス
 * 
 * XAudio2を使用してWAVやMP3などの音声ファイルの読み込み、再生、停止を管理する。
 * 複数の音声データとその再生状態を一元管理し、音量調整機能も提供する。
 */
class AudioManager {
public:

	AudioManager() = default;
	~AudioManager();

	/**
	 * @brief オーディオシステムの初期化
	 * 
	 * XAudio2エンジンとマスタリングボイスを作成し、オーディオシステムを使用可能にする。
	 */
	void Initialize();

	/**
	 * @brief 音声ファイルの読み込み
	 * 
	 * 指定されたファイルパスから音声データ(mp3, wav)を読み込み、指定されたIDで管理する。
	 * 
	 * @param filepath ファイルパス(Assets/Audio/からの相対パス)
	 * @param id 音声データを識別するための一意なID
	 */
	void Load(std::filesystem::path filepath, uint32_t id);

	/**
	 * @brief 音声の再生
	 * 
	 * 指定された音声ハンドルの音声を再生する。
	 * 
	 * @param soundHandle 読み込み済みの音声データのハンドル
	 * @param isLoop ループ再生するかどうか
	 * @return 再生ハンドル。再生終了またはStop呼び出し時に破棄される
	 */
	uint32_t Play(uint32_t soundHandle, bool isLoop);

	/**
	 * @brief 音量の変更
	 * 
	 * @param soundHandle 対象の音声ハンドル
	 * @param volume 音量(初期値1.0f)
	 */
	void SetVolume(uint32_t soundHandle, float volume);

	/**
	 * @brief 再生中かどうかを確認
	 * 
	 * @param playHandle 再生ハンドル
	 * @return 再生中の場合true
	 */
	bool IsPlay(uint32_t playHandle);

	/**
	 * @brief 再生の停止
	 * 
	 * 再生を停止し、再生ハンドルを破棄する。
	 * 再度再生する場合は最初から再生される。
	 * 
	 * @param playHandle 再生ハンドル
	 */
	void Stop(uint32_t playHandle);

	/**
	 * @brief オーディオデータの取得
	 * 
	 * @param id 音声データのID
	 * @return 音声データへのポインタ
	 */
	AudioData* GetAudioData(uint32_t id);

private:

	/// @brief XAudio2のインスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	/// @brief マスタリングボイス(最終的な音声出力を制御)
	IXAudio2MasteringVoice* masterVoice_;

	/// @brief 音声データのマップ(ID -> AudioData)
	std::map<uint32_t, std::unique_ptr<AudioData>> audioData_{};

	/// @brief 音声ファイルのベースパス
	const std::string basePath = "Assets/Audio/";
};

