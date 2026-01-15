#pragma once
#include "AudioData.h"

/**
 * @class WAVData
 * @brief WAV形式の音声データを扱うクラス
 * 
 * AudioDataクラスを継承し、WAVファイルの読み込みと再生機能を実装する。
 * RIFFフォーマットに従ってWAVファイルを解析し、XAudio2で再生可能な形式に変換する。
 */
class WAVData : public AudioData {
public:

	WAVData() = default;
	~WAVData() = default;

	/**
	 * @brief WAVファイルの読み込み
	 * 
	 * 指定されたパスからWAVファイルを読み込み、音声データとフォーマット情報を取得する。
	 * 
	 * @param filepath 読み込むWAVファイルのパス
	 */
	void Load(const std::filesystem::path& filepath) override;

	/**
	 * @brief WAV音声の再生
	 * 
	 * 読み込んだWAV音声データをXAudio2で再生する。
	 * 
	 * @param xAudio XAudio2インスタンス
	 * @param isLoop ループ再生するかどうか
	 * @return 再生ハンドル
	 */
	int Play(IXAudio2* xAudio, bool isLoop) override;

private:

};

