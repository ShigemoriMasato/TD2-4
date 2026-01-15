#pragma once
#include <string>
#include <xaudio2.h>
#include <filesystem>
#include <fstream>
#include <array>
#include <wrl.h>

enum class AudioType {
	wav,
	mp3
};

/**
 * @struct ChunkHeader
 * @brief WAVファイルのチャンクヘッダ情報
 * 
 * RIFFフォーマットの各チャンクに共通するヘッダ構造。
 */
struct ChunkHeader {
	char id[4];   ///< チャンクID(4文字のASCII)
	int32_t size; ///< チャンクのデータサイズ(バイト)
};

/**
 * @struct RiffHeader
 * @brief WAVファイルのRIFFヘッダ
 * 
 * WAVファイル全体を示すRIFFチャンクのヘッダ情報。
 */
struct RiffHeader {
	ChunkHeader chunk; ///< RIFFチャンクヘッダ
	char type[4];      ///< ファイルタイプ("WAVE")
};

/**
 * @struct FormatChunk
 * @brief WAVファイルのフォーマットチャンク
 * 
 * 音声データの形式(サンプリングレート、ビット深度等)を定義する。
 */
struct FormatChunk {
	ChunkHeader chunk;  ///< fmtチャンクヘッダ
	WAVEFORMATEX fmt;   ///< 波形フォーマット情報
};

/**
 * @class AudioData
 * @brief 音声データの基底クラス
 * 
 * 音声ファイルの読み込み、再生、停止などの基本機能を提供する抽象クラス。
 * 派生クラスで各フォーマット固有の読み込みと再生処理を実装する。
 */
class AudioData {
public:

	AudioData() = default;
	~AudioData();

	/**
	 * @brief 音声ファイルの読み込み(純粋仮想関数)
	 * 
	 * 派生クラスで各フォーマットに応じた読み込み処理を実装する。
	 * 
	 * @param filePath 読み込むファイルのパス
	 */
	virtual void Load(const std::filesystem::path& filePath) = 0;

	/**
	 * @brief 音量の設定
	 * 
	 * @param volume 音量(0.0f～1.0f以上)
	 */
	void SetVolume(float volume);

	/**
	 * @brief 読み込んだ音声データを再生(純粋仮想関数)
	 * 
	 * 派生クラスで各フォーマットに応じた再生処理を実装する。
	 * 
	 * @param xAudio XAudio2インスタンス
	 * @param isLoop ループ再生するかどうか
	 * @return 再生ハンドル
	 */
	virtual int Play(IXAudio2* xAudio, bool isLoop) = 0;

	/**
	 * @brief 指定ハンドルの音声が再生中かどうか
	 * 
	 * @param handle 再生ハンドル
	 * @return 再生中の場合true
	 */
	bool IsPlay(int handle);

	/**
	 * @brief 再生の停止
	 * 
	 * @param handle 再生ハンドル
	 */
	void Stop(int handle);

protected:

	/// @brief 波形フォーマット情報
	WAVEFORMATEX wfex_;
	/// @brief 音声データバッファの先頭アドレス
	std::unique_ptr<BYTE[]> pBuffer_;
	/// @brief バッファのサイズ(バイト)
	unsigned int bufferSize_;
	/// @brief 音声データの名前
	std::string name_;

	/// @brief 音声データの形式
	AudioType type_;

	/// @brief 同時再生可能な最大数
	static const int resourceNum_ = 8;

	/// @brief 通常再生用のソースボイス配列
	std::array<IXAudio2SourceVoice*, resourceNum_> playResource_ = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};
	/// @brief ループ再生用のソースボイス
	IXAudio2SourceVoice* loopPlayResource_ = 0;
	/// @brief 次に使用する再生リソースのインデックス
	int playResourceIndex_ = 0;

	/// @brief 音量(デフォルト1.0f)
	float volume_ = 1.0f;

};
